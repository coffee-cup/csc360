#include "fat12.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Update last accessed date

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

    // copy_bytes(bytes_to_copy, physical_sector, fat12->fp, fp);

    if (!next_cluster(&next, next, fat12)) {
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

  char *disk_filename = argv[1]; // The FAT12 image file
  char *copy_filename = argv[2]; // The file to copy
  Fat12 *fat12 = create_fat_struct(disk_filename);

  read_disk_info(fat12);

  FILE *copy_fp = fopen(copy_filename, "r");

  if (copy_fp == NULL) {
    printf("File not found.");
    exit(1);
  }

  int filesize = get_filesize(copy_fp);
  printf("filesize: %d\n", filesize);

  if (fat12->free_size < filesize) {
    printf("Not enough free space in the disk image.\n");
    exit(1);
  }

  int next_free = next_free_cluster(fat12);
  printf("Next free %d\n", next_free);

  destroy_fat_struct(fat12);
}