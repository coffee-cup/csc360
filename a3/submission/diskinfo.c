#include "fat.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 2) {
    printf("filename of disk image is required.\n");
    exit(1);
  }

  char *filename = argv[1];
  Fat *fat = create_fat_struct(filename);
  read_boot_sector(fat);

  printf("\n");
  printf("OS Name: %s\n", fat->boot->osname);
  printf("Label of the disk: %s\n", fat->boot->volume_label);
  printf("Total size of the disk: \n");
  printf("Free size of the disk: \n");

  printf("\n==============\n");
  printf("The number of files in the root directory (not including "
         "subdirectories): \n");

  printf("\n==============\n");
  printf("Number of FAT copies: %d\n", fat->boot->num_fats);
  printf("Sectors per FAT: %d\n", fat->boot->sectors_per_fat);

  destroy_fat_struct(fat);
}