#include "fat12.h"

Fat12 *create_fat_struct(char *filename) {
  Fat12 *fat12 = (Fat12 *)malloc(sizeof(Fat12));
  fat12->filename = filename;

  FILE *fp = fopen(fat12->filename, "r+");

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

void read_disk_info(Fat12 *fat12) {
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
  fat12->total_size = boot->total_sectors * SECTOR_SIZE;

  fseek(fat12->fp, 22, SEEK_SET);
  fread(&boot->sectors_per_fat, 2, 1, fat12->fp);

  fseek(fat12->fp, 43, SEEK_SET);
  fread(&boot->volume_label, 11, 1, fat12->fp);
  if (boot->volume_label == "") {
    strcpy(boot->volume_label, "NO NAME ");
  }

  free_space(fat12);
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

char *create_root_entry(char *name, char *ext, char attributes,
                        DosTime *write_time, DosDate *write_date,
                        uint16_t first_logical_cluster, uint32_t filesize) {

  char *entry = (char *)malloc(32 * sizeof(char));
  char *p = entry;

  // Init to all 0x00
  int i;
  for (i = 0; i < strlen(entry); i += 1) {
    entry[i] = 0x00;
  }

  // Name
  memcpy(p, name, 8);

  // Extension
  p = entry + 8;
  memcpy(p, ext, 3);

  // Attributes
  p = entry + 11;
  memcpy(p, &attributes, 1);

  // Creation time
  p = entry + 14;
  memcpy(p, write_time, 2);

  // Creation date
  p = entry + 16;
  memcpy(p, write_date, 2);

  // Last access date
  p = entry + 18;
  memcpy(p, write_date, 2);

  // Last write time
  p = entry + 22;
  memcpy(p, write_time, 2);

  // Last write date
  p = entry + 24;
  memcpy(p, write_date, 2);

  // First logical cluster
  p = entry + 26;
  memcpy(p, &first_logical_cluster, 2);

  // Filesize
  p = entry + 28;
  memcpy(p, &filesize, 4);

  return entry;
}

void add_root_entry(Fat12 *fat12, char *root_entry) {
  int index = 0;
  int status;

  while (1) {
    DirEntry *direntry;
    status = get_root_directory_entry(&direntry, index, fat12);

    if (status == -1) {
      break;
    }

    index += 1;
  }

  int entry_offset = (19 * SECTOR_SIZE) + (index * 32);
  fseek(fat12->fp, entry_offset, SEEK_SET);
  fwrite(root_entry, 32, 1, fat12->fp);
}

uint16_t get_fat_value(int entry_num, Fat12 *fat12) {
  unsigned int fat_offset = SECTOR_SIZE;
  unsigned int entry_offset = (3 * entry_num) / 2;
  unsigned int offset = fat_offset + entry_offset;

  uint8_t fullbits;
  uint8_t halfbits;
  unsigned int fat_entry = 0;

  // printf("\n");
  // printf("sectors per fat: %d\n", fat12->boot->sectors_per_fat);
  // printf("entry_num: %d - 0x%04x\n", entry_num, entry_num);
  // printf("fat_offset: %d - 0x%04x\n", fat_offset, fat_offset);
  // printf("entry_offset: %d - 0x%04x\n", entry_offset, entry_offset);
  // printf("offset: %d - 0x%04x\n", offset, offset);
  // printf("\n");

  // If n is even, then physical location of the entry
  //    is the low four bits in location 1+(3*n)/2
  //    and the 8 bits in location (3*n)/2
  // If n is odd, then physical location of the entry
  //    is the high four bits in location (3*n)/2
  //    and the 8 bits in location 1+(3*n)/2
  if (entry_num % 2 == 0) {
    // printf("eight bits at 0x%04x\n", offset);
    // printf("low four bits at 0x%x\n", offset + 1);

    fseek(fat12->fp, offset, SEEK_SET);
    fread(&fullbits, 8, 1, fat12->fp);

    fseek(fat12->fp, offset + 1, SEEK_SET);
    fread(&halfbits, 8, 1, fat12->fp);

    fat_entry = (halfbits << 12) | fullbits;
  } else {
    // printf("eight bits at 0x%04x\n", offset + 1);
    // printf("high four bits at 0x%x\n", offset);

    fseek(fat12->fp, offset + 1, SEEK_SET);
    fread(&fullbits, 8, 1, fat12->fp);

    fseek(fat12->fp, offset, SEEK_SET);
    fread(&halfbits, 8, 1, fat12->fp);

    halfbits = halfbits >> 4;

    fat_entry = (fullbits << 4) | halfbits;
  }

  // printf("\n");
  // printf("halfbits: 0x%02x\n", halfbits);
  // printf("fullbits: 0x%02x\n", fullbits);

  // printf("\n");
  // printf("entry_num: %d\n", entry_num);
  // printf("fat_entry: %d\n", fat_entry);

  // printf("FAT VALUE %d - 0x%03x\n", fat_entry, fat_entry);
  return fat_entry;
}

void write_fat_entry(Fat12 *fat12, int entry_num, uint16_t value) {
  unsigned int fat_offset = SECTOR_SIZE;
  unsigned int entry_offset = (3 * entry_num) / 2;
  unsigned int offset = fat_offset + entry_offset;

  uint8_t fullbits, halfbits, halfbits_curr;

  printf("\n");
  printf("entry num: %d - 0x%03x\n", entry_num, entry_num);
  printf("fat value %d - 0x%03x\n", value, value);

  if (entry_num % 2 == 0) {
    printf("fullbits: 0x%02x\n", value & 0x0FF);
    printf("halfbits: 0x%02x\n", value & 0xF00);

    fullbits = value & 0x0FF;
    halfbits = value >> 8;

    // Write full bits
    fseek(fat12->fp, offset, SEEK_SET);
    fwrite(&fullbits, 8, 1, fat12->fp);

    // Write half bits
    fseek(fat12->fp, offset + 1, SEEK_SET);
    fread(&halfbits_curr, 8, 1, fat12->fp);

    halfbits |= halfbits_curr;
    fwrite(&halfbits, 8, 1, fat12->fp);
  } else {
    printf("fullbits: 0x%02x\n", value >> 4);
    printf("halfbits: 0x%02x\n", (value & 0x00F) << 4);

    fullbits = value >> 4;
    halfbits = (value & 0x00F) << 4;

    // printf("full: 0x%02x\n", fullbits);
    // printf("half: 0x%02x\n", halfbits);

    // Write full bits
    fseek(fat12->fp, offset + 1, SEEK_SET);
    fwrite(&fullbits, 8, 1, fat12->fp);

    // Write half bits
    fseek(fat12->fp, offset, SEEK_SET);
    fread(&halfbits_curr, 8, 1, fat12->fp);

    halfbits |= halfbits_curr;
    fwrite(&halfbits, 8, 1, fat12->fp);
  }

  printf("full: 0x%02x\n", fullbits);
  printf("half: 0x%02x\n", halfbits);
}

int next_cluster(uint16_t *next, int entry_num, Fat12 *fat12) {
  uint16_t fat_value = get_fat_value(entry_num, fat12);
  // printf("next: 0x%x\n", fat_value);
  if (fat_value >= 0xFF0) {
    return FALSE;
  }

  *next = fat_value;
  return TRUE;
}

int next_free_cluster(Fat12 *fat12, int not_index) {
  int i;
  printf("NOT index: %d\n", not_index);
  for (i = 2; i <= 2846; i += 1) {
    uint16_t fat_value = get_fat_value(i, fat12);
    if (fat_value == 0x0000 && i != not_index) {
      return i;
    }
  }
  return -1;
}

// correct first
// 1389568 bytes = 2714 sectors
//
void free_space(Fat12 *fat12) {
  int free_sectors = 0;

  int i;
  for (i = 2; i <= 2842; i += 1) {
    int fat_value = get_fat_value(i, fat12);
    if (fat_value == 0x00) {
      free_sectors += 1;
    }
  }

  // fat12->free_size = free_sectors;
  fat12->free_size = free_sectors * SECTOR_SIZE;
}

void verify_disk(Fat12 *fat12) {
  if (fat12->boot->bytes_per_sector != SECTOR_SIZE) {
    printf("Disk it not FAT12");
    exit(1);
  }
}

int get_physical_sector_number(uint16_t logical_sector_number) {
  return (33 + logical_sector_number - 2) * SECTOR_SIZE;
}