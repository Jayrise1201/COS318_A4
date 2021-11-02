/* syslib.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Implementation of the system call libary for user processes
 * Each function implements a trap to the kernel
 */

#include "common.h"
#include "syslib.h"
#include "util.h"

// 1. Place system call number (i) in eax and arg1 in ebx
// 2. Trigger interrupt 48 (system call)
// 3. Return value is in eax after returning from interrupt
static int invoke_syscall(int i, int arg1, int arg2, int arg3) {
    int ret;
    asm volatile ("int $48"  // 48 = 0x30
                  :"=a" (ret)
                  :"a"(i), "b"(arg1), "c"(arg2), "d"(arg3));
    return ret;
}

void yield(void) {
    invoke_syscall(SYSCALL_YIELD, IGNORE, IGNORE, IGNORE);
}

void exit(void) {
    invoke_syscall(SYSCALL_EXIT, IGNORE, IGNORE, IGNORE);
}

int getpid(void) {
    return invoke_syscall(SYSCALL_GETPID, IGNORE, IGNORE, IGNORE);
}

int getpriority(void) {
    return invoke_syscall(SYSCALL_GETPRIORITY, IGNORE, IGNORE, IGNORE);
}

void setpriority(int p) {
    invoke_syscall(SYSCALL_SETPRIORITY, p, IGNORE, IGNORE);
}

void sleep(int milliseconds) {
    invoke_syscall(SYSCALL_SLEEP, milliseconds, IGNORE, IGNORE);
}

void shutdown(void) {
    invoke_syscall(SYSCALL_SHUTDOWN, IGNORE, IGNORE, IGNORE);
}

void write_serial(char c) {
    invoke_syscall(SYSCALL_WRITE_SERIAL, (int)c, IGNORE, IGNORE);
}

char get_char(void) {
    return (char) invoke_syscall(SYSCALL_GET_CHAR, IGNORE, IGNORE, IGNORE);
}

pid_t spawn(const char *filename) {
    return (pid_t) invoke_syscall(SYSCALL_SPAWN, (int)filename, IGNORE, IGNORE);
}

int kill(pid_t pid) {
    return invoke_syscall(SYSCALL_KILL, (int)pid, IGNORE, IGNORE);
}

int wait(pid_t pid) {
    return invoke_syscall(SYSCALL_WAIT, (int)pid, IGNORE, IGNORE);
}

mbox_t mbox_open(const char *name) {
    return (mbox_t) invoke_syscall(SYSCALL_MBOX_OPEN, (int)name, IGNORE, IGNORE);
}

void mbox_close(mbox_t mbox) {
  invoke_syscall(SYSCALL_MBOX_CLOSE, (int)mbox, IGNORE, IGNORE);
}

void mbox_send(mbox_t mbox, void *msg, int nbytes) {
    invoke_syscall(SYSCALL_MBOX_SEND, (int)mbox, (int)msg, nbytes);
}

void mbox_recv(mbox_t mbox, void *msg, int nbytes) {
    invoke_syscall(SYSCALL_MBOX_RECV, (int) mbox, (int)msg, nbytes);
}
