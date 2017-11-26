#include "utils.h"

void format_time(char *s, DosTime *time) {
  sprintf(s, "%02d:%02d", time->hours, time->minutes);
}

void format_date(char *s, DosDate *date) {
  sprintf(s, "%d-%02d-%02d", date->year + 1980, date->month, date->date);
}

void copy_bytes(int num_bytes, int from_location, int to_location,
                FILE *from_fp, FILE *to_fp) {
  char buffer[num_bytes];

  fseek(from_fp, from_location, SEEK_SET);
  fread(buffer, num_bytes, 1, from_fp);

  fseek(to_fp, to_location, SEEK_SET);
  fwrite(buffer, num_bytes, 1, to_fp);
}

void uppercase_string(char *s) {
  char *c = s;
  while (*c) {
    if (*c != '.' && *c != '\0') {
      *c = toupper((unsigned char)*c);
    }
    c++;
  }
}

int get_filesize(FILE *fp) {
  int pos_start = ftell(fp);

  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);

  fseek(fp, pos_start, SEEK_SET);
  return size;
}