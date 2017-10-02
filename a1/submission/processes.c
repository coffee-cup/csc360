#include "processes.h"

struct _Process {
  pid_t pid;             /* The process ID */
  char *command;         /* The command the process is running */
  struct _Process *next; /* Pointer to next process in linked list */
};

struct _Status {
  pid_t pid;           /* The process ID */
  char comm[1000];     /* The filename of the executable */
  char state;          /* One character from the string RSDZTW */
  unsigned long utime; /* Amount of time that this process has been scheduled in
                          user mode */
  unsigned long
      stime; /* Amount of time that this process has been scheduled in kernel
                mode */
  long int
      rss; /* Resident Set Size: number of pages process has in real memory */
  int voluntary_ctxt_switches;    /* Number of voluntary context switches */
  int nonvoluntary_ctxt_switches; /* Number of involuntary context switches */
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
  while (curr != NULL && curr->pid != pid) {
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
    prev->next = NULL;
  } else { // deleting from the middle
    prev->next = curr->next;
  }

  free(curr);
  return head;
}

Process *kill_process(Process *head, pid_t pid) {
  if (get_process(head, pid) == NULL) {
    printf("Error: Process %d does not exist.\n", pid);
    return head;
  }

  if (kill(pid, SIGTERM) == 0) {
    usleep(1000); /* Wait for process to finish */
    return remove_zombies(head);
  } else {
    perror("Error killing process");
    printf("\n");
    return head;
  }
}

void *stop_process(Process *head, pid_t pid) {
  if (get_process(head, pid) == NULL) {
    printf("Error: Process %d does not exist.\n", pid);
    return head;
  }

  if (kill(pid, SIGSTOP) != 0) {
    perror("Error stopping process\n");
    printf("\n");
  }
}

void *start_process(Process *head, pid_t pid) {
  if (get_process(head, pid) == NULL) {
    printf("Error: Process %d does not exist.\n", pid);
    return head;
  }

  if (kill(pid, SIGCONT) != 0) {
    perror("Error starting process");
    printf("\n");
  }
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
      printf("Process %d has terminated\n", curr->pid);
      head = delete_process(head, curr->pid);
      curr = head;
    } else {
      curr = curr->next;
    }
  }
  return head;
}

void print_process_status(Process *head, pid_t pid) {
  if (get_process(head, pid) == NULL) {
    printf("Error: Process %d does not exist.\n", pid);
    return;
  }

  // Create structure to hold status information
  Status *s = (Status *)malloc(sizeof(Status));

  // Read from /proc/PID/stat
  char stat_filename[1000];
  sprintf(stat_filename, "/proc/%d/stat", pid);
  FILE *fstat = fopen(stat_filename, "r");
  if (fstat == NULL) {
    printf("Error: Failed to read from %s\n", stat_filename);
    return;
  }

  int unused_d;
  unsigned int unused_u;
  unsigned long unused_lu;
  char unused_s[100];

  fscanf(fstat, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld "
                "%ld %ld %ld %ld %ld %lu %ld",
         &s->pid,    // (1) pid
         s->comm,    // (2) comm
         &s->state,  // (3) state
         &unused_d,  // (4) ppid
         &unused_d,  // (5) pgrp
         &unused_d,  // (6) session
         &unused_d,  // (7) tty_nr
         &unused_d,  // (8) tpgid
         &unused_u,  // (9) flags
         &unused_lu, // (10) minflt
         &unused_lu, // (11) cminflt
         &unused_lu, // (12) majflt
         &unused_lu, // (13) cmajflt
         &s->utime,  // (14) utime
         &s->stime,  // (15) stime
         &unused_lu, // (16) cutime
         &unused_lu, // (17) cstime
         &unused_lu, // (18) priority
         &unused_lu, // (19) nice
         &unused_lu, // (20) num_threads
         &unused_lu, // (21) itrealvalue
         &unused_lu, // (22) starttime
         &unused_lu, // (23) vsize
         &s->rss     // (24) rss
         );
  fclose(fstat);

  // Read from /proc/PID/status
  char status_filename[1000];
  sprintf(status_filename, "/proc/%d/status", pid);
  FILE *fstatus = fopen(status_filename, "r");
  if (fstatus == NULL) {
    printf("Error: Failed to read from %s\n", status_filename);
    return;
  }

  char *line = NULL;
  size_t len = 0;
  int i = 0;
  while (getline(&line, &len, fstatus) != -1) {
    i += 1;
    if (i == 40) {
      // read voluntary_ctxt_switches
      sscanf(line, "%s %d", unused_s, &s->voluntary_ctxt_switches);
    } else if (i == 41) {
      // read nonvoluntary_ctxt_switches
      sscanf(line, "%s %d", unused_s, &s->nonvoluntary_ctxt_switches);
    }
  }
  fclose(fstatus);

  printf("\n");
  printf("pid:                        %d\n", s->pid);
  printf("comm:                       %s\n", s->comm);
  printf("state:                      %c\n", s->state);
  printf("utime:                      %.2f seconds\n",
         ((float)s->utime / sysconf(_SC_CLK_TCK)));
  printf("stime:                      %.2f seconds\n",
         ((float)s->stime / sysconf(_SC_CLK_TCK)));
  printf("rss:                        %ld\n", s->rss);
  printf("voluntary_ctxt_switches:    %d\n", s->voluntary_ctxt_switches);
  printf("nonvoluntary_ctxt_switches: %d\n", s->nonvoluntary_ctxt_switches);
  printf("\n");

  // Free the memory
  free(s);
  free(line);
}