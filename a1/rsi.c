#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
  char *input = NULL;
  char *prompt = "your command:";

  input = readline(prompt);

  printf("%s\n", input);
  return 1;
}
