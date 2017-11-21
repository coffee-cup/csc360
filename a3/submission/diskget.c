#include "fat12.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Copy the directory entry to the file fp
void copy_found_file(DirEntry *direntry, Fat12 *fat12, FILE *fp,
                     char *search_filename) {
  int count = 0;
  uint16_t next = direntry->first_logical_cluster;

  while (1) {
    int physical_sector = (33 + next - 2) * 512;
    int bytes_to_copy = direntry->file_size - (count * SECTOR_SIZE);
    if (bytes_to_copy > SECTOR_SIZE) {
      bytes_to_copy = SECTOR_SIZE;
    }

    copy_bytes(bytes_to_copy, physical_sector, fat12->fp, fp);

    if (!next_cluster(&next, 1, next, fat12)) {
      break;
    }

    count += 1;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 3) {
    printf("disk image filename and root directory filename required.\n");
    exit(1);
  }

  char *disk_filename = argv[1];   // The FAT12 image file
  char *search_filename = argv[2]; // The file to copy
  Fat12 *fat12 = create_fat_struct(disk_filename);
  read_boot_sector(fat12);

  // TODO: Uppercase search filename

  int file_found = FALSE;
  int index = 0;
  int status;

  DirEntry *found_entry;

  // Loop through all files in the root directory
  while (1) {
    DirEntry *direntry;
    status = get_root_directory_entry(&direntry, index, fat12);

    if (status == -1) {
      break;
    }

    if (direntry != NULL) {
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
    printf("File not found.\n");
  } else {
    FILE *fp = fopen(search_filename, "wb");
    if (fp == NULL) {
      printf("Error opening file %s for writing binary.\n", search_filename);
      exit(1);
    }

    copy_found_file(found_entry, fat12, fp, search_filename);

    printf("Successfully copied %s (%d bytes) to current directory.\n",
           search_filename, found_entry->file_size);
  }

  destroy_fat_struct(fat12);
}