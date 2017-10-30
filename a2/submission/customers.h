#ifndef CUSTOMERS
#define CUSTOMERS

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct _Customer Customer;

Customer *create_customer(int id, int arrival_time, int service_time);

void print_customer(Customer *c);

#endif