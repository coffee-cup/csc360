#include "utils.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
  char *input = NULL;
  char *prompt = "PMan: >";
  int status;

  char cont = 'y'; // y - continue

  char *command;
  char **args;

  while (1) {
    input = readline(prompt);
    if (strcasecmp(input, "") == 0)
      continue;

    parse_input(input, &command, &args);

    printf("command: %s\n", command);
    printf("args: ");
    int j = 0;
    while (args[j] != NULL) {
      printf("%s ", args[j]);
      j += 1;
    }
    printf("\n");

    if (command_compare("bg", command)) {
      printf("Background Task\n");
    } else if (command_compare("bglist", command)) {
      printf("List\n");
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