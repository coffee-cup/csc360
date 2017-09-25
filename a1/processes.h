#ifndef PROCESSES
#define PROCESSES

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct _Process Process;

// Create new process and place at beginning of chain
// Returns new head
Process *create_process(Process *head, pid_t pid, char *command);

// Get a process with specific pid
// Returns *Process
Process *get_process(Process *head, pid_t pid);

// Delete a process with specific pid from the chain
// Returns new head
Process *delete_process(Process *head, pid_t pid);

#endif