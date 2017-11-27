#include "fat12.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void copy_local_file(Fat12 *fat12, FILE *fp, char *name, char *ext,
                     uint32_t filesize) {
  int count = 0;

  uint16_t current_fat_index = next_free_cluster(fat12, -1);
  uint16_t next_free;
  uint16_t first_logical = current_fat_index;
  uint16_t fat_value;

  int bytes_left = filesize;

  DosTime *time;
  DosDate *date;
  create_time_date_structs(&time, &date);

  printf("first logical %d\n", first_logical);
  char *root_entry =
      create_root_entry(name, ext, 0x00, time, date, first_logical, filesize);

  add_root_entry(fat12, root_entry);

  while (bytes_left > 0) {
    printf("\n---\n");

    int bytes_to_copy = bytes_left;
    if (bytes_to_copy > SECTOR_SIZE) {
      bytes_to_copy = SECTOR_SIZE;
    }

    int from_location = ftell(fp);
    int to_location = get_physical_sector_number(current_fat_index);

    copy_bytes(bytes_to_copy, from_location, to_location, fp, fat12->fp);

    // printf("Copying %d bytes to physical sector %d\n", bytes_to_copy,
    //        to_location);

    next_free = next_free_cluster(fat12, current_fat_index);
    // printf("next free 0x%x (%d)\n", next_free, next_free);

    bytes_left -= bytes_to_copy;
    if (bytes_left <= 0) {
      fat_value = 0xFFF;
    } else {
      fat_value = next_free;
    }

    printf("%d points to %d\n", current_fat_index, next_free);
    write_fat_entry(fat12, current_fat_index, fat_value);

    printf("\n");

    // int i;
    // for (i = 0; i < 5; i += 1) {
    //   uint16_t curr_value = get_fat_value(current_fat_index + i, fat12);
    //   printf("%d: 0x%03x\n", current_fat_index + i, curr_value);
    // }

    current_fat_index = next_free;

    count += 1;
    // if (count == 2) {
    //   break;
    // }
  }

  printf("first logical %d 0x%03x\n", first_logical, first_logical);
}

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 3) {
    printf("Disk image filename and root directory filename required.\n");
    exit(1);
  }

  char *disk_filename = argv[1]; // The FAT12 image file
  char *copy_filename = argv[2]; // The file to copy

  // Make a copy of the local filename
  // This copy will not be uppercased
  char local_filename[strlen(copy_filename) + 1];
  strcpy(local_filename, copy_filename);

  char name[8];
  char ext[3];
  uppercase_string(copy_filename);
  if (!verify_filename(copy_filename, name, ext)) {
    printf("Filename is not valid\n");
    exit(1);
  }

  printf("%s.%s\n", name, ext);

  Fat12 *fat12 = create_fat_struct(disk_filename);

  read_disk_info(fat12);
  free_space(fat12);

  FILE *copy_fp = fopen(local_filename, "r");

  if (copy_fp == NULL) {
    printf("File not found.\n");
    exit(1);
  }

  int filesize = get_filesize(copy_fp);
  printf("filesize: %d\n", filesize);

  if (fat12->free_size < filesize) {
    printf("Not enough free space in the disk image.\n");
    exit(1);
  }

  printf("%d, 0x%03x\n", 139, get_fat_value(139, fat12));

  copy_local_file(fat12, copy_fp, name, ext, filesize);
  printf("Successfully copied %s (%d bytes) to %s.\n", local_filename, filesize,
         disk_filename);

  destroy_fat_struct(fat12);
}