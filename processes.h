/* processes.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for processes included in the ramdisk filesystem
 */

#ifndef PROCESSES_H
#define PROCESSES_H

// It is VERY IMPORTANT that these functions do not use any global variables
// This is because each one might be running many times, and we do not have any
// facility to duplicate their data segments

// The 'init' process is the first process started by the kernel
void init(void);

// Robin hood and little John are allies
void RobinHood(void);
void LittleJohn(void);

// And there is a villian (hiss) - Sheriff of KnottingHam
void Sheriff(void);

#endif
