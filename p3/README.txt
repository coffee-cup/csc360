  ______   _______ __ ___
 |  ____/\|__   __/_ |__ \
 | |__ /  \  | |   | |  ) |
 |  __/ /\ \ | |   | | / /
 | | / ____ \| |   | |/ /_
 |_|/_/    \_\_|   |_|____|

 ==========================

 Welcome to the code for CSC 360 Assignment 3.
 Operations involving the FAT12 filesystem.

Compiling the Code
------------------

$ make

This creates the following executables

- diskinfo
- disklist
- diskget
- diskput

Running the Code
----------------

$ ./diskinfo disk.IMA

Prints basic information about the disk image.

$ ./disklist disk.IMA

Lists all files in the root directory.

$ ./diskget disk.IMA file.txt

Copies file.txt from the root directory of disk.IMA to the current directory.
Note: A file in the current directory with the same name will be overwritten.

$ ./diskput disk.IMA file.txt

Copies file.txt from the current directory to the root directory of disk.IMA.
Note: The filename will be converted to all uppercase.

Notes
-----

In the usage examples examples, disk.IMA can be replaced with any FAT12 disk image file and
file.txt can be replaced with any file.

The filenames used in diskput must compily to https://en.wikipedia.org/wiki/8.3_filename.

I have provided a disk image, disk.IMA, which can be used.

Files
-----

- diskinfo.c/disklist.c/diskget.c/diskput.c: The main function for each program respectivley.
- fat12.h/.c : The majority of the logic for dealing with the FAT12 filesystem.
- utils.h/.c : Utility functions for dealing with date and time, copying data from one file to another, and verifying filenames.

Design
------

Most of the logic for this assignment is in fat12.c. See fat12.h for a description of the functions. Operations dealing with the filesystem are abstracted
into this file for easy reusability between the programs. I use the Fat12 struct to store common information
about the filesystem which I use in multiple functions. A pointer to the file is also stored in this struct.