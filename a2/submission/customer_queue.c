#include "customer_queue.h"

struct _CustomerQueue {
  struct _Node *head;
  struct _Node *rear;
  int count;
};

struct _Node {
  struct _Customer *customer;
  struct _Node *next;
};

CustomerQueue *create_queue() {
  CustomerQueue *queue = (CustomerQueue *)malloc(sizeof(CustomerQueue));
  queue->head = NULL;
  queue->rear = NULL;
  queue->count = 0;

  return queue;
}

void *enqueue(CustomerQueue *queue, Customer *c) {
  Node *temp = (Node *)malloc(sizeof(Node));
  temp->customer = c;
  temp->next = NULL;
  if (queue->head == NULL && queue->rear == NULL) {
    queue->head = queue->rear = temp;
  } else {
    queue->rear->next = temp;
    queue->rear = temp;
  }
  queue->count += 1;

  return queue;
}

Customer *dequeue(CustomerQueue *queue) {
  Customer *c = NULL;
  if (queue->head == NULL) { // Queue is empty
    return NULL;
  }

  c = queue->head->customer;
  queue->count -= 1;

  if (queue->head == queue->rear) { // Only 1 node in queue
    queue->head = queue->rear = NULL;
  } else {
    queue->head = queue->head->next;
  }

  return c;
}

int queue_count(CustomerQueue *queue) { return queue->count; }

void print(CustomerQueue *queue) {
  Node *curr = queue->head;
  Customer *c;

  while (curr != NULL) {
    print_customer(curr->customer);
    curr = curr->next;
  }
}
