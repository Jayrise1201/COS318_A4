# helper.S
# COS 318, Fall 2019: Project 4 IPC and Process Management
# Helper functions used in some tests

.text
.code32
.globl  dirty_yield
dirty_yield:
    pushfl
    pushal
    movl    44(%esp), %eax
    movl    %eax, %ecx
    movl    %eax, %edx
    movl    %eax, %ebx
    movl    %eax, %ebp
    movl    %eax, %esi
    movl    %eax, %edi
    pushl   48(%esp)
    popfl
    pushl   $0
    call    *0xf00
    movl    %esp, (esp_save)
    movl    44(%esp), %esp
    pushfl
    pushal
    movl    (esp_save), %esp
    popl    %eax
    popal
    popfl
    ret
esp_save:
    .long  0
