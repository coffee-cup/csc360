#include "processes.h"

struct _Process {
  pid_t pid;
  char *command;
  struct _Process *next;
};

Process *create_process(Process *head, pid_t pid, char *command) {
  Process *p = (Process *)malloc(sizeof(Process));
  p->pid = pid;
  p->command = command;
  p->next = head;

  return p;
}

Process *get_process(Process *head, pid_t pid) {
  Process *curr = head;
  while (curr->pid != pid && curr != NULL) {
    curr = curr->next;
  }
  return curr;
}

Process *delete_process(Process *head, pid_t pid) {
  Process *curr = head;
  Process *prev = NULL;
  while (curr->pid != pid && curr->next != NULL) {
    prev = curr;
    curr = curr->next;
  }

  if (curr == head) { // deleting the head
    head = curr->next;
  } else if (curr->next == NULL) { // deleting from the end
    prev->next == NULL;
  } else { // deleting from the middle
    prev->next = curr->next;
  }

  free(curr);
  return head;
}