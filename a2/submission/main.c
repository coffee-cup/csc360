#include "customer_queue.h"
#include "customers.h"
#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_CLERKS 2

void *customer_thread(void *customer_pointer) {
  Customer *customer = (Customer *)customer_pointer;
  printf("Customer thread %d\n", customer->id);
  return NULL;
}

void *clerk_thread() {
  printf("I am clerk thread\n");
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Filename of customers required.\n");
    return;
  } else if (argc > 2) {
    printf("Too many arguments provided.\n");
    return;
  }

  int num_customers = 0;

  FILE *fp = fopen(argv[1], "r");
  char line[256];

  fgets(line, sizeof(line), fp);
  sscanf(line, "%d", &num_customers);

  Customer *customers[num_customers];

  int i = 0;
  while (fgets(line, sizeof(line), fp)) {
    int id;
    int arrival_time;
    int service_time;
    sscanf(line, "%d:%d,%d", &id, &arrival_time, &service_time);

    if (arrival_time < 0 || service_time < 0) {
      printf("Invalid times for customer %d -- ignoring\n", id);
    } else {
      Customer *c = create_customer(id, arrival_time, service_time);
      customers[i] = c;
      i += 1;
    }
  }
  fclose(fp);

  for (i = 0; i < num_customers; i += 1) {
    print_customer(customers[i]);
  }
  printf("\n");

  // Create clerk threds
  pthread_t clerks_threads[NUM_CLERKS];
  for (i = 0; i < NUM_CLERKS; i += 1) {
    if (pthread_create(&clerks_threads[i], NULL, clerk_thread, NULL)) {
      fprintf(stderr, "Error creating clerk thread %d\n", i);
      return 2;
    }
  }

  // Create customer threads
  pthread_t customer_threads[num_customers];
  for (i = 0; i < num_customers; i += 1) {
    if (pthread_create(&customer_threads[i], NULL, customer_thread,
                       customers[i])) {
      fprintf(stderr, "Error creating customer thread %d\n", customers[i]->id);
      return 2;
    }
  }

  // Wait on all threads to return
  for (i = 0; i < NUM_CLERKS; i += 1) {
    if (pthread_join(clerks_threads[i], NULL)) {
      fprintf(stderr, "Error joining clerk thread %d\n", i);
      return 2;
    }
  }
  for (i = 0; i < num_customers; i += 1) {
    if (pthread_join(customer_threads[i], NULL)) {
      fprintf(stderr, "Error joining customer thread %d\n", customers[i]->id);
      return 2;
    }
  }

  return 1;
}