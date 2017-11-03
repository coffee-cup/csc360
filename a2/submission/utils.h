#ifndef UTILS
#define UTILS

#include "customer_queue.h"
#include "customers.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0

void parse_customers(char *filename, CustomerQueue **customers_queue);

double get_current_simulation_time(struct timeval start_time);

#endif