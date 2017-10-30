//#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#define NTHREADS 3
#define TARGET 6

void *thread_function(void *);                    // adding thread
void *watch(void *);                              // notification thread
int counter = 0;                                  // a counter
pthread_mutex_t lock;                             // mutex declaration
pthread_cond_t convar = PTHREAD_COND_INITIALIZER; // convar initialization

int main() {
  pthread_t thread_id[NTHREADS];
  int i, j, err;

  if (pthread_mutex_init(&lock, NULL) != 0) { // mutex initialization
    printf("\n mutex init failed\n");
    return 1;
  }

  for (i = 0; i < NTHREADS - 1; i++) { // create threads array
    if (pthread_create(&thread_id[i], NULL, thread_function, NULL) != 0)
      printf("can't create thread %d\n", i);
  }
  if (pthread_create(&thread_id[NTHREADS], NULL, watch, NULL) != 0)
    printf("can't create thread %d\n", NTHREADS);

  for (j = 0; j < NTHREADS - 1; j++) // wait for completion of all threads
    pthread_join(thread_id[j], NULL);
  pthread_join(thread_id[NTHREADS], NULL);
  printf("Main thread exits !\n");

  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&convar);
  return 0;
}

void *thread_function(void *dummyPtr) {
  int i;

  for (i = 0; i < 5; i++) {
    pthread_mutex_lock(&lock); // lock critical section

    counter++;
    printf("thread-%ld: counter is added to %2d\n", (long)pthread_self(),
           counter);
    if (counter == TARGET)
      pthread_cond_signal(&convar); // signal convar

    pthread_mutex_unlock(&lock); // unlock critical section
    sleep(1);
  }
  pthread_exit(NULL);
}

void *watch(void *arg) {
  pthread_mutex_lock(&lock);

  // while (counter < TARGET) {
  pthread_cond_wait(
      &convar,
      &lock); // release mutex(lock), wait on convar, until it is signaled
  printf("thread-%ld: counter reaches: %d\n", (long)pthread_self(), counter);
  // }

  pthread_mutex_unlock(&lock);

  printf("start timer\n");
  usleep(5000000);
  printf("end timer\n");
  return ((void *)0);
}