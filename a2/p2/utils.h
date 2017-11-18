#ifndef UTILS
#define UTILS

#include "customer_queue.h"
#include "customers.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0

// Parse the customer file into a queue of customer structs
void parse_customers(char *filename, CustomerQueue **customers_queue);

// Get the time in seconds since the simulation has started
double get_current_simulation_time(struct timeval start_time);

#endif