#include "fat12.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void print_dir_entry(DirEntry *direntry) {
  char type = 'F';
  if (direntry->attributes & SUBDIR) {
    type = 'D';
  }

  // filename
  char filename[12 + 1];
  sprintf(filename, "%s.%s", direntry->name, direntry->ext);

  // formatted date
  char date_s[13];
  format_date(date_s, &direntry->creation_date);

  // formatted time
  char time_s[6];
  format_time(time_s, &direntry->creation_time);

  printf("%c %10d %20s %s %s\n", type, direntry->file_size, filename, date_s,
         time_s);
}

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 2) {
    printf("filename of disk image is required.\n");
    exit(1);
  }

  char *filename = argv[1];
  Fat12 *fat12 = create_fat_struct(filename);
  read_disk_info(fat12);

  int index = 0;
  int status;

  while (1) {
    DirEntry *direntry;
    status = get_root_directory_entry(&direntry, index, fat12);

    if (status == -1) {
      break;
    }

    if (direntry != NULL) {
      print_dir_entry(direntry);
    }

    index += 1;
  }

  destroy_fat_struct(fat12);
}