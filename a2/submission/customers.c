#include "customers.h"

struct _Customer {
  int id;           /* The ID of the customer */
  int arrival_time; /* Arrival time of the customer */
  int service_time; /* Service time of the customer */
};

Customer *create_customer(int id, int arrival_time, int service_time) {
  Customer *c = (Customer *)malloc(sizeof(Customer));
  c->id = id;
  c->arrival_time = arrival_time;
  c->service_time = service_time;

  return c;
}

void print_customer(Customer *c) {
  printf("%d: %d, %d\n", c->id, c->arrival_time, c->service_time);
}