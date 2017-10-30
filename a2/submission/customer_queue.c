#include "customer_queue.h"

struct _CustomerQueue {
  struct _Node *head;
  struct _Node *rear;
};

struct _Node {
  struct _Customer *customer;
  struct _Node *next;
};

CustomerQueue *create_queue() {
  CustomerQueue *queue = (CustomerQueue *)malloc(sizeof(CustomerQueue));
  queue->head = NULL;
  queue->rear = NULL;

  return queue;
}

void *enqueue(CustomerQueue *queue, Customer *c) {
  Node *temp = (Node *)malloc(sizeof(Node));
  temp->customer = c;
  temp->next = NULL;
  if (queue->head == NULL && queue->rear == NULL) {
    queue->head = queue->rear = temp;
    return;
  }
  queue->rear->next = temp;
  queue->rear = temp;

  return queue;
}

Customer *dequeue(CustomerQueue *queue) {
  Customer *c = NULL;
  if (queue->head == NULL) { // Queue is empty
    return NULL;
  } else if (queue->head == queue->rear) { // Only 1 node in queue
    c = queue->head->customer;
    queue->head = queue->rear = NULL;
  } else {
    c = queue->head->customer;
    queue->head = queue->head->next;
  }

  return c;
}

void print(CustomerQueue *queue) {
  Node *curr = queue->head;
  Customer *c;

  while (curr != NULL) {
    print_customer(curr->customer);
    curr = curr->next;
  }
}
