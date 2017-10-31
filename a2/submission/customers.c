#include "customers.h"

Customer *create_customer(int id, int arrival_time, int service_time) {
  Customer *c = (Customer *)malloc(sizeof(Customer));
  c->id = id;
  c->arrival_time = arrival_time;
  c->service_time = service_time;

  return c;
}

void print_customer(Customer *c) {
  printf("%d: %d,%d\n", c->id, c->arrival_time, c->service_time);
}

size_t customer_size() { return sizeof(Customer); }

size_t customer_pointer_size() { return sizeof(Customer *); }