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

int count_processes(Process *head) {
  int count = 0;
  Process *curr = head;
  while (curr != NULL) {
    count += 1;
    curr = curr->next;
  }
  return count;
}

void list_processes(Process *head) {
  Process *curr = head;
  int count = 0;
  while (curr != NULL) {
    printf("%d:\t%s\n", curr->pid, curr->command);

    count += 1;
    curr = curr->next;
  }
  printf("Total background jobs: %d\n", count);
}

Process *remove_zombies(Process *head) {
  Process *curr = head;
  int status;

  while (curr != NULL) {
    int opts = WNOHANG;
    int retVal;

    retVal = waitpid(curr->pid, &status, opts);
    if (retVal == curr->pid) {
      // We need to remove this process
      head = delete_process(head, curr->pid);
      curr = head;
    } else {
      curr = curr->next;
    }
  }
  return head;
}