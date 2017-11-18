#ifndef FAT
#define FAT

#include <stdio.h>
#include <stdlib.h>

#define SECTOR_SIZE 512 // bytes

// Base FAT struct
typedef struct _Fat Fat;

// Sectors
typedef struct _Boot Boot;

// Struct definitions
struct _Fat {
  char *filename;                    /* The filename of the disk image */
  FILE *fp;                          /* The file pointer */
  int total_size;                    /* Total size of the disk */
  int free_size;                     /* Free size of the disk */
  unsigned short int num_files_root; /* Number of files in the root directory */

  struct _Boot *boot;
};

struct _Boot {
  char osname[8];                        /* The name of the operating system */
  unsigned short int bytes_per_sector;   /* The number of bytes per sector */
  unsigned short int num_fats;           /* The number of FATs */
  unsigned short int total_sector_count; /* The total number of sectors */
  unsigned short int sectors_per_fat;    /* The number of sectors per FAT */
  char volume_label[11];                 /* The label of the volume */
};

// Create an empty Fat struct
Fat *create_fat_struct(char *filename);

void destroy_fat_struct(Fat *fat);

void read_disk_image(Fat *fat);

void read_boot_sector(Fat *fat);

#endif