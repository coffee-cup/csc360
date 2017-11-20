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
  if (boot->volume_label == "") {
    strcpy(boot->volume_label, "NO NAME ");
  }

  verify_disk(fat12);
}

int num_root_files(Fat12 *fat12) {
  int count = 0;
  int index = 0;
  int status;

  while (1) {
    DirEntry *direntry;
    status = get_root_directory_entry(&direntry, index, fat12);

    if (status == -1) {
      break;
    }

    if (direntry != NULL && (direntry->attributes & SUBDIR) == 0) {
      count += 1;
    }
    index += 1;
  }

  return count;
}

int get_root_directory_entry(DirEntry **direntry_ptr, int entry_num,
                             Fat12 *fat12) {
  DirEntry *direntry = (DirEntry *)malloc(sizeof(DirEntry));

  int entry_offset = (19 * SECTOR_SIZE) + (entry_num * 32);

  /*
   * Start Reading Directory Entry Info
   */

  // filename
  fseek(fat12->fp, entry_offset + 0, SEEK_SET);
  fread(&direntry->name, 8, 1, fat12->fp);
  int i = 0;
  while (i < 8) {
    if (direntry->name[i] == ' ')
      break;
    i += 1;
  }
  direntry->name[i] = '\0'; // terminating char

  // extension
  fseek(fat12->fp, entry_offset + 8, SEEK_SET);
  fread(&direntry->ext, 3, 1, fat12->fp);
  direntry->ext[3] = '\0'; // terminating char

  // attributes
  fseek(fat12->fp, entry_offset + 11, SEEK_SET);
  fread(&direntry->attributes, 1, 1, fat12->fp);

  // creation time
  fseek(fat12->fp, entry_offset + 14, SEEK_SET);
  fread(&direntry->creation_time, 2, 1, fat12->fp);

  // creating date
  fseek(fat12->fp, entry_offset + 16, SEEK_SET);
  fread(&direntry->creation_date, 2, 1, fat12->fp);

  // last write time
  fseek(fat12->fp, entry_offset + 22, SEEK_SET);
  fread(&direntry->last_write_time, 2, 1, fat12->fp);

  // last write date
  fseek(fat12->fp, entry_offset + 24, SEEK_SET);
  fread(&direntry->last_write_date, 2, 1, fat12->fp);

  // first logical cluster
  fseek(fat12->fp, entry_offset + 26, SEEK_SET);
  fread(&direntry->first_logical_cluster, 2, 1, fat12->fp);

  // file size (in bytes)
  fseek(fat12->fp, entry_offset + 28, SEEK_SET);
  fread(&direntry->file_size, 4, 1, fat12->fp);

  /*
   * End Reading Info
   */

  // Ignore this directory entry if
  //  first byte of filename is 0xE5
  //  or attributes byte is 0x0F (long filename)
  if ((uint8_t)direntry->name[0] == 0xE5 || direntry->attributes == 0x0F) {
    *direntry_ptr = NULL;
    return 0;
  }

  // If this entry is the volume label
  if (direntry->attributes & VOLUME_LABEL) {
    strcpy(fat12->boot->volume_label, direntry->name);
    strcat(fat12->boot->volume_label, direntry->ext);
    *direntry_ptr = NULL;
    return 0;
  }

  if ((uint8_t)direntry->name[0] == 0x00) {
    *direntry_ptr = NULL;
    return -1;
  }

  *direntry_ptr = direntry;

  return 0;
}

void verify_disk(Fat12 *fat12) {
  if (fat12->boot->bytes_per_sector != SECTOR_SIZE) {
    printf("Disk it not FAT12");
    exit(1);
  }
}