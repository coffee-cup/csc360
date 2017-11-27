#ifndef FAT
#define FAT

#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECTOR_SIZE (512) // bytes

// Attribute Defines
#define READ_ONLY (0x01)
#define HIDDEN (0x02)
#define SYSTEM (0x04)
#define VOLUME_LABEL (0x08)
#define SUBDIR (0x10)
#define ARCHIVE (0x20)

typedef struct _Fat12 Fat12;
typedef struct _Boot Boot;
typedef struct _DirEntry DirEntry;

// Struct definitions
struct _Fat12 {
  char *filename; /* The filename of the disk image */
  FILE *fp;       /* The file pointer */
  int total_size; /* Total size of the disk */
  int free_size;  /* Free size of the disk */

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

// Read data from the boot sector
void read_disk_info(Fat12 *fat12);

// Read file data from the root directory sector
void read_root_directory(Fat12 *fat12);

// Count the number of non-subdirs entries in the root directory
int num_root_files(Fat12 *fat12);

// Get the entry_num'th directory entry from the root directory
// Returns
//  -1 if no more entries
//  0 if entries exist after
int get_root_directory_entry(DirEntry **direntry_ptr, int entry_num,
                             Fat12 *fat12);

// Creates a 32 byte root entry for a file
char *create_root_entry(char *name, char *ext, char attributes,
                        DosTime *write_time, DosDate *write_date,
                        uint16_t first_logical_cluster, uint32_t filesize);

// Adds a file to the directory entry
void add_root_entry(char *root_entry, Fat12 *fat12);

// Returns the entry_num'th value in the fat table table_num
uint16_t get_fat_value(int entry_num, Fat12 *fat12);

// Writes a new fat entry to a specific entry number
void write_fat_entry(int entry_num, uint16_t value, Fat12 *fat12);

// Get the next logical cluster that belongs to a file
// Returns FALSE if no next cluster
int next_cluster(uint16_t *next, int entry_num, Fat12 *fat12);

// Gets the entry number of next free cluster that is not index
// Returns -1 if no free cluster available
int next_free_cluster(int not_index, Fat12 *fat12);

// Calculate free size on fat12 struct
void free_space(Fat12 *fat12);

// Verifies the disk is FAT12
// Exits with error of not FAT12
void verify_disk(Fat12 *fat12);

// Returns physical sector number given the logical sector number
int get_physical_sector_number(uint16_t logical_sector_number);

// Returns a root directory entry for a file matching filename
DirEntry *find_root_entry(char *search_filename, Fat12 *fat12);

// Copies the contents of FAT1 to FAT2
void copy_fats(Fat12 *fat12);

#endif