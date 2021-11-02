# entry.S
# COS 318, Fall 2019: Project 4 IPC and Process Management
# Entry points into the kernel for processes and threads

# Offset of the kernel context in a PCB
.equ  KERNEL, 8
# Offset of the user context in a PCB
.equ  USER, 48
# Size of a context
.equ  CONTEXT_SIZE, 40
# Offset of %esp in a context
.equ  STACK, 36
# Offset of field nested_count in a PCB
.equ  NESTED_COUNT, 88

# Save registers/flags to the specified offset in the current PCB
#define SAVE_CONTEXT(offset) \
    movl    %esp, scratch; \
    movl    current_running, %esp; \
    leal    ((offset)+CONTEXT_SIZE)(%esp), %esp; \
    pushl   scratch; \
    pushal; \
    pushfl; \
    movl    scratch, %esp
# As above, but save only %esp
#define SAVE_STACK(offset) \
    movl    %eax, scratch; \
    movl    current_running, %eax; \
    movl    %esp, ((offset)+STACK)(%eax); \
    movl    scratch, %eax
# Restore registers/flags from the specified offset in the current PCB
#define RESTORE_CONTEXT(offset) \
    movl    current_running, %esp; \
    leal    (offset)(%esp), %esp; \
    popfl;  \
    popal;  \
    popl    %esp
# As above, but restore only %esp
#define RESTORE_STACK(offset) \
    movl    %eax, scratch; \
    movl    current_running, %eax; \
    movl    ((offset)+STACK)(%eax), %esp; \
    movl    scratch, %eax
# Acknowledge a hardware interrupt
#define SEND_EOI \
    movl    %eax, scratch; \
    movb    $0x20, %al; \
    outb    %al, $0x20; \
    movl    scratch, %eax
# Increment disable_count, panicking on a double ENTER
# Interrupts should be disabled before this macro is used
#define ENTER_CRITICAL \
    movl    %eax, scratch; \
    movl    disable_count, %eax; \
    testl   %eax, %eax; \
    jnz     panic; \
    incl    %eax; \
    movl    %eax, disable_count; \
    movl    scratch, %eax
# Decrement disable_count, panicking on a double LEAVE
# Interrupts should be enabled after this macro is used
#define LEAVE_CRITICAL \
    movl    %eax, scratch; \
    movl    disable_count, %eax; \
    decl    %eax; \
    testl   %eax, %eax; \
    jnz     panic; \
    movl    %eax, disable_count; \
    movl    scratch, %eax
# Test current_running->nested_count, clobbering only eflags
#define TEST_NESTED_COUNT \
    movl    %eax, scratch; \
    movl    current_running, %eax; \
    movl    NESTED_COUNT(%eax), %eax; \
    testl   %eax, %eax; \
    movl    scratch, %eax

.data
# Temporary storage for 32-bit registers
scratch:
    .long  0
# Temporary storage for syscall return value
return_value:
    .long  0
# Signals status of interrupts
.globl  disable_count
disable_count:
    .long  0

.text
.code32

.globl  scheduler_entry
scheduler_entry:
    SAVE_CONTEXT(KERNEL)
    call    scheduler
    RESTORE_CONTEXT(KERNEL)
    ret

# System call interrupt handler
# 1. The system call number is passed in %eax
# 2. The argument is in %ebx
# 3. The return value is placed in %eax
# 4. When an interrupt is called, the eflags is pushed onto the stack along with
#    the return address
# 5. The IF flag is cleared, which prevents further interrupts (either hardware
#    or software) from being recognized until IF is set again
.globl  syscall_entry
syscall_entry:
    ENTER_CRITICAL
    SAVE_CONTEXT(USER)
    RESTORE_STACK(KERNEL)
    # return_value = system_call_helper(%eax, %ebx);
    pushl   %edx
    push    %ecx
    pushl   %ebx
    pushl   %eax
    call    system_call_helper
    movl    %eax, return_value
    addl    $16, %esp
    SAVE_STACK(KERNEL)
    RESTORE_CONTEXT(USER)
    # return return_value;
    movl    return_value, %eax
    LEAVE_CRITICAL
    iret

# Timer interrupt handler
.globl  irq0_entry
irq0_entry:
    SEND_EOI
    # Increment num_ticks
    pushl   %eax
    pushl   %edx
    movl    num_ticks, %eax
    movl    num_ticks+4, %edx
    addl    $1, %eax
    adcl    $0, %edx
    movl    %eax, num_ticks
    movl    %edx, num_ticks+4
    popl    %edx
    popl    %eax
    ENTER_CRITICAL
    # If we are not inside the kernel (nor kernel threads)
    TEST_NESTED_COUNT
    jnz     nonzero_nested_count
zero_nested_count:
    # A timer interrupt has occurred while a process was executing (not in the
    # kernel already) so we call yield to preempt the current process
    SAVE_CONTEXT(USER)
    RESTORE_STACK(KERNEL)
    movl    current_running, %eax
    incl    NESTED_COUNT(%eax) 
    call    do_yield_naked
    movl    current_running, %eax
    decl    NESTED_COUNT(%eax) 
    SAVE_STACK(KERNEL)
    RESTORE_CONTEXT(USER)
nonzero_nested_count:
exit_timer_interrupt:
    LEAVE_CRITICAL
    iret

.globl  irq1_entry
irq1_entry:
    ENTER_CRITICAL
    TEST_NESTED_COUNT
    jnz     irq1_inKernel
irq1_inProcess:
    SAVE_CONTEXT(USER)
    RESTORE_STACK(KERNEL)
    movl    current_running, %eax
    incl    NESTED_COUNT(%eax) 
    call    keyboard_interrupt
    movl    current_running, %eax
    decl    NESTED_COUNT(%eax) 
    SAVE_STACK(KERNEL)
    RESTORE_CONTEXT(USER)
    jmp leave_irq1  
irq1_inKernel:
    pushl   %eax
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    call    keyboard_interrupt
    popl    %edx
    popl    %ecx
    popl    %ebx
    popl    %eax
leave_irq1:
    SEND_EOI
    LEAVE_CRITICAL
    iret

# Functions are for use outside entry.S
.globl  fake_irq7_entry
fake_irq7_entry:
    SEND_EOI
    iret

.globl  enter_critical
enter_critical:
    cli
    ENTER_CRITICAL
    ret

.globl  leave_critical
leave_critical:
    LEAVE_CRITICAL
    sti
    ret

# Error Message
# Write "PANIC" to the screen and enter an infinite loop
panic:
    cli
    movl    $0xb8000, %edi
    movl    $0x4f50, %eax
    stosw
    movl    $0x4f41, %eax
    stosw
    movl    $0x4f4e, %eax
    stosw
    movl    $0x4f49, %eax
    stosw
    movl    $0x4f43, %eax
    stosw
infinite_loop:
    hlt
    jmp     infinite_loop
