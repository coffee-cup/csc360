#include "fat12.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 3) {
    printf("disk image filename and root directory filename required.\n");
    exit(1);
  }

  char *disk_filename = argv[1];
  char *search_filename = argv[2];
  Fat12 *fat12 = create_fat_struct(disk_filename);
  read_boot_sector(fat12);

  int file_found = FALSE;

  int index = 0;
  int status;

  DirEntry *found_entry;

  while (1) {
    DirEntry *direntry;
    status = get_root_directory_entry(&direntry, index, fat12);

    if (status == -1) {
      break;
    }

    if (direntry != NULL) {
      // filename
      char filename[12 + 1];
      sprintf(filename, "%s.%s", direntry->name, direntry->ext);

      if (strcmp(filename, search_filename) == 0) {
        file_found = TRUE;
        found_entry = direntry;
        break;
      }
    }

    index += 1;
  }

  if (!file_found) {
    printf("File not found\n");
  } else {
    printf("File %s found. Start Cluster %d\n", search_filename,
           found_entry->first_logical_cluster);

    uint16_t next;
    if (!next_cluster(&next, 1, found_entry->first_logical_cluster, fat12)) {
      printf("No more clusters\n");
    } else {
      printf("\nnext cluster: %d - 0x%x\n", next, next);
    }
  }

  destroy_fat_struct(fat12);
}