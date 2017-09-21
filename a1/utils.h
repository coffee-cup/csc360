#ifndef UTILS
#define UTILS

#define MAX_WORDS 80
#define TRUE 1
#define FALSE 0

/* Parses the input string into command and arguments */
void parse_input(char *input, char **command, char **args[]);

/* Returns TRUE if c1 matches the command */
int command_compare(char *c1, char *command);

#endif