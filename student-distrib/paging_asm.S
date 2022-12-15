#define ASM     1
#include "paging.h"

.globl load_page_dir

# void load_page_dir(unsigned int * pageDir)
# Inputs: pageDir - pointer to the starting address
# of the page directory made in header file.
# Return Value: void
# Function: takes care of loading page directory
# and setting enable paging register.
load_page_dir:

    pushl %ebp
    movl %esp, %ebp

    movl 8(%esp), %eax
    movl %eax, %cr3

    movl %cr4, %eax
    orl $0x00000010, %eax
    movl %eax, %cr4

    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    leave
    ret


