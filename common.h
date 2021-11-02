/* common.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Common definitions and types
 */

#ifndef COMMON_H
#define COMMON_H

// Reserved for Extra Credit for prioritized processes using lottery scheduling
#define EC_PRIORITIES

// Reserved for Extra Credit for automatic deadlock
#define EC_DEADLOCK

// Equivalent to above, but defined to 0 or 1, so they can be included in a
// condition
#ifdef EC_PRIORITIES
  #define ENABLE_PRIORITIES 1
#else
  #define ENABLE_PRIORITIES 0
#endif

#define NULL ((void *) 0)

// System calls
typedef enum {
    SYSCALL_YIELD=0,
    SYSCALL_EXIT,
    SYSCALL_GETPID,
    SYSCALL_GETPRIORITY,
    SYSCALL_SETPRIORITY,
    SYSCALL_SLEEP,
    SYSCALL_SHUTDOWN,
    SYSCALL_WRITE_SERIAL,
    SYSCALL_GET_CHAR,
    SYSCALL_SPAWN,
    SYSCALL_KILL,
    SYSCALL_WAIT,
    SYSCALL_MBOX_OPEN,
    SYSCALL_MBOX_CLOSE,
    SYSCALL_MBOX_SEND,
    SYSCALL_MBOX_RECV,
    NUM_SYSCALLS
} syscall_t;

// Assertion macros
#define ASSERT2(p, s) \
do { \
    if (!(p)) { \
        print_str(0, 0, "Assertion failure: "); \
        print_str(0, 19, s); \
        print_str(1, 0, "file: "); \
        print_str(1, 6, __FILE__); \
        print_str(2, 0, "line: "); \
        print_int(2, 6, __LINE__); \
        asm volatile ("cli"); \
        while (1); \
    } \
} while (0)
#define ASSERT(p) ASSERT2(p, #p)
#define HALT(s) ASSERT2(FALSE, s)

// Commonly used types
typedef enum {
    FALSE, TRUE
} bool_t;

typedef signed char int8_t;
typedef short int int16_t;
typedef long int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef int pid_t;
typedef int mbox_t;

// We do not have any implementation of memory allocation yet, so it helps to
// impose static upper limits on various data structures
#define NUM_PCBS            (32)
#define MAX_MBOXEN          (32)
#define MBOX_NAME_LENGTH    (32)
#define MAX_MBOX_LENGTH     (32)
#define MAX_MESSAGE_LENGTH  (32)

#endif
