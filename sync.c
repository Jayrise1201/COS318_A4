/* sync.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Implementation of synchronization primitives
 */

#include "common.h"
#include "interrupt.h"
#include "queue.h"
#include "scheduler.h"
#include "sync.h"
#include "util.h"
#include "kernel.h"

static int check_for_deadlock(pcb_t *owner, pcb_t *src) {
    int timeout;
    pcb_t *i;
    
    for (i=owner, timeout=get_max_pcbs(); i && i->waiting_for_lock && 
         timeout >= 0; i=i->waiting_for_lock->owner, timeout--) {
        if( i == src )
            return 1;
    }
    
    return 0;
}

static pcb_t * unblock_one(node_t * wait_queue) {
    pcb_t *p;

    ASSERT(disable_count);
    p = (pcb_t *) queue_get(wait_queue);
    if (NULL != p) {
        unblock(p);
        return p;
    } else {
        return NULL;
    }
}

static void unblock_all(node_t * wait_queue) {
    while(unblock_one(wait_queue)) {}
}

// Initialize a lock
void lock_init(lock_t * l) {
    // No critical section as it is the caller's responsibility to make sure
    // that locks are initialized only once
    l->status = UNLOCKED;
    l->owner = NULL;
    queue_init(&l->wait_queue);
}

// Return 0 on success, 1 on failure (Extra Credit)
static int lock_acquire_helper(lock_t * l) {
    ASSERT(disable_count);
    if (LOCKED == l->status) {
        current_running->waiting_for_lock = l;
        if (check_for_deadlock(l->owner, current_running)) {
            current_running->waiting_for_lock = NULL;
            return 1;
        }
        block(&l->wait_queue);
    } else {
        current_running->waiting_for_lock = NULL;
        l->owner = current_running;
    }

    l->status = LOCKED;
    return 0;
}

// Return 0 on success, 1 on failure (Extra Credit)
int lock_acquire(lock_t * l) {
    enter_critical();
    int result = lock_acquire_helper(l);
    leave_critical();

    return result;
}

static void lock_release_helper(lock_t * l) {
    ASSERT(disable_count);
    pcb_t *p = unblock_one(&l->wait_queue);
    
    if (p) {
        p->waiting_for_lock = NULL;
        l->owner = p;
    } else {
        l->owner = NULL;
    }

    l->status = UNLOCKED;
}

void lock_release(lock_t * l) {
    enter_critical();
    lock_release_helper(l);
    leave_critical();
}

// Initialize a condition variable
void condition_init(condition_t * c) {
    queue_init( &c->wait_queue );
}

// Release m and block the thread (enqueued on c); when unblocked,
// reacquire m
void condition_wait(lock_t * m, condition_t * c) {
  enter_critical();
  lock_release_helper(m);
  ASSERT(disable_count);
  block( &c->wait_queue );
  lock_acquire_helper(m);
  leave_critical();
}

// Unblock the first thread waiting on c, if it exists
void condition_signal(condition_t * c) {
  enter_critical();
  ASSERT(disable_count);
  unblock_one( &c->wait_queue );
  leave_critical();
}

// Unblock all threads waiting on c
void condition_broadcast(condition_t * c) {
  enter_critical();
  ASSERT(disable_count);
  unblock_all( &c->wait_queue );
  leave_critical();
}

// Initialize a semaphore with the specified value
void semaphore_init(semaphore_t * s, int value) {
  s->value = value;
  queue_init( &s->wait_queue );
}

// Increment the semaphore value atomically
void semaphore_up(semaphore_t * s) {
  enter_critical();

  if (s->value < 1 && !queue_empty(&s->wait_queue))
      unblock_one(&s->wait_queue);
  else
      s->value++;

  leave_critical();
}

// Block until the semaphore value is greater than zero and decrement it
void semaphore_down(semaphore_t * s) {
  enter_critical();

  if (s->value < 1)
      block(&s->wait_queue);
  else
      s->value--;

  leave_critical();
}

// Initialize a barrier where n is number of threads that rendezvous at the
// barrier
void barrier_init(barrier_t * b, int n) {
  b->quorum = n;
  b->size = 0;
  queue_init( &b->wait_queue );
}

// Block until all n threads have called barrier_wait
void barrier_wait(barrier_t * b) {
  enter_critical();

  if (b->size + 1 >= b->quorum) {
      b->size = 0;
      unblock_all( &b->wait_queue );
  } else {
      b->size ++;
      block( &b->wait_queue );
  }

  leave_critical();
}
