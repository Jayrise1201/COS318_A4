/* ramdisk.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for ramdisk filesystem
 */

#ifndef RAMDISK_H
#define RAMDISK_H

// Ramdisk provides a fake filesystem
// It is read-only and its contents are assembled at compile time
typedef void (*Process)(void);

typedef struct {
    const char *filename;
    Process process;
} File;

Process ramdisk_find(const char* filename);

#endif
