#ifndef UTILS
#define UTILS

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_WORDS 80
#define TRUE 1
#define FALSE 0

/* Parses the input string into command and arguments */
void parse_input(char *input, char **command, char **args[], int *num_args);

/* Returns TRUE if c1 matches the command */
int command_compare(char *c1, char *command);

/* Prints an error if there are not enough arguments */
/* Returns 0 if the number of arguments is okay, -1 otherwise */
int check_args(int supplied, int required);

#endif