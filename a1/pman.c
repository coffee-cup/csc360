#include "processes.h"
#include "utils.h"
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

Process *background_task(Process *head, char *args[]) {
  // TODO: Wait on zombie processes here
  pid_t pid = fork();
  if (pid >= 0) {
    if (pid == 0) {
      // Child process
      if (execvp(args[0], args) < 0) {
        perror("Error on execvp");
      }

      exit(EXIT_FAILURE);
    } else {
      // Parent process
      printf("Created a new child process with pid %d\n", pid);
      return create_process(head, pid, args[0]);
    }
  } else {
    perror("fork\n"); /* display error message */
    exit(EXIT_FAILURE);
  }
}

int main() {
  char *input = NULL;
  char *prompt = "PMan: >";
  int status;

  Process *head = NULL;

  char cont = 'y'; // y - continue

  char *command;
  char **args;

  while (1) {
    input = readline(prompt);
    if (strcasecmp(input, "") == 0)
      continue;

    parse_input(input, &command, &args);

    // printf("command: %s\n", command);
    // printf("args: ");
    int j = 0;
    while (args[j] != NULL) {
      printf("%s ", args[j]);
      j += 1;
    }
    printf("\n");

    if (command_compare("bg", command)) {
      head = background_task(head, args);
    } else if (command_compare("bglist", command)) {
      list_processes(head);
    } else if (command_compare("bgkill", command)) {
      printf("Kill\n");
    } else if (command_compare("bgstop", command)) {
      printf("Stop\n");
    } else if (command_compare("bgstart", command)) {
      printf("Start\n");
    } else if (command_compare("pstat", command)) {
      printf("Stats\n");
    } else {
      printf("%s:  command not found\n", command);
    }
  }

  return 1;
}