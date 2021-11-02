/* files.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Table mapping filenames to entry points
 */

#include "files.h"
#include "processes.h"

// This file is analogous to tasks.c in the previous project
static File files[] = {
    {.filename = "init", .process = &init_process },
    {.filename = "help", .process = &help_process },
    {.filename = "count", .process = &count_process },
    {.filename = "producer", .process = &producer_process },
    {.filename = "consumer", .process = &consumer_process },
    {.filename = "shutdown", .process = &shutdown_process },
    {.filename = "plane", .process = &airplane_process }
};

#define NUM_FILES (sizeof(files) / sizeof(File))

int get_num_files() {
  return NUM_FILES;
}

File *get_nth_file(int n) {
  return &files[n];
}
