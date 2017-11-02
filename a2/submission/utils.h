#ifndef UTILS
#define UTILS

#include "customer_queue.h"
#include "customers.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0

double get_current_simulation_time(struct timeval start_time);

#endif