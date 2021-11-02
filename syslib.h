/* syslib.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for the system call library
 */

#ifndef SYSLIB_H
#define SYSLIB_H

#include "common.h"

enum {
    IGNORE = 0
};

// Yield the current process
void yield(void);

// Terminate the current process
void exit(void);

// Get the process id
int getpid(void);

// Get the process priority
int getpriority(void);

// Set the process priority (1 = lowest priority, >1 = higher priority)
void setpriority(int);

// Sleep for the specified number of milliseconds
void sleep(int);

// Shut down the computer
void shutdown(void);

// Write the character to the serial port
// NOTE: Bochs is configured so that every character written to the serial
// port is appended to the file serial.out
void write_serial(char);

// Reads a character from the keyboard input buffer
// If the buffer is empty, the process will block until a character is available
char get_char(void);

// Start a new process from the given name
// If the process name could not be found, return -1
// If the process table is full, return -2
// If successful, return the PID of the new process
// NOTE: These a not really filenames, but actually refer to names in the ramdisk
pid_t spawn(const char *filename);

// Kills a process and marks the process as exited
// This should free-up the entry in the process table, so that later calls to
// spawn() can use that process table entry
// Returns -1 if that process does not exist, or 0 otherwise
int kill(pid_t pid);

// Blocks until the specified process terminates
// Returns -1 if that process does not exist and  0 otherwise.
int wait(pid_t pid);

// Opens the mailbox named 'name', or reates a new message box if it doesn't
// already exist
// A message box is a bounded buffer which holds up to MAX_MBOX_LENGTH items
// If it fails because the message box table is full, it will return -1
// Otherwise, it returns a message box id
mbox_t mbox_open(const char *name);

// Closes a message box
void mbox_close(mbox_t mbox);

// Enqueues a message onto a message box
// If the message box is full, the process will block until it can add the item
// You may assume that the message box id has been properly opened before this call
// The message is 'nbytes' bytes starting at offset 'msg'
// If the message is longer than MAX_MESSAGE_LENGTH, it will be truncated
void mbox_send(mbox_t mbox, void *msg, int nbytes);

// Receives a message from the specified message box
// If empty, the process will block until it can remove an item
// You may assume that the message box has been properly opened before this call
// The message is copied into 'msg' and no more than 'nbytes' bytes will by copied
// into this buffer; longer messages will be truncated
void mbox_recv(mbox_t mbox, void *msg, int nbytes);

#endif
