#include "customer_queue.h"
#include "customers.h"
// #include "utils.h"
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void file_process(const char *filename, Customer ***customers_pointer,
                  int *num_customers) {
  FILE *fp = fopen(filename, "r");
  char line[256];

  fgets(line, sizeof(line), fp);
  sscanf(line, "%d", num_customers);

  Customer *customers[*num_customers];
  // Customer *customers =
  //     (Customer *)malloc(customer_pointer_size() * *num_customers);
  int i = 0;

  while (fgets(line, sizeof(line), fp)) {
    int id;
    int arrival_time;
    int service_time;
    sscanf(line, "%d:%d,%d", &id, &arrival_time, &service_time);
    Customer *c = create_customer(id, arrival_time, service_time);
    customers[i] = c;
    i += 1;
  }

  // *customers_pointer = customers;
  for (i = 0; i < *num_customers; i += 1) {
    // print_customer(customers[i]);
  }

  printf("\n\n");

  fclose(fp);
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
    Customer *c = create_customer(id, arrival_time, service_time);
    customers[i] = c;
    i += 1;
  }
  fclose(fp);

  for (i = 0; i < num_customers; i += 1) {
    print_customer(customers[i]);
  }

  return 1;
}