#ifndef CUSTOMERQUEUE
#define CUSTOMERQUEUE

#include "customers.h"
#include <stdio.h>

typedef struct _CustomerQueue CustomerQueue;
typedef struct _Node Node;

// Create a new Customer queue
CustomerQueue *create_queue();

// Push Customer onto the queue
void *enqueue(CustomerQueue *queue, Customer *c);

// Get the next Customer in line
Customer *dequeue(CustomerQueue *queue);

// Get the number of customers in the queue
int queue_count(CustomerQueue *queue);

// Print the queue for readability
void print(CustomerQueue *queue);

#endif