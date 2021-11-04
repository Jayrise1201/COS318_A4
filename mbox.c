/* mailbox.c
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Mailbox implementation
 */

#include "common.h"
#include "mbox.h"
#include "sync.h"
#include "util.h"


typedef struct {
    // TODO: Fill this in
    void* msg; 
    int nbytes;
} Message;

typedef struct {
    char name[MBOX_NAME_LENGTH];
    // TODO: Fill this in
    Message messages[MAX_MBOX_LENGTH];
    int head;
    int tail;
    lock_t lock;
    condition_t full_buffer;
    condition_t empty_buffer;
    unsigned int usage_count;
    int size;
} MessageBox;

static MessageBox MessageBoxen[MAX_MBOXEN];

// Perform any system-startup initialization for the message boxes
void init_mbox(void) {
    (void) MessageBoxen;
    // TODO: Fill this in
    for (int i=0; i<MAX_MBOXEN; i++) {
        MessageBox messageBox = MessageBoxen[i];
        bcopy("", messageBox.name, 1);
        messageBox.head = 0;
        messageBox.tail = 0;
        messageBox.usage_count = 0;
        messageBox.size = 0;
        lock_init(&messageBox.lock);
        condition_init(&messageBox.full_buffer);
        condition_init(&messageBox.empty_buffer);
    }
}

// Opens the mailbox named 'name', or creates a new message box if it doesn't
// already exist. A message box is a bounded buffer which holds up to
// MAX_MBOX_LENGTH items. If it fails because the message box table is full, it
// will return -1. Otherwise, it returns a message box id
mbox_t do_mbox_open(const char *name) {
    (void) name;
    // TODO: Fill this in

    for (int i=0; i<MAX_MBOXEN; i++) {
        MessageBox mBox = MessageBoxen[i];
        if (same_string(mBox.name, "")) continue;
        if (same_string(mBox.name, name) == 1) {
            mBox.usage_count++;
            return i;
        }
    }

    int index = -1;

    for (int i=0; i<MAX_MBOXEN; i++) {
        MessageBox mBox = MessageBoxen[i];
        if (same_string(mBox.name, "")) {
            index = i;
            break;
        }
    }

    if (index == -1) return -1;

    bcopy((char *)name, MessageBoxen[index].name, strlen( (char *) name));
   
    MessageBoxen[index].usage_count = 1;

    return index;
}

// Closes a message box
void do_mbox_close(mbox_t mbox) {
    (void) mbox;
    MessageBox mBox = MessageBoxen[mbox];

    if (mBox.usage_count == 0) {
        bcopy("", mBox.name, 1);
        mBox.usage_count = 0;
        mBox.size = 0;
        mBox.tail =0;
        mBox.head = 0;
    }
    else {
        mBox.usage_count--;
    }

}

// Determine if the given message box is full. Equivalently, determine if sending
// to this message box would cause a process to block
int do_mbox_is_full(mbox_t mbox) {
    (void) mbox;
    // TODO: Fill this in
    MessageBox mBox = MessageBoxen[mbox];
    if (mBox.size == MAX_MBOX_LENGTH) {
        return 1;
    }
    else {
        return 0;
    }
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
    Message message;
    message.msg = msg;
    message.nbytes = nbytes;

    MessageBox mBox = MessageBoxen[mbox];

    lock_acquire(&mBox.lock);
    if (do_mbox_is_full(mbox)) {
        condition_wait(&mBox.lock, &mBox.full_buffer);
    }

    mBox.messages[mBox.head] = message;
    mBox.size++;
    mBox.head = (mBox.head + 1) % MAX_MBOX_LENGTH;
    
    lock_release(&mBox.lock);

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

    MessageBox mBox = MessageBoxen[mbox];

    lock_acquire(&mBox.lock);
    if (mBox.size == 0) {
        condition_wait(&mBox.lock, &mBox.empty_buffer);
    }

    Message message = mBox.messages[mBox.tail];
    int size = 0;
    if (message.nbytes < nbytes) {
        size = message.nbytes;
    }
    else {
        size = nbytes;
    }

    bcopy(message.msg, msg, size);

    lock_release(&mBox.lock);
}

// Returns the number of processes that have opened but not closed this mailbox
unsigned int do_mbox_usage_count(mbox_t mbox) {
    (void) mbox;
    // TODO: Fill this in
    MessageBox mBox = MessageBoxen[mbox];
    return mBox.usage_count;
}
