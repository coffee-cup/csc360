#include "utils.h"

void format_time(char *s, DosTime *time) {
  sprintf(s, "%02d:%02d", time->hours, time->minutes);
}

void format_date(char *s, DosDate *date) {
  sprintf(s, "%d-%02d-%02d", date->year + 1980, date->month, date->date);
}

void create_time_date_structs(DosTime **time_ptr, DosDate **date_ptr) {
  // Fat12 *fat12 = (Fat12 *)malloc(sizeof(Fat12));
  DosTime *time_struct = (DosTime *)malloc(sizeof(DosTime));
  DosDate *date_struct = (DosDate *)malloc(sizeof(DosDate));

  time_t t = time(NULL);
  struct tm *now = localtime(&t);

  // Time stuff
  int minute = now->tm_min;
  int hour = now->tm_hour;

  // Date struff
  int day = now->tm_mday;
  int month = (now->tm_mon + 1);
  int year = now->tm_year - 80;

  time_struct->minutes = minute;
  time_struct->hours = hour;

  date_struct->date = day;
  date_struct->month = month;
  date_struct->year = year;

  *time_ptr = time_struct;
  *date_ptr = date_struct;
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

int verify_filename(char *filename, char *name, char *ext) {
  int name_size = 0;
  int ext_size = 0;
  int count_name = TRUE;

  char *c = filename;
  while (*c) {
    if (count_name) {

      if (*c == '.') {
        count_name = FALSE;
      } else {
        name_size += 1;
        name[name_size - 1] = *c;
      }

    } else {
      if (*c != '\0') {
        ext_size += 1;
        ext[ext_size - 1] = *c;
      }
    }

    if (name_size > 8 || ext_size > 3) {
      return FALSE;
    }

    c++;
  }

  // Pad any empty space in name and ext with empty chars
  int i;
  for (i = name_size; i < 8; i += 1) {
    name[i] = ' ';
  }
  for (i = ext_size; i < 3; i += 1) {
    ext[i] = ' ';
  }

  return TRUE;
}
