#include "fat12.h"

Fat12 *create_fat_struct(char *filename) {
  Fat12 *fat12 = (Fat12 *)malloc(sizeof(Fat12));
  fat12->filename = filename;

  FILE *fp = fopen(fat12->filename, "r");

  if (fp == NULL) {
    printf("Error opening file %s\n", filename);
    exit(1);
  }
  fat12->fp = fp;

  Boot *boot = (Boot *)malloc(sizeof(Boot));
  fat12->boot = boot;

  return fat12;
}

void destroy_fat_struct(Fat12 *fat12) {
  fclose(fat12->fp);
  free(fat12->boot);
  free(fat12);
}

void read_disk_image(Fat12 *fat12) {}

void read_boot_sector(Fat12 *fat12) {
  Boot *boot = fat12->boot; // for convenience

  fseek(fat12->fp, 3, SEEK_SET);
  fread(&boot->osname, 8, 1, fat12->fp);

  fseek(fat12->fp, 11, SEEK_SET);
  fread(&boot->bytes_per_sector, 2, 1, fat12->fp);

  fseek(fat12->fp, 16, SEEK_SET);
  fread(&boot->num_fats, 1, 1, fat12->fp);

  fseek(fat12->fp, 17, SEEK_SET);
  fread(&boot->max_root_entries, 2, 1, fat12->fp);

  fseek(fat12->fp, 19, SEEK_SET);
  fread(&boot->total_sectors, 2, 1, fat12->fp);

  fseek(fat12->fp, 22, SEEK_SET);
  fread(&boot->sectors_per_fat, 2, 1, fat12->fp);

  fseek(fat12->fp, 43, SEEK_SET);
  fread(&boot->volume_label, 11, 1, fat12->fp);

  verify_disk(fat12);
}

void read_root_directory(Fat12 *fat12) {
  int entry_count = 0;

  // Loop and read each directory entry
  while (1) {
    DirEntry *direntry = (DirEntry *)malloc(sizeof(DirEntry));

    int entry_offset = (19 * SECTOR_SIZE) + (entry_count * 32);

    // filename
    fseek(fat12->fp, entry_offset + 0, SEEK_SET);
    fread(&direntry->name, 8, 1, fat12->fp);
    direntry->name[8] = '\0';

    // TODO: fix this
    if ((uint8_t)direntry->name[0] == 0xE5) {
      continue;
    }

    // extension
    fseek(fat12->fp, entry_offset + 8, SEEK_SET);
    fread(&direntry->ext, 3, 1, fat12->fp);
    direntry->ext[3] = '\0';

    printf("\n");
    printf("name: %s\n", direntry->name);
    printf("ext: %s\n", direntry->ext);

    if (entry_count == 5) {
      break;
    }
    entry_count += 1;
  }
}

void verify_disk(Fat12 *fat12) {
  if (fat12->boot->bytes_per_sector != SECTOR_SIZE) {
    printf("Disk it not FAT12");
    exit(1);
  }
}