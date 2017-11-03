#include "customer_queue.h"
#include "customers.h"
#include "utils.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_CLERKS 2
#define NUM_QUEUES 4

CustomerQueue *queues[NUM_QUEUES];
int total_customers_remaining = 0;
static struct timeval simulation_start_time;
double total_waiting_time = 0;

// Mutexes and Condition variables
pthread_mutex_t queue_lock;
pthread_mutex_t time_lock;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

// Start Lock Section
// Must have lock on queue_lock before calling these methods

void print_queue_lengths() {
  printf("\n");
  int i;
  for (i = 0; i < NUM_QUEUES; i += 1) {
    printf("Queue %d length is %d\n", i, queue_count(queues[i]));
  }
  printf("\n");
}

void get_shortest_queue(CustomerQueue **queue, int *index, int *length) {
  int shortest_length = -1;
  int shortest_queue_index = 0;

  int i;
  for (i = 0; i < NUM_QUEUES; i += 1) {
    if (queue_count(queues[i]) < shortest_length || shortest_length == -1) {
      shortest_length = queue_count(queues[i]);
      shortest_queue_index = i;
    }
  }

  *queue = queues[shortest_queue_index];
  *index = shortest_queue_index;
  *length = shortest_length;
}

void get_longest_queue(CustomerQueue **queue, int *index, int *length) {
  int longest_length = -1;
  int longest_queue_index = 0;

  int i;
  for (i = 0; i < NUM_QUEUES; i += 1) {
    if (queue_count(queues[i]) > longest_length || longest_length == -1) {
      longest_length = queue_count(queues[i]);
      longest_queue_index = i;
    }
  }

  *queue = queues[longest_queue_index];
  *index = longest_queue_index;
  *length = longest_length;
}

// End Lock Section

void enqueue_customer(Customer *customer) {
  int index;
  int length;
  CustomerQueue *queue;

  pthread_mutex_lock(&queue_lock);

  total_customers_remaining -= 1;
  get_shortest_queue(&queue, &index, &length);
  enqueue(queue, customer);
  pthread_cond_signal(&queue_cond);

  // print_queue_lengths();

  printf("A customer enters a queue: the queue ID %1d, and length of the queue "
         "%1d. \n",
         index, length);

  pthread_mutex_unlock(&queue_lock);
}

void process_customer(Customer *customer, int clerk_id) {

  pthread_mutex_lock(&time_lock);
  double start_time = get_current_simulation_time(simulation_start_time);
  pthread_mutex_unlock(&time_lock);

  printf("A clerk starts serving a customer: start time %.2f, the customer ID "
         "%2d, "
         "the clerk ID %1d. \n",
         start_time, customer->id, clerk_id);

  usleep(customer->service_time * 100000);

  pthread_mutex_lock(&time_lock);
  double end_time = get_current_simulation_time(simulation_start_time);
  total_waiting_time += end_time - start_time;
  pthread_mutex_unlock(&time_lock);

  printf("A clerk finishes serving a customer: end time %.2f, the customer ID "
         "%2d, "
         "the clerk ID %1d. \n",
         end_time, customer->id, clerk_id);
}

void *customer_thread(void *customer_pointer) {
  Customer *customer = (Customer *)customer_pointer;
  // printf("Customer thread %d\n", customer->id);

  // Wait to arrive
  usleep(customer->arrival_time * 100000);

  printf("A customer arrives: customer ID %2d. \n", customer->id);
  enqueue_customer(customer);

  // This thread can end, clerk thread will handle customer now

  return NULL;
}

void *clerk_thread(void *clerk_id_pointer) {
  int clerk_id = *((int *)clerk_id_pointer);
  // printf("I am clerk thread %d\n", clerk_id);

  while (TRUE) {
    Customer *customer;

    int index;
    int length;
    CustomerQueue *queue;

    pthread_mutex_lock(&queue_lock);

    // Get the customer in the longest queue
    get_longest_queue(&queue, &index, &length);

    // No customers in queue, wait on condition variable
    customer = dequeue(queue);

    while (customer == NULL) {
      if (total_customers_remaining <= 0) {
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&queue_lock);
        return;
      }

      pthread_cond_wait(&queue_cond, &queue_lock);
      customer = dequeue(queue);
    }

    // print_queue_lengths();
    pthread_mutex_unlock(&queue_lock);

    process_customer(customer, clerk_id);
  }

  return NULL;
}

void create_queues() {
  int i;
  for (i = 0; i < NUM_QUEUES; i += 1) {
    queues[i] = create_queue();
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Filename of customers required.\n");
    return;
  } else if (argc > 2) {
    printf("Too many arguments provided.\n");
    return;
  }

  CustomerQueue *customers;
  parse_customers(argv[1], &customers);

  int num_customers = queue_count(customers);
  total_customers_remaining = num_customers;

  // Debug Info
  // printf("There are %d customers\n", num_customers);
  // print_queue(customers);
  // printf("\n");

  // Record simulation start time
  gettimeofday(&simulation_start_time, NULL); // record simulation start time

  // Init pthread
  if (pthread_mutex_init(&queue_lock, NULL) != 0) {
    printf("\n mutex init failed\n");
    return 2;
  }

  // Create queues
  int i;
  for (i = 0; i < NUM_QUEUES; i += 1) {
    queues[i] = create_queue();
  }

  // Create clerk threds
  pthread_t clerks_threads[NUM_CLERKS];
  for (i = 0; i < NUM_CLERKS; i += 1) {
    int *clerk_id =
        (int *)malloc(sizeof(int)); // Force compiler to create new variable
    *clerk_id = i;
    if (pthread_create(&clerks_threads[i], NULL, clerk_thread, clerk_id)) {
      fprintf(stderr, "Error creating clerk thread %d\n", i);
      return 2;
    }
  }

  // Create customer threads
  pthread_t customer_threads[num_customers];
  Customer *c = dequeue(customers);
  i = 0;
  while (c != NULL) {
    if (pthread_create(&customer_threads[i], NULL, customer_thread, c)) {
      fprintf(stderr, "Error creating customer thread %d\n", c->id);
      return 2;
    }
    c = dequeue(customers);
    i += 1;
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
      fprintf(stderr, "Error joining customer thread\n");
      return 2;
    }
  }

  // Destroy muxtexes and condition variables
  pthread_mutex_destroy(&queue_lock);

  double average_waiting_time = total_waiting_time / num_customers;
  printf("The average waiting time for all customers in the system is: %.2f "
         "seconds. \n",
         average_waiting_time);

  return 1;
}