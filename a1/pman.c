#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define MAX_WORDS 80

// Parses the input string into command and arguments
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

int main() {
  char *input = NULL;
  char *prompt = "PMan: >";
  int status;

  char cont = 'y'; // y - continue

  char *command;
  char **args;

  while (1) {
    input = readline(prompt);

    parse_input(input, &command, &args);

    printf("command: %s\n", command);
    printf("args: ");
    int j = 0;
    while (args[j] != NULL) {
      printf("%s ", args[j]);
      j += 1;
    }
    printf("\n");
  }

  return 1;
}