#include "fat12.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 2) {
    printf("Filename of disk image is required.\n");
    exit(1);
  }

  char *filename = argv[1];
  Fat12 *fat12 = create_fat_struct(filename);

  read_disk_info(fat12);

  int root_file_count = num_root_files(fat12);

  printf("\n");
  printf("OS Name: %s\n", fat12->boot->osname);
  printf("Label of the disk: %s\n", fat12->boot->volume_label);
  printf("Total size of the disk: %d bytes\n", fat12->total_size);
  printf("Free size of the disk: %d bytes\n", fat12->free_size);

  printf("\n==============\n");
  printf("The number of files in the root directory (not including "
         "subdirectories): %d\n",
         root_file_count);

  printf("\n==============\n");
  printf("Number of FAT copies: %d\n", fat12->boot->num_fats);
  printf("Sectors per FAT: %d\n", fat12->boot->sectors_per_fat);

  destroy_fat_struct(fat12);
}