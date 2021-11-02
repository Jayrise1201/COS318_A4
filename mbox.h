/* mbox.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for mailbox implementation
 */

#ifndef MBOX_H
#define MBOX_H

// Includes definitions or MAX_MBOXEN, MAX_MBOX_LENGTH, and MAX_MESSAGE_LENGTH
#include "common.h"

void init_mbox(void);
mbox_t do_mbox_open(const char *name);
void do_mbox_close(mbox_t mbox);
int do_mbox_is_full(mbox_t mbox);
void do_mbox_send(mbox_t mbox, void *msg, int nbytes);
void do_mbox_recv(mbox_t mbox, void *msg, int nbytes);
unsigned int do_mbox_usage_count(mbox_t mbox);

#endif
