
/* Includes */
#include <errno.h>     /* Errors */
#include <stdio.h>     /* Input/Output */
#include <stdlib.h>    /* General Utilities */
#include <sys/types.h> /* Primitive System Data Types */
#include <sys/wait.h>  /* Wait for Process Termination */
#include <unistd.h>    /* Symbolic Constants */

int main() {
  pid_t childpid, childpid2; /* variable to store the child's pid */
  int retval;                /* child process: user-provided return code */
  int status;                /* parent process: child's exit status */

  /* only 1 int variable is needed because each process would have its
     own instance of the variable
     here, 2 int variables are used for clarity */

  /* now create new process */
  childpid = fork();
  childpid2 = fork();

  if (childpid >= 0) /* fork succeeded */
  {
    if (childpid == 0) /* fork() returns 0 to the child process */
    {
      printf("CHILD: I am the child process!\n");
      printf("CHILD: Here's my PID: %d\n", getpid());
      printf("CHILD: My parent's PID is: %d\n", getppid());
      printf("CHILD: My copy of childpid is: %d \n", childpid);
      printf("CHILD: Sleeping for 1 second...\n");
      sleep(2); /* sleep for 1 second */
      printf("CHILD: Goodbye!\n\n");
      exit(retval); /* child exits with user-provided return code */
    } else          /* fork() returns new pid to the parent process */
    {
      printf("PARENT: I am the parent process!\n");
      printf("PARENT: Here's my PID: %d\n", getpid());
      printf("PARENT: my copy of childpid: %d \n", childpid);
      printf("PARENT: I will now wait for my child to exit.\n");
      printf("PARENT: Goodbye!\n");
      exit(5); /* parent exits */
    }
  } else /* fork returns -1 on failure */

  {
    perror("fork\n"); /* display error message */
    exit(0);
  }
}
