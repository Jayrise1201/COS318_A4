/* processes.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for programs included in the ramdisk filesystem
 */

#ifndef PROCESSES_H
#define PROCESSES_H

// It is VERY IMPORTANT that these functions do not use any global variables
// This is because each one might be running many times, and we do not have any
// facility to duplicate their data segments

void init_process(void);
void help_process(void);
void count_process(void);
void producer_process(void);
void consumer_process(void);
void shutdown_process(void);
void airplane_process(void);

#endif

