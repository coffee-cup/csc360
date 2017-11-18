
     _    ____ ____
    / \  / ___/ ___|
   / _ \| |   \___ \
  / ___ \ |___ ___) |
 /_/   \_\____|____/

======================

Welcome to ACS! A C program for scheduling and processing customers.

Compiling the Code
------------------

$ make

This creates the ACS executable.

Running the Code
----------------

$ ./ACS customers.txt

A customers.txt file has been included with this assignment package.

Files
-----

- main.c : The main function where all mutexes, condition variables, and threads are handled.
- customers.h/.c : Struct and functions for dealing with customer data.
- customer_queue.h/.c : Structs and functions for dealing with a queue of customers. The queue is implemented as a linked list.
- utils.h/.c : Utility functions for parsing the customer file and dealing with time

Design Changes
--------------

I have made some changes to my original design. This section will explain what those changes are. Anything
in my design not mentioned has remained the same.

I originally had said I would use only 4 threads (2 clerk threads, a customer managing thread, and the main thread).

I know use 2 clerk threads, a thread for each customer, and the main thread. Instead of sorting the customers by arrival time,
I just create a thread for each customer and have that thread usleep until they have arrived. This is a better approach as
it makes handling cases when multiple customers arrive at the same time easier to work with.

Another change with my design is that I now calculate average waiting time correctly. I was originally calculating
average service time. To calculate average waiting time I added a field in the Customer struct to record the customers
arrival time (double start_wait_time;). This is populated when the customer first arrives. The wait time is calculated by
comparing this value to the simulation time when a clerk thread starts to service a customer.

Design Differences
------------------

My design is a bit different to the design suggested in the tutorial.

Instead of having each customer thread wait for their service time, the clerk threads will grab a customer
from the longest queue and will then wait for that customers service time. After the customer has arrived,
the customer will add itself to the shortest queue. Then that customer thread can end since it is not needed anymore.

This allows me to only need a single condition variable. This condition variable will wake up a clerk thread after
it has been waiting with no customers.