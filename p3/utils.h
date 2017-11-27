#ifndef UTILS
#define UTILS

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

// Creates time and date structs for the current time
void create_time_date_structs(DosTime **time_ptr, DosDate **date_ptr);

// Copy num_bytes at location from file fp1 to file fp2
void copy_bytes(int num_bytes, int from_location, int to_location,
                FILE *from_fp, FILE *to_fp);

// Converts a string to uppercase
void uppercase_string(char *s);

// Returns the size of a file pointer in bytes
int get_filesize(FILE *fp);

// Verifies a filename is correct (name + extension)
// Also, if filename is correct, splits into 8 char name and 3 char ext
int verify_filename(char *filename, char *name, char *ext);

#endif