#include "utils.h"

void format_time(char *s, DosTime *time) {
  sprintf(s, "%02d:%02d", time->hours, time->minutes);
}

void format_date(char *s, DosDate *date) {
  sprintf(s, "%d-%02d-%02d", date->year + 1980, date->month, date->date);
}