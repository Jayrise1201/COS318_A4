/* interrupt.h
 * COS 318, Fall 2019: Project 4 IPC and Process Management
 * Definitions and types for basic interrupt handlers
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

enum {
    NUM_EXCEPTIONS = 15,
    // Used by the exception handlers to define where to print messages
    START_COL = 40,
};

// This macro clears a line, before printing a string to the line
// PARAMS: l = line, s = string, f = print function, v = value
#define PRINT_INFO(l,s,f,v) { \
    print_str(l, START_COL, "                                        "); \
    print_str(l, START_COL, s); \
    f(l, START_COL+15, v); \
}

// This macro expands to a full function definition, allowing us to easily
// define multiple similar exception handlers without a lot of trouble
// PARAMS: name = exception name, str = error string, error_code = TRUE if
// exception has an error code
#define INTERRUPT_HANDLER(name,str, error_code) \
void name (void) { \
    asm volatile ("movl %%esp, %0" : "=q" (esp));\
    s = (uint32_t*) esp;\
    PRINT_INFO( 0, "Error", print_str, (str)); \
    PRINT_INFO( 1, "PID", print_int, current_running->pid); \
    PRINT_INFO( 2, "Stack", print_hex, (( int)s)); \
    PRINT_INFO( 3, "EIP", print_hex, error_code ? s[1] : s[0]); \
    PRINT_INFO( 4, "CS", print_hex, error_code ? s[2] : s[1]); \
    PRINT_INFO( 5, "Error code", print_hex, error_code ? s[0] : 0); \
    PRINT_INFO( 6, "Kernel reentry", print_int, \
                current_running->nested_count); \
    print_status(); \
    asm volatile ("hlt"); \
}

// Copy the segment descriptor index "seg" into the DS and ES registers
void load_data_segments(int seg);

// Helper function for system calls
int system_call_helper(int fn, int arg1, int arg2, int arg3);

// Handler for fake irqs
void fake_irq7(void);

// Mask/unmask a hardware interrupt source
void mask_hw_int(int irq);
void unmask_hw_int(int irq);

// Used for any other interrupt which should not happen
void bogus_interrupt(void);

// Exception handlers (See PMSA Chapter 12)
void exception_0(void);   // Divide by zero
void exception_1(void);   // Debug
void exception_2(void);   // NMI
void exception_3(void);   // Breakpoint
void exception_4(void);   // INTO instruction
void exception_5(void);   // BOUNDS instruction
void exception_6(void);   // Invalid opcode
void exception_7(void);   // Device not available
void exception_8(void);   // Double-fault encountered
void exception_9(void);   // Coprocessor segment overrun
void exception_10(void);  // Invalid TSS fault
void exception_11(void);  // Segment not present
void exception_12(void);  // Stack exception
void exception_13(void);  // General protection
void exception_14(void);  // Page fault

// The following prototypes are for functions located in entry.S
extern int32_t disable_count;
void syscall_entry(void);

// Entry points for the above interrupt handlers (irq0 doesn't have an explicit
// interrupt handler, it calls the scheduler directly)
void irq0_entry(void);
void fake_irq7_entry(void);
void irq1_entry(void);

// Enter/leave a critical region
void enter_critical(void);
void leave_critical(void);

#endif
