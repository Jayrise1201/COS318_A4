/* mailbox.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Mailbox implementation
 */

#include "common.h"
#include "mbox.h"

typedef struct {
    // TODO: Fill this in
} Message;

typedef struct {
    char name[MBOX_NAME_LENGTH];
    // TODO: Fill this in
} MessageBox;

static MessageBox MessageBoxen[MAX_MBOXEN];

// Perform any system-startup initialization for the message boxes
void init_mbox(void) {
    (void) MessageBoxen;
    // TODO: Fill this in
}

// Opens the mailbox named 'name', or creates a new message box if it doesn't
// already exist. A message box is a bounded buffer which holds up to
// MAX_MBOX_LENGTH items. If it fails because the message box table is full, it
// will return -1. Otherwise, it returns a message box id
mbox_t do_mbox_open(const char *name) {
    (void) name;
    // TODO: Fill this in
    return -1;
}

// Closes a message box
void do_mbox_close(mbox_t mbox) {
    (void) mbox;
    // TODO: Fill this in
}

// Determine if the given message box is full. Equivalently, determine if sending
// to this message box would cause a process to block
int do_mbox_is_full(mbox_t mbox) {
    (void) mbox;
    // TODO: Fill this in
    return 1;
}

// Enqueues a message onto a message box. If the message box is full, the process
// will block until it can add the item. You may assume that the message box ID
// has been properly opened before this call. The message is 'nbytes' bytes
// starting at 'msg'
void do_mbox_send(mbox_t mbox, void *msg, int nbytes) {
    (void) mbox;
    (void) msg;
    (void) nbytes;
    // TODO: Fill this in
}

// Receives a message from the specified message box. If empty, the process will
// block until it can remove an item. You may assume that the message box has
// been properly opened before this call. The message is copied into 'msg'. No
// more than 'nbytes' bytes will be copied into this buffer; longer messages
// will be truncated
void do_mbox_recv(mbox_t mbox, void *msg, int nbytes) {
    (void) mbox;
    (void) msg;
    (void) nbytes;
    // TODO: Fill this in
}

// Returns the number of processes that have opened but not closed this mailbox
unsigned int do_mbox_usage_count(mbox_t mbox) {
    (void) mbox;
    return 0;
    // TODO: Fill this in
}
