/* kernel.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for kernel with IPC and process management
 */

#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"
#include "scheduler.h"

void print_status(void);

extern int (*syscall[NUM_SYSCALLS]) ();

enum {
    // Used to set Timer 0 frequency. For more details on how to set
    // PREEMPT_TICKS refer to The Undocumented PC, Page 978-979 (Port 40h).
    // Alternate values are for instance 1193 (1000 Hz) and 119 (10000 Hz). The
    // last value will be too have the timer interrupt fire too often, so try
    // something higher!
    PREEMPT_TICKS = 1193,  // Timer interrupt every 1ms

    // Kernel code segment descriptor
    KERNEL_CS = 1 << 3,

    // IDT = Interrupt Descriptor Table
    // GDT = Global Descriptor Table
    // TSS = Task State Segment
    GDT_SIZE = 7,
    IDT_SIZE = 49,
    IRQ_START = 32,         // Remapped irq0 IDT entry
    INTERRUPT_GATE = 0x0e,  // Interrupt gate descriptor
    IDT_SYSCALL_POS = 48,   // System call IDT entry
};

// Structure describing the contents of the idt and gdt registers used for
// loading the idt and gdt registers (See PMSA, Page 42)
struct point_t {
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed));

// Shut down the computer
void do_shutdown(void);

// Write a character to the serial port
void do_write_serial(int character);

// Determine the number of entries in the PCB table
int get_max_pcbs(void);

#endif
