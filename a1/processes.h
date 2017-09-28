#ifndef PROCESSES
#define PROCESSES

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

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

// Kill a process and remove it from the chain
// Returns new head
Process *kill_process(Process *head, pid_t pid);

// Prints out a list of the processes
void list_processes(Process *head);

// Returns the number of processes in the chain
int count_processes(Process *head);

// Removes zombie processes from the chain
// Returns the new chain
Process *remove_zombies(Process *head);

#endif