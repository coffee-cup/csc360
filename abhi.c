//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: CENG 355 "Microprocessor-Based Systems".
// Authors: Abhi Singh + Graeme Turney
// Lab Section: B05
//
// See "system/include/cmsis/stm32f0xx.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f0xx.c" for handler declarations.
// ----------------------------------------------------------------------------

#include "cmsis/cmsis_device.h"
#include "diag/Trace.h"
#include <stdio.h>

// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

// ===== Start of Defines =====

/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

// Set the LCD_LCK pin to PB7
#define LCD_LCK_PIN GPIO_Pin_7

// Start of defined LCD command values
#define LCD_ENABLE (0x80)
#define LCD_DISABLE (0x0)
#define LCD_COMMAND (0x0)
#define LCD_DATA (0x40)
#define LCD_CURSOR (0x80)
// End of defined LCD command values

#define ADC_MAX_VALUE                                                          \
  ((float)(0xFFF)) // ADC bit resolution, since DAC is 12, ADC must be too
#define DAC_MAX_VALUE ((float)(0xFFF)) // DAC bit resolution
#define DAC_MAX_VOLTAGE                                                        \
  (2.95) // measured output from PA4 when DAC->DHR12R1 is DAC_MAX_VALUE
#define RESISTANCE_MAX_VALUE 5000
#define DAC_MIN (1.0) // min voltage needed

// ===== End of Defines ======

// ===== Start of Prototypes =====

void myGPIOA_Init();
void myGPIOB_Init();
void myGPIOC_Init();
void myTIM2_Init();
void myEXTI_Init();
void digitalToAnalogConfig();
void analogToDigitalConfig();
uint32_t analogToDigitalRun();
void digitalToAnalogRun(uint32_t valueADC);
uint32_t normalizeForDAC(uint32_t rawDAC);

void waitForDelay(unsigned int waitCount);

void initializeSPI();
void SPI1to74HC595(uint8_t data);
void initializeLCD();
void LCD_writeCmd(uint8_t type, uint8_t cmd);
void LCD_writeDigit(int digit);
void LCD_moveCursor(uint8_t row, uint8_t column);
void LCD_updateResistance();
void LCD_updateFrequency();
void LCD_updateRow(int row, uint32_t value);

// ===== End of Prototypes =====

// ===== Start of Global Variables =====

int firstEdge = 1;

uint32_t resistanceOfPOT;
uint32_t frequencyOf555;
uint32_t normalResistance;

// ===== End of Global Variables =====

int main(int argc, char *argv[]) {

  trace_printf("CEng 355 Project\n");
  trace_printf("System clock: %u Hz\n", SystemCoreClock);

  trace_printf("Starting configuration...\n");

  // Configure and initialize all parts
  myGPIOA_Init(); // Initialize I/O port PA ---- Might be related to NE555 timer
  myGPIOB_Init(); // Initialize I/O port PC
  myGPIOC_Init(); // Initialize I/O port PC
  myTIM2_Init();  // Initialize timer TIM2
  myEXTI_Init();  // Initialize EXTI
  trace_printf("Config ADC\n");
  analogToDigitalConfig(); // Configure ADC
  trace_printf("Config DAC\n");
  digitalToAnalogConfig(); // Configure DAC
  trace_printf("Config SPI\n");
  initializeSPI(); // initialize SPI1
  trace_printf("Config LCD\n");
  initializeLCD(); // initialize the LCD

  trace_printf("All configurations and initializations complete.\n");

  while (1) {

    // trace_printf("Run ADC...\n");
    // Run ADC
    resistanceOfPOT = analogToDigitalRun();
    // trace_printf("%u\n", resistanceOfPOT);

    // trace_printf("Run DAC...\n");
    // Run DAC
    uint32_t normalizedDACValue = normalizeForDAC(resistanceOfPOT);
    digitalToAnalogRun(normalizedDACValue);

    // trace_printf("\n\nUPDATE UPDATE UPDATE\n\n");
    // Convert to resistance range
    float normalizedPotADC = ((float)resistanceOfPOT) / ADC_MAX_VALUE;
    normalResistance = normalizedPotADC * RESISTANCE_MAX_VALUE;
    // trace_printf("%u\n", normalResistance);
    // waitForDelay(4800);
    LCD_updateFrequency();
    LCD_updateResistance();
    waitForDelay(48000000 / 2); // Half second delay?
  }

  return 0;
}

// ========== Start of Configurations and ADC and DAC ================

// Initialize GPIOA for ADC and DAC
void myGPIOA_Init() {

  // PA1 ----> Input from NE555

  // PA4 ----> Output for DAC

  /* Enable clock for GPIOA peripheral */
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  /* Configure PA1 as input */
  GPIOA->MODER &= ~(GPIO_MODER_MODER1);
  /* Ensure no pull-up/pull-down for PA1 */
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

  // Analog Mode - PORT A4 for DAC
  GPIOA->MODER &= ~(GPIO_MODER_MODER4);
  // No pull up/pull down
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4);
}

// Configure GPIOC for the POT
void myGPIOC_Init() {

  // PC1 ----> Analog input for ADC resistance

  // GPIOC
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  // Analog Mode - PORT C1
  GPIOC->MODER &= ~(GPIO_MODER_MODER1);
  // No pull up/down
  GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR1);
}

// Initialize timer for interrupts
void myTIM2_Init() {

  /* Enable clock for TIM2 peripheral */
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  /* Configure TIM2: buffer auto-reload, count up, stop on overflow,
   * enable update events, interrupt on overflow only */
  TIM2->CR1 = ((uint16_t)0x008C);
  /* Set clock prescaler value */
  TIM2->PSC = myTIM2_PRESCALER;
  /* Set auto-reloaded delay */
  TIM2->ARR = myTIM2_PERIOD;

  /* Update timer registers */
  TIM2->EGR = ((uint16_t)0x0001);
  /* Assign TIM2 interrupt priority = 0 in NVIC */
  NVIC_SetPriority(TIM2_IRQn, 0);
  /* Enable TIM2 interrupts in NVIC */
  NVIC_EnableIRQ(TIM2_IRQn);
  /* Enable update interrupt generation */
  TIM2->DIER |= TIM_DIER_UIE;
}

// Initialize EXTI
void myEXTI_Init() {

  /* Map EXTI1 line to PA1 */
  SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI1_PA;
  /* EXTI1 line interrupts: set rising-edge trigger */
  EXTI->RTSR |= EXTI_RTSR_TR1;
  /* Unmask interrupts from EXTI1 line */
  EXTI->IMR |= EXTI_IMR_MR1;
  /* Assign EXTI1 interrupt priority = 0 in NVIC */
  NVIC_SetPriority(EXTI0_1_IRQn, 0);
  /* Enable EXTI1 interrupts in NVIC */
  NVIC_EnableIRQ(EXTI0_1_IRQn);
}

/* This handler is declared in system/src/cmsis/vectors_stm32f0xx.c */
void TIM2_IRQHandler() {

  /* Check if update interrupt flag is indeed set */
  if ((TIM2->SR & TIM_SR_UIF) != 0) {
    trace_printf("\n*** Overflow! ***\n");

    /* Clear update interrupt flag */
    TIM2->SR &= TIM_SR_UIF != 0;
    /* Restart stopped timer */
    TIM2->CR1 |= TIM_CR1_CEN;
  }
}

/* This handler is declared in system/src/cmsis/vectors_stm32f0xx.c */
void EXTI0_1_IRQHandler() {

  /* Check if EXTI1 interrupt pending flag is indeed set */
  if ((EXTI->PR & EXTI_PR_PR1) != 0) {

    // 1. If this is the first edge:
    if (firstEdge) {
      //	- Clear count register (TIM2->CNT).
      TIM2->CNT = (uint32_t)0x0;
      //	- Start timer (TIM2->CR1).
      TIM2->CR1 |= (TIM_CR1_CEN);
      // Set firstEdge to 0 so that on next rising edge we calculate things.
      firstEdge = 0;
      // trace_printf("Found first edge\n");
    }
    //    Else (this is the second edge):
    else {
      //	- Stop timer (TIM2->CR1).
      TIM2->CR1 &= ~(TIM_CR1_CEN);
      //	- Read out count register (TIM2->CNT).
      uint32_t count = TIM2->CNT;
      //	- Calculate signal period and frequency.
      frequencyOf555 = (SystemCoreClock / count);
      unsigned int period = (count) / 48;

      // float frequency = (SystemCoreClock/count);
      // float period = 1/frequency;
      //	- Print calculated values to the console.

      // Range is actually 1Hz to 1.8MHz
      // trace_printf("The timer count is: %u\n", count);
      // trace_printf("The signal period is: %ux10^-6\n", period);
      // trace_printf("The signal frequency is: %u\n", frequencyOf555);

      // trace_printf("The signal period is: %f\n", period);
      // trace_printf("The signal frequency is: %f\n", frequency);

      //	  NOTE: Function trace_printf does not work
      //	  with floating-point numbers: you must use
      //	  "unsigned int" type to print your signal
      //	  period and frequency.
      firstEdge = 1;

      // If only want to run once
      // exit(0);
    }

    // 2. Clear EXTI1 interrupt pending flag (EXTI->PR).
    EXTI->PR |= EXTI_PR_PR1;
  }
}

// Configure Digital to Analog (DAC) converter
void digitalToAnalogConfig() {

  // DAC Clock
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;
  // Enable DAC
  DAC->CR |= DAC_CR_EN1;
}

// Run Digital to Analog converter
void digitalToAnalogRun(uint32_t valueADC) {

  // write analog voltage
  DAC->DHR12R1 = valueADC;
}

// Configure Analog to Digital (ADC) converter
void analogToDigitalConfig() {

  // ADC Clock enable
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
  // Start calibration
  ADC1->CR |= ADC_CR_ADCAL;
  // Wait for calibration
  while (ADC1->CR == ADC_CR_ADCAL) {

    trace_printf("Waiting on ADC calibration...\n");
  }

  trace_printf("Calibration complete\n");
  // Enable ADC1
  ADC1->CR |= ADC_CR_ADEN;
  while (!(ADC1->ISR & ADC_ISR_ADRDY))
    ;
  // Set Continuous Mode
  ADC1->CFGR1 |= ADC_CFGR1_CONT;
  // Set Overrun Mode
  ADC1->CFGR1 |= ADC_CFGR1_OVRMOD;
  // Channel Select for PC1
  ADC1->CHSELR |= ADC_CHSELR_CHSEL11;
  // Set sampling rate?
  ADC1->SMPR;
}

// Run Analog to Digital converter
uint32_t analogToDigitalRun() {

  // Trigger conversion
  ADC1->CR |= ADC_CR_ADSTART;
  // Wait until conversion completes
  while (!(ADC1->ISR & ADC_ISR_EOC)) {
    // trace_printf("Waiting on ADC\n");
  }
  // Reset EOC flag
  ADC1->ISR &= ~(ADC_ISR_EOC);
  // Read converted value
  uint32_t value = (ADC1->DR & ADC_DR_DATA);

  // trace_printf("Value of ADC1->DR: %u\n", value);

  return value;
}

uint32_t normalizeForDAC(uint32_t rawDAC) {

  // normalize the DAC value
  float normalizedDAC = rawDAC / DAC_MAX_VALUE;
  float voltageRange = DAC_MAX_VOLTAGE - DAC_MIN;
  float voltage = (normalizedDAC * voltageRange) + DAC_MIN;

  // convert the voltage value back to a DAC level
  float normalizedVoltage = voltage / DAC_MAX_VOLTAGE;
  float outputDACValue = normalizedVoltage * DAC_MAX_VALUE;

  return ((uint32_t)outputDACValue);
}

// ==========  End of Configurations and ADC and DAC ================

// Our own "elegant" Delay Function
void waitForDelay(unsigned int waitCount) {

  unsigned int i;

  // wait for x many clock cycles
  for (i = 0; i < waitCount; i++) {

    // Send a NOP instruction to make sure that the for loop doesn't
    // get optimized out at compile time
    asm("nop");
  }
}

// ============= Start of LCD Code ================

// Configure GPIOB
void myGPIOB_Init() {

  // Turn on the GPIOB clock
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  // PB3 -----> SPI MOSI
  // PB5 -----> SPI SCK
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure the LCK pin for "manual" control in SPI1to74HC595
  GPIO_InitStruct.GPIO_Pin = LCD_LCK_PIN; // PB7
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// Initialize the SPI(1)
void initializeSPI() {

  // Enable SPI Clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  SPI_InitTypeDef SPI_InitStructInfo;
  SPI_InitTypeDef *SPI_InitStruct = &SPI_InitStructInfo;

  SPI_InitStruct->SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStruct->SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct->SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct->SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct->SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct->SPI_NSS = SPI_NSS_Soft;
  SPI_InitStruct->SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStruct->SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct->SPI_CRCPolynomial = 7;

  SPI_Init(SPI1, SPI_InitStruct);
  SPI_Cmd(SPI1, ENABLE);
}

// Initialize the LCD with the proper commands, then set up the "always on"
// letters
void initializeLCD() {

  // Set the LCD to 4 bit mode
  LCD_writeCmd(LCD_COMMAND, 0x2);
  waitForDelay(4800000);

  // 00xx 0010 1000 = 0x28
  // 00xx 0000 1100 = 0x0C
  // 00xx 0000 0110 = 0x06
  // 00xx 0000 0001 = 0x01 (clear display)

  LCD_writeCmd(LCD_COMMAND, 0x28);
  LCD_writeCmd(LCD_COMMAND, 0x0C);
  LCD_writeCmd(LCD_COMMAND, 0x06);
  LCD_writeCmd(LCD_COMMAND, 0x01); // Clear Display

  trace_printf("Now delay for LCD...\n");
  waitForDelay(4800000);

  // Display on LCD --> F:
  LCD_moveCursor(0, 0);
  LCD_writeCmd(LCD_DATA, 0x46); // F
  LCD_moveCursor(0, 1);
  LCD_writeCmd(LCD_DATA, 0x3A); // :
  // Display on LCD --> R:
  LCD_moveCursor(1, 0);
  LCD_writeCmd(LCD_DATA, 0x52); // R
  LCD_moveCursor(1, 1);
  LCD_writeCmd(LCD_DATA, 0x3A); // :
}

// Send data from the SPI1 to the HC595 LCD
void SPI1to74HC595(uint8_t data) {

  // Force LCK signal to 0
  GPIOB->BRR = LCD_LCK_PIN;
  // Wait until SPI1 is ready (TXE = 1 or BSY = 0)
  while (!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY))
    ;
  // Assumption: Data holds 8 bits to be sent
  SPI_SendData8(SPI1, data);
  // Wait until SPI1 is not busy (BSY = 0)
  while (SPI1->SR & SPI_SR_BSY)
    ;
  // Force LCK signal to 1
  GPIOB->BSRR = LCD_LCK_PIN;
}

// Write the specific command to the LCD, it is either DATA (display a
// character) or COMMAND
void LCD_writeCmd(uint8_t type, uint8_t cmd) {

  // To write to LCD, must issue the following commmands for both high and low
  // nibbles 0000 xxxx 1000 xxxx 0000 xxxx

  uint8_t high_nibble = (cmd >> 4) & 0x0F;
  uint8_t low_nibble = cmd & 0x0F;

  // Send the High nibble
  SPI1to74HC595(LCD_DISABLE | type | high_nibble);
  SPI1to74HC595(LCD_ENABLE | type | high_nibble);
  SPI1to74HC595(LCD_DISABLE | type | high_nibble);

  // Send the Low nibble
  SPI1to74HC595(LCD_DISABLE | type | low_nibble);
  SPI1to74HC595(LCD_ENABLE | type | low_nibble);
  SPI1to74HC595(LCD_DISABLE | type | low_nibble);
}

// Write the specific digit to the LCD
void LCD_writeDigit(int digit) {

  // Apply the correct ascii offset
  int asciiDigit = digit + 0x30;
  LCD_writeCmd(LCD_DATA, asciiDigit);
}

// Move the cursor to the specified row/column on the LCD
// Top row = 0, bottom row = 1, leftmost column = 0, rightmost column = 7
void LCD_moveCursor(uint8_t row, uint8_t column) {

  // Apply proper offset based on the row
  if (row == 1) {

    row = 0x40;
  }

  uint8_t cursorCmd = LCD_CURSOR | row | column;

  LCD_writeCmd(LCD_COMMAND, cursorCmd);
}

// Update the resistance display on the LCD
void LCD_updateResistance() { LCD_updateRow(1, normalResistance); }

// Update the frequency display on the LCD
void LCD_updateFrequency() { LCD_updateRow(0, frequencyOf555); }

// General update of row based on row number
void LCD_updateRow(int row, uint32_t value) {

  uint8_t ones = value % 10;
  uint8_t tens = (value / 10) % 10;
  uint8_t hundreds = (value / 100) % 10;
  uint8_t thousands = (value / 1000) % 10;

  LCD_moveCursor(row, 2);

  LCD_writeDigit(thousands);
  LCD_writeDigit(hundreds);
  LCD_writeDigit(tens);
  LCD_writeDigit(ones);

  // Add Hz for Frequency Row
  if (row == 0) {

    LCD_writeCmd(LCD_DATA, 0x48);
    LCD_writeCmd(LCD_DATA, 0x7A);

  }

  // Add Ohm symbol for resistance row
  else {

    LCD_writeCmd(LCD_DATA, 0xF4);
  }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------