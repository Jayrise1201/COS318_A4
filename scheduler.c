/* scheduler.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Process scheduler for the kernel
 */

#include "common.h"
#include "interrupt.h"
#include "queue.h"
#include "scheduler.h"
#include "util.h"
#include "printf.h"
#include "kernel.h"

volatile uint64_t num_ticks;
pcb_t *current_running;
node_t ready_queue;
// The sum of the priorities of all tasks in the ready queue
priority_t total_ready_priority;

// An ORDERED queue of processes that are sleeping
// They are ordered by pcb->sleep_until in ascending order
node_t sleep_queue;

// System idle "task"
// Not a task per se, but does something to kill time when everything else is
// sleeping
static void idle(void);

// Order sleeping tasks so that they are ordered by their wake-up times
// Tasks which are scheduled to wake up earlier will appear earlier in the list
static int order_by_wake_up(node_t *a, node_t *b) {
    pcb_t *pcb_a = (pcb_t*)a, *pcb_b = (pcb_t*)b;
    
    if( pcb_a->sleep_until <= pcb_b->sleep_until )
        return 1;
    else
        return 0;
}

// Try to wake up sleeping processes
static void try_wake() {
    ASSERT(disable_count);

    uint64_t now = do_gettimeofday();
    
    // Extract as many PCBs from the sleep queue as are ready to be awoken
    // Recall that the sleep queue is sorted by wake-up times in ascending order
    while (!queue_empty(&sleep_queue)) {
        // Inspect (but do not remove) the first process to be woken up
        pcb_t *sleeper = (pcb_t*) queue_first(&sleep_queue);

        /* printf(0, 20, "I will wake pid %u at %u <=> %d    ", sleeper->pid,
               (uint32_t)sleeper->sleep_until, (uint32_t)now); */
        
        if (now >= sleeper->sleep_until) {
            // Remove from sleep list
            ASSERT(sleeper == (pcb_t*) queue_get(&sleep_queue));
            
            // Mark as awake
            sleeper->status = READY;
            
            // Add to the ready queue
            if (ENABLE_PRIORITIES)
                total_ready_priority += sleeper->priority;
            
            queue_put( &ready_queue, (node_t*)sleeper );
            
            // Consider other PCBs in the sleep queue
            continue;
        } else {
            // If the first doesn't wake up yet, then none of the others could
            // either (because they're sorted)
            return;
        }
    }
}

void do_sleep(int milliseconds) {
    enter_critical();
    
    current_running->sleep_until = do_gettimeofday() + milliseconds;
    current_running->status = BLOCKED;
    queue_put_sort(&sleep_queue, (node_t*) current_running, &order_by_wake_up);
    
    scheduler_entry();
    leave_critical();
}

void put_current_running() {
    ASSERT(disable_count);
    current_running->status = READY;

    if (ENABLE_PRIORITIES)
        total_ready_priority += current_running->priority;
    
    queue_put(&ready_queue, (node_t*) current_running);
}

// State for a random number generator used by lottery scheduling
static int32_t my_rand = 0xdeadbeef;

// Change current_running to the next task */
void scheduler() {
    ASSERT(disable_count);

    if (ENABLE_PRIORITIES)
        current_running->total_process_time += 
        get_timer() - current_running->last_entry_time;

    // Check if any sleeping threads are scheduled to wake up now
    try_wake();

    // Since all threads may be sleeping (or blocked), it is possible that the
    // ready_queue is empty
    while(queue_empty(&ready_queue)) {
      leave_critical();

      // If interrupts are disabled, then the timeofday will not increment, and
      // sleeping processes will never awaken
      // Here, we spend some time so that timer interrupts might occur
      idle();

      enter_critical();
      try_wake();
    }

    my_rand = rand_step(my_rand);
    priority_t choice = my_rand % total_ready_priority;
    pcb_t *chosen_process = NULL;
    
    for (;;) {
        // Select the front of the ready queue
        chosen_process = (pcb_t *)queue_get(&ready_queue);

        if (choice >= chosen_process->priority) {
            choice -= chosen_process->priority;
            queue_put(&ready_queue, (node_t*) chosen_process );
        } else {
            // Choose this one
            break;
        }
    }
    current_running = chosen_process;

    current_running->entry_count++;
    if (ENABLE_PRIORITIES) {
        total_ready_priority -= current_running->priority;
        current_running->last_entry_time = get_timer();
    }
    // Returning from this function will cause a context switch
}

// This is just like do_yield() but does not call enter/leave_critical first
// It is called by the timer interrupt in entry.S
void do_yield_naked() {
    ASSERT(disable_count);
    put_current_running();
    scheduler_entry();
}

void do_yield() {
    enter_critical();
    do_yield_naked();
    leave_critical();
}

void do_exit() {
    enter_critical();
    current_running->status = EXITED;
    scheduler_entry();
    // No need for leave_critical() since scheduler_entry() never returns
}

pid_t do_getpid() {
    pid_t pid;

    enter_critical();
    pid = current_running->pid;
    leave_critical();

    return pid;
}

priority_t do_getpriority() {
    priority_t priority;

    enter_critical();
    priority = current_running->priority;
    leave_critical();

    return priority;
}

void do_setpriority(priority_t priority) {
    if (priority >= 1) {
        enter_critical();
        current_running->priority = priority;
        leave_critical();
    }
}

uint64_t do_gettimeofday(void) {
    return num_ticks;
}

void block(node_t * wait_queue) {
    ASSERT(disable_count);
    current_running->status = BLOCKED;
    queue_put(wait_queue, (node_t *) current_running);
    scheduler_entry();
}

void unblock(pcb_t * task) {
    ASSERT(disable_count);
    task->status = READY;
    if (ENABLE_PRIORITIES)
        total_ready_priority += task->priority;
    queue_put(&ready_queue, (node_t*) task);
}

// The system 'idle' task
// If all processes are sleeping, this kills time...
static void idle(void) {
  // Interrupts must be ENABLED
  ASSERT(!disable_count);
}
