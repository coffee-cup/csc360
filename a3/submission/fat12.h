#ifndef FAT
#define FAT

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SECTOR_SIZE 512 // bytes

typedef struct _Fat12 Fat12;
typedef struct _Boot Boot;
typedef struct _DirEntry DirEntry;
typedef struct _DosTime DosTime;
typedef struct _DosDate DosDate;

// Struct definitions
struct _Fat12 {
  char *filename;                    /* The filename of the disk image */
  FILE *fp;                          /* The file pointer */
  int total_size;                    /* Total size of the disk */
  int free_size;                     /* Free size of the disk */
  unsigned short int num_files_root; /* Number of files in the root directory */

  struct _Boot *boot;
};

struct _Boot {
  char osname[8];            /* The name of the operating system */
  uint16_t bytes_per_sector; /* The number of bytes per sector */
  uint8_t num_fats;          /* The number of FATs */
  uint16_t max_root_entries; /* The maximum number of root directory entries */
  uint16_t total_sectors;    /* The total number of sectors */
  uint16_t sectors_per_fat;  /* The number of sectors per FAT */
  char volume_label[11];     /* The label of the volume */
};

struct _DosTime {
  unsigned int two_secs : 5;
  unsigned int minutes : 6;
  unsigned int hours : 5;
} __attribute__((packed));

struct _DosDate {
  unsigned int date : 5;
  unsigned int month : 4;
  unsigned int year : 7;
} __attribute__((packed));

struct _DirEntry {
  unsigned char name[9]; // extra char for \0
  unsigned char ext[4];  // extra char for \0
  uint8_t attributes;
  struct _DosTime creation_time;
  struct _DosDate creation_date;
  struct _DosTime last_write_time;
  struct _DosDate last_write_date;
  uint16_t first_logical_cluster;
  uint32_t file_size;
} __attribute__((packed));

// Create an empty Fat struct
Fat12 *create_fat_struct(char *filename);

// Close the file pointer and free memory
void destroy_fat_struct(Fat12 *fat12);

void read_disk_image(Fat12 *fat12);

// Read data from the boot sector
void read_boot_sector(Fat12 *fat12);

// Read file data from the root directory sector
void read_root_directory(Fat12 *fat12);

// Verifies the disk is FAT12
// Exits with error of not FAT12
void verify_disk(Fat12 *fat12);

#endif