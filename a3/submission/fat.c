#include "fat.h"

Fat *create_fat_struct(char *filename) {
  Fat *fat = (Fat *)malloc(sizeof(Fat));
  fat->filename = filename;

  FILE *fp = fopen(fat->filename, "r");

  if (fp == NULL) {
    printf("Error opening file %s\n", filename);
    exit(1);
  }
  fat->fp = fp;

  Boot *boot = (Boot *)malloc(sizeof(Boot));
  fat->boot = boot;

  return fat;
}

void destroy_fat_struct(Fat *fat) {
  fclose(fat->fp);
  free(fat->boot);
  free(fat);
}

void read_disk_image(Fat *fat) {}

void read_boot_sector(Fat *fat) {
  Boot *boot = fat->boot; // for convenience

  fseek(fat->fp, 3, SEEK_SET);
  fread(&boot->osname, 8, 1, fat->fp);

  fseek(fat->fp, 11, SEEK_SET);
  fread(&boot->bytes_per_sector, 2, 1, fat->fp);

  fseek(fat->fp, 16, SEEK_SET);
  fread(&boot->num_fats, 1, 1, fat->fp);

  fseek(fat->fp, 22, SEEK_SET);
  fread(&boot->sectors_per_fat, 2, 1, fat->fp);

  fseek(fat->fp, 43, SEEK_SET);
  fread(&boot->volume_label, 11, 1, fat->fp);
}