/* keyboard.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for keyboard device driver
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kernel.h"

// Constants
enum {
    RIGHT_SHIFT = 1,
    LEFT_SHIFT = 2,
    CAPS_SHIFT = 4,
    CONTROL = 8,
    ALT = 16,
};

#define KEYBOARD_MBOX_NAME "keyboard"

// Used for scancode conversion
struct ascii {
    unsigned char no_shift;          // No shift ascii code (lower case)
    unsigned char shift;             // Shift acii code (upper case)
    unsigned char control;           // Control ascii code
    void (*handler)(unsigned char);  // Pointer to handler
};

struct character {
    unsigned char character;  // Ascii code
    unsigned char attribute;  // Shift status
    unsigned char scancode;
};

// Called by the kernel to initialize keyboard handling.
// NOTE: It uses mailboxes, so this must be initialized after the mailboxes
void keyboard_init( void);

// Keyboard interrupt handler
void keyboard_interrupt( void);

// Function to read a character from the keyboard queue
int do_getchar();

#endif
