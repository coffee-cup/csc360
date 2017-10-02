  _____  __  __
 |  __ \|  \/  |
 | |__) | \  / | __ _ _ __
 |  ___/| |\/| |/ _` | '_ \
 | |    | |  | | (_| | | | |
 |_|    |_|  |_|\__,_|_| |_|

=============================

Welcome to PMan! A C program to create and manage processes.

Compiling the Code
------------------

$ make

This creates the PMan executable.

Running the Code
----------------

$ ./PMan

Commands
--------

The following commands are available in the PMan program.

- bg COMMAND ARGS
  + Runs COMMAND ARGS in a new process
- bglist
  + Lists all processes that have been backgrounded
- bgstop PID
  + Stops process with id PID
- bgstart PID
  + Starts process with id PID
- bgkill PID
  + Terminates process with id PID
- pstat PID
  + Shows status information for process with id PID

Files
-----

- pman.c : The main function and infinite loop where user input is accepted
- processes.h/.c : Structs and functions for dealing with processes and the linked list data structure
- utils.h/.c : Utility functions for parsing and comparing user input