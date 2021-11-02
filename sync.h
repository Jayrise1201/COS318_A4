/* sync.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for synchronization primitivies
 */

#ifndef THREAD_H
#define THREAD_H

#include "queue.h"

typedef struct lock {
    enum {
        UNLOCKED,
        LOCKED
    } status;
    node_t wait_queue;
    // Who currently holds this lock?
    struct pcb *owner;
} lock_t;

typedef struct {
  // A list of threads blocking on this condition variable
  node_t wait_queue;
} condition_t;

typedef struct {
  // The value of this semaphore
  unsigned value;
  // An ordered list of processes waiting for this semaphore
  node_t wait_queue;
} semaphore_t;

typedef struct {
  // The quorum size
  unsigned quorum;
  // The number of processes who are already waiting
  unsigned size;
  // The list of processes who are already waiting
  node_t wait_queue;
} barrier_t;

// Lock functions
void lock_init(lock_t *);
int lock_acquire(lock_t *);
void lock_release(lock_t *);

// Condition variable functions
void condition_init(condition_t * c);
void condition_wait(lock_t * m, condition_t * c);
void condition_signal(condition_t * c);
void condition_broadcast(condition_t * c);

// Semaphore functions
void semaphore_init(semaphore_t * s, int value);
void semaphore_up(semaphore_t * s);
void semaphore_down(semaphore_t * s);

// Barrier functions
void barrier_init(barrier_t * b, int n);
void barrier_wait(barrier_t * b);

#endif
