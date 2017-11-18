#include "utils.h"

double get_current_simulation_time(struct timeval start_time) {
  struct timeval cur_time;
  double cur_secs, init_secs;

  init_secs = (start_time.tv_sec + (double)start_time.tv_usec / 1000000);

  gettimeofday(&cur_time, NULL);
  cur_secs = (cur_time.tv_sec + (double)cur_time.tv_usec / 1000000);

  return cur_secs - init_secs;
}

void parse_customers(char *filename, CustomerQueue **customers_queue) {
  int num_customers = 0;

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file %s\n", filename);
    exit(1);
  }

  char line[256];

  fgets(line, sizeof(line), fp);
  sscanf(line, "%d", &num_customers);

  CustomerQueue *customers = create_queue();

  int i = 0;
  while (fgets(line, sizeof(line), fp) && i < num_customers) {
    int id;
    int arrival_time;
    int service_time;
    sscanf(line, "%d:%d,%d", &id, &arrival_time, &service_time);

    if (arrival_time < 0 || service_time < 0) {
      printf("Invalid times for customer %d -- ignoring\n", id);
    } else {
      Customer *c = create_customer(id, arrival_time, service_time);
      enqueue(customers, c);
      i += 1;
    }
  }
  fclose(fp);

  *customers_queue = customers;
}