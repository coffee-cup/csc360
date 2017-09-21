#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void parse_input(char *input, char **command, char **args[]) {
  // Split input string on space
  char *del = " ";
  char *words[MAX_WORDS + 1];
  char *token;
  int i = 0;
  token = strtok(input, del);
  while (token != NULL) {
    words[i++] = token;
    if (i >= MAX_WORDS - 1) {
      break;
    }
    token = strtok(NULL, del);
  };
  words[i++] = NULL;

  // Command is first word
  *command = words[0];

  // Arguments are the rest of the words followed by NULL
  char *argv_execvp[i];
  int j = 1;
  while (words[j] != NULL) {
    argv_execvp[j - 1] = words[j];
    j += 1;
  }
  argv_execvp[j] = NULL;

  *args = argv_execvp;
}

int command_compare(char *c1, char *command) {
  return strcasecmp(c1, command) == 0;
}