#include "utils.h"

double get_current_simulation_time(struct timeval start_time) {
  struct timeval cur_time;
  double cur_secs, init_secs;

  init_secs = (start_time.tv_sec + (double)start_time.tv_usec / 1000000);

  gettimeofday(&cur_time, NULL);
  cur_secs = (cur_time.tv_sec + (double)cur_time.tv_usec / 1000000);

  return cur_secs - init_secs;
}