/* scheduler.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for process scheduler for the kernel
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common.h"
#include "queue.h"

typedef enum {
    KERNEL_THREAD,
    PROCESS
} task_type_t;

typedef int priority_t;

typedef enum {
    EXITED = 0,
    FIRST_TIME,
    READY,
    BLOCKED
} status_t;

// Simplified structure, used in processes.c to declare all processes to start
struct task_info {
    uint32_t entry_point;
    task_type_t task_type;
};

typedef struct pcb {
    // DO NOT CHANGE THE ORDER
    node_t node;
    uint32_t kflags;
    uint32_t kregs[8];
    uint32_t *ksp;
    uint32_t uflags;
    uint32_t uregs[8];
    uint32_t *usp;
    int nested_count;
    // entry.S depends on the precise ordering of fields preceding this comment
    uint32_t entry_point;
    pid_t pid;
    task_type_t task_type;
    priority_t priority;
    status_t status;
    uint32_t entry_count;
    // If this process is in the sleep queue, this tells us the time after which
    // we should let it out
    uint64_t sleep_until;
    // For priorities, keep track of how much time we have spent in this process
    uint64_t last_entry_time;
    uint64_t total_process_time;
    // For deadlock detection
    struct lock *waiting_for_lock;
} pcb_t;

extern priority_t total_ready_priority;
extern node_t ready_queue;
extern pcb_t *current_running;
extern node_t sleep_queue;

// Save the context and the kernel stack before calling scheduler
// This function is implemented in entry.S
extern void scheduler_entry(void);

// Schedule another task, called from a kernel thread or kernel_entry_helper()
void do_yield(void);

// Schedule another task, do not reschedule the current one, called from a
// kernel thread or kernel_entry_helper()
void do_exit(void);

// Return the current task's pid
pid_t do_getpid(void);

// Return the current task's priority
priority_t do_getpriority(void);

// Set the current task's priority
void do_setpriority(priority_t priority);

// Milliseconds since boot
uint64_t do_gettimeofday(void);

// Sleep for specified number of mulliseconds
void do_sleep(int milliseconds);

// Enable blocking sleep (Extra Credit)
void do_enable_blocking();

// Schedule another task, putting the current one in the specified queue
// Do not reschedule the current task until it is unblocked
void block(node_t * wait_queue);

// Unblock the specified task
void unblock(pcb_t * task);

#endif
