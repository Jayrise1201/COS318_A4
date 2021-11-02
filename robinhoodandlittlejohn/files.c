/* files.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Table mapping filenames to entry points
 */

#include "files.h"
#include "processes.h"

// This file is analogous to tasks.c in the previous project
static File files[] = {
    {.filename = "init", .process = &init},
    {.filename = "RobinHood", .process = &RobinHood },
    {.filename = "LittleJohn", .process = &LittleJohn },
    {.filename = "Sheriff", .process = &Sheriff },
};

#define NUM_FILES (sizeof(files) / sizeof(File))

int get_num_files() {
    return NUM_FILES;
}

File *get_nth_file(int n) {
    return &files[n];
}
