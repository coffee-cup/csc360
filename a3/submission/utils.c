#include "utils.h"

void format_time(char *s, DosTime *time) {
  sprintf(s, "%02d:%02d", time->hours, time->minutes);
}

void format_date(char *s, DosDate *date) {
  sprintf(s, "%d-%02d-%02d", date->year + 1980, date->month, date->date);
}

void copy_bytes(int num_bytes, int location, FILE *fp1, FILE *fp2) {
  char buffer[num_bytes];

  fseek(fp1, location, SEEK_SET);
  fread(buffer, num_bytes, 1, fp1);
  fwrite(buffer, num_bytes, 1, fp2);
}