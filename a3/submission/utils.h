#ifndef UTILS
#define UTILS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FALSE (0)
#define TRUE (1)

typedef struct _DosTime DosTime;
typedef struct _DosDate DosDate;

struct _DosTime {
  unsigned int two_secs : 5;
  unsigned int minutes : 6;
  unsigned int hours : 5;
} __attribute__((packed));

struct _DosDate {
  unsigned int date : 5;
  unsigned int month : 4;
  unsigned int year : 7;
} __attribute__((packed));

void format_time(char *s, DosTime *time);

void format_date(char *s, DosDate *date);

// Copy num_bytes at location from file fp1 to file fp2
void copy_bytes(int num_bytes, int location, FILE *fp1, FILE *fp2);

#endif