# linkage.S - Assembly linkage between IDT and handler
# Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
# Author: Peizhe Liu, Eric Chen

#define ASM     1
#include "linkage.h"

.align 4

.text
# void handle_exception_division_by_zero(void);
# ASM linkage to exception handling of division by zero
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_division_by_zero
handle_exception_division_by_zero:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $DIV_BY_ZERO_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_reserved(void);
# ASM linkage to exception handling of reserved
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_reserved
handle_exception_reserved:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $RESERVED_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_NMI(void);
# ASM linkage to exception handling of NMI
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_NMI
handle_exception_NMI:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $NMI_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_breakpoint(void);
# ASM linkage to exception handling of breakpoint
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_breakpoint
handle_exception_breakpoint:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $BREAKPOINT_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_overflow(void);
# ASM linkage to exception handling of overflow
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_overflow
handle_exception_overflow:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $OVERFLOW_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_bound_range_exceeded(void);
# ASM linkage to exception handling of bound range exceeded
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_bound_range_exceeded
handle_exception_bound_range_exceeded:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $BOUND_RANGE_EXCEEDED_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_invalid_opcode(void);
# ASM linkage to exception handling of invalid opcode
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_invalid_opcode
handle_exception_invalid_opcode:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $INVALID_OPCODE_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_coprocessor_not_available(void);
# ASM linkage to exception handling of coprocessor not available
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_coprocessor_not_available
handle_exception_coprocessor_not_available:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $COPROCESSOR_NOT_AVAILABLE_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_double_fault(void);
# ASM linkage to exception handling of double fault
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_double_fault
handle_exception_double_fault:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Adjust the error code location on stack
    movl 52(%esp), %edi
    pushl %edi
    
    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $DOUBLE_FAULT_CODE

    # Proceed to call the handler
    jmp handle_exception_with_code

# void handle_exception_coprocessor_segment_overrun(void);
# ASM linkage to exception handling of segment overrun
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_coprocessor_segment_overrun
handle_exception_coprocessor_segment_overrun:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $COPROCESSOR_SEGMENT_OVERRUN_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_invalid_task_state_segment(void);
# ASM linkage to exception handling of invalid TSS
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_invalid_task_state_segment
handle_exception_invalid_task_state_segment:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Adjust the error code location on stack
    movl 52(%esp), %edi
    pushl %edi
    
    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $INVALID_TSS_CODE

    # Proceed to call the handler
    jmp handle_exception_with_code

# void handle_exception_segment_not_present(void);
# ASM linkage to exception handling of segment not present
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_segment_not_present
handle_exception_segment_not_present:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Adjust the error code location on stack
    movl 52(%esp), %edi
    pushl %edi
    
    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $SEGMENT_NOT_PRESENT_CODE

    # Proceed to call the handler
    jmp handle_exception_with_code

# void handle_exception_stack_segment_fault(void);
# ASM linkage to exception handling of stack segment fault
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_stack_segment_fault
handle_exception_stack_segment_fault:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Adjust the error code location on stack
    movl 52(%esp), %edi
    pushl %edi
    
    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $STACK_SEG_FAULT_CODE

    # Proceed to call the handler
    jmp handle_exception_with_code

# void handle_exception_general_protection_fault(void);
# ASM linkage to exception handling of general protection fault
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_general_protection_fault
handle_exception_general_protection_fault:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Adjust the error code location on stack
    movl 52(%esp), %edi
    pushl %edi
    
    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $GENERAL_PROTECTION_CODE

    # Proceed to call the handler
    jmp handle_exception_with_code

# void handle_exception_page_fault(void);
# ASM linkage to exception handling of page fault
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_page_fault
handle_exception_page_fault:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Adjust the error code location on stack
    movl 52(%esp), %edi
    pushl %edi

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $PAGE_FAULT_CODE

    # Proceed to call the handler
    jmp handle_exception_with_code


# void handle_exception_exception_floating_point(void);
# ASM linkage to exception handling of floating point
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_exception_floating_point
handle_exception_exception_floating_point:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $FLOATING_POINT_ERROR_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_alignment_check(void);
# ASM linkage to exception handling of alignment check
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_alignment_check
handle_exception_alignment_check:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Adjust the error code location on stack
    movl 52(%esp), %edi
    pushl %edi
    
    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $ALIGNMENT_CHECK_CODE

    # Proceed to call the handler
    jmp handle_exception_with_code

# void handle_exception_machine_check(void);
# ASM linkage to exception handling of machine check
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_machine_check
handle_exception_machine_check:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $MACHINE_CHECK_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_SIMD_floating_point(void);
# ASM linkage to exception handling of SIMD floating point
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_exception_SIMD_floating_point
handle_exception_SIMD_floating_point:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Push placeholder
    pushl $DUMMY
    pushl $DUMMY

    # Gives additional argument for signal dispatch
    movl 72(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push arguments
    pushl $SIMD_FLOATING_POINT_CODE

    # Proceed to call the handler
    jmp handle_exception_without_code

# void handle_exception_with_code(void);
# Call the unified exception handler for exception w/ error#
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Restore all registers and flag register
handle_exception_with_code:
    # Call Unified Exception Handler
    call unified_exception_handler

    # Pop arguments
    addl $8, %esp

    # Save %eax again
    pushl %eax

    # Call for signal dispatch
    call sig_dispatch
    addl $4, %esp

    # Resolve PF
    addl $56, %esp
    popl %edi
    subl $5, %edi
    pushl %edi
    subl $56, %esp

    # Restore registers
    popfl
    popal
    popl %ds
    popl %es
    popl %fs
    popl %gs

    # Return
    addl $4, %esp
    iret

# void handle_exception_without_code(void);
# Call the unified exception handler for exception w/o error#
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Restore all registers and flag register
handle_exception_without_code:
    # Call Unified Exception Handler
    call unified_exception_handler

    # Pop arguments
    addl $12, %esp

    # Save %eax again
    pushl %eax

    # Call for signal dispatch
    call sig_dispatch
    addl $4, %esp

    # Resolve PF
    addl $56, %esp
    popl %edi
    subl $5, %edi
    pushl %edi
    subl $56, %esp

    # Restore registers
    popfl
    popal
    popl %ds
    popl %es
    popl %fs
    popl %gs

    # Return
    iret

# void handle_interrupt_PIT(void);
# ASM linkage to interrupt handling of PIT
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_interrupt_PIT
handle_interrupt_PIT:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Gives additional argument for signal dispatch
    movl 64(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push IRQ
    pushl $PIT_IRQ

    # Proceed to call the handler
    jmp handle_interrupt

# void handle_interrupt_keyboard(void);
# ASM linkage to interrupt handling of keyboard
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_interrupt_keyboard
handle_interrupt_keyboard:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Gives additional argument for signal dispatch
    movl 64(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push IRQ
    pushl $KEYBOARD_IRQ

    # Proceed to call the handler
    jmp handle_interrupt

# void handle_interrupt_RTC(void);
# ASM linkage to interrupt handling of RTC
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Pull all registers and flag register
.globl handle_interrupt_RTC
handle_interrupt_RTC:
    # Save registers
    pushl %gs
    pushl %fs
    pushl %es
    pushl %ds
    pushal
    pushfl

    # Gives additional argument for signal dispatch
    movl 64(%esp), %edi
    pushl %edi

    # Collect ESP
    call sig_collect_esp
    addl $4, %esp

    # Push IRQ#
    pushl $RTC_IRQ

    # Proceed to call the handler
    jmp handle_interrupt

# void handle_interrupt(void);
# Call the unified interrupt handler
#
# Interface: none
#    Inputs: none
#   Outputs: none
# Registers: Restore all registers and flag register
handle_interrupt:
    # Call Unified Interrupt Handler
    call unified_interrupt_handler

    # Pop argument
    addl $4, %esp

    # Save %eax again
    pushl %eax

    # Call for signal dispatch
    call sig_dispatch
    addl $4, %esp

    # Restore registers
    popfl
    popal
    popl %ds
    popl %es
    popl %fs
    popl %gs

    # Return
    iret

# void handle_syscall(void);
# Call the specified syscall
#
# Interface: EAX - syscall number, EBX, ECX, EDX - arguments
#    Inputs: none
#   Outputs: none
# Registers: Returns EAX, otherwise restored
.globl handle_syscall
handle_syscall:
        # Save registers
        pushfl

        pushl %gs
        pushl %fs
        pushl %es
        pushl %ds

        pushl %ebp
        pushl %edi
        pushl %esi
        pushl %edx
        pushl %ecx
        pushl %ebx
        pushl %esp

        # Caller save again
        pushl %edx
        pushl %ecx
        pushl %ebx
        pushl %eax

        # Gives additional argument for signal dispatch
        movl 76(%esp), %edi
        pushl %edi

        # Collect ESP
        call sig_collect_esp
        addl $4, %esp
        popl %eax
        popl %ebx
        popl %ecx
        popl %edx

        # Validate System call # in EAX
        # 6 Syscalls are supported
        cmpl $0, %eax
        jle syscall_invalid

        cmpl $10, %eax
        jg syscall_invalid

        # Push param registers
        # EBX -> 1st arg (name), ECX -> 2nd arg (flags), EDX -> 3rd arg (mode)
        pushl %edx
        pushl %ecx
        pushl %ebx

        # Call syscall handler via the jump table
        call *syscall_jump_table(, %eax, 4)

        # Post call, restore stack and finish
        addl $12, %esp
        jmp syscall_finish

    # If the argument is not valid
    syscall_invalid:
        pushl %eax
        call sys_invalid
        addl $4, %esp

    # Restore registers
    syscall_finish:
        # Save %eax again
        pushl %eax

        # Call for signal dispatch
        call sig_dispatch
        addl $4, %esp

        sti
        popl %esp
        popl %ebx
        popl %ecx
        popl %edx
        popl %esi
        popl %edi
        popl %ebp

        popl %ds
        popl %es
        popl %fs
        popl %gs

        popfl

        # Return
        iret

# Jump table for specific system calls
syscall_jump_table:
    .long 0, sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn
    .end
