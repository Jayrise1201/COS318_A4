/* files.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for table mapping files names to entry points
 */

#ifndef FILES_H
#define FILES_H

#include "ramdisk.h"

// Return the number of files in this filesystem
int get_num_files();

// Get a pointer to the nth file
File *get_nth_file(int n);

#endif
