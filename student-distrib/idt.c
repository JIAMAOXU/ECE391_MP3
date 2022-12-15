/**
 *  idt.c - IDT initialization
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: Lecture notes, Intel x86 docs
 */

#include "idt.h"

// File-scope functions
static void exception_entry_init(const int index);
static void interrupt_entry_init(const int index);
static void syscall_entry_init(const int index);

/* 
 * unified_interrupt_handler
 *   DESCRIPTION: Initialize IDT
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: interrupt will be dispatched to appropriate
 *                 handler accordingly.
 */
void idt_init()
{
    // Load Exception Handlers
    exception_entry_init(DIV_BY_ZERO_CODE);
    exception_entry_init(RESERVED_CODE);
    exception_entry_init(NMI_CODE);
    exception_entry_init(BREAKPOINT_CODE);
    exception_entry_init(OVERFLOW_CODE);
    exception_entry_init(BOUND_RANGE_EXCEEDED_CODE);
    exception_entry_init(INVALID_OPCODE_CODE);
    exception_entry_init(COPROCESSOR_NOT_AVAILABLE_CODE);
    exception_entry_init(DOUBLE_FAULT_CODE);
    exception_entry_init(COPROCESSOR_SEGMENT_OVERRUN_CODE);
    exception_entry_init(INVALID_TSS_CODE);
    exception_entry_init(SEGMENT_NOT_PRESENT_CODE);
    exception_entry_init(STACK_SEG_FAULT_CODE);
    exception_entry_init(GENERAL_PROTECTION_CODE);
    exception_entry_init(PAGE_FAULT_CODE);
    exception_entry_init(FLOATING_POINT_ERROR_CODE);
    exception_entry_init(ALIGNMENT_CHECK_CODE);
    exception_entry_init(MACHINE_CHECK_CODE);
    exception_entry_init(SIMD_FLOATING_POINT_CODE);
    SET_IDT_ENTRY(idt[DIV_BY_ZERO_CODE], &handle_exception_division_by_zero);
    SET_IDT_ENTRY(idt[RESERVED_CODE], &handle_exception_reserved);
    SET_IDT_ENTRY(idt[NMI_CODE], &handle_exception_NMI);
    SET_IDT_ENTRY(idt[BREAKPOINT_CODE], &handle_exception_breakpoint);
    SET_IDT_ENTRY(idt[OVERFLOW_CODE], &handle_exception_overflow);
    SET_IDT_ENTRY(idt[BOUND_RANGE_EXCEEDED_CODE], &handle_exception_bound_range_exceeded);
    SET_IDT_ENTRY(idt[INVALID_OPCODE_CODE], &handle_exception_invalid_opcode);
    SET_IDT_ENTRY(idt[COPROCESSOR_NOT_AVAILABLE_CODE], &handle_exception_coprocessor_not_available);
    SET_IDT_ENTRY(idt[DOUBLE_FAULT_CODE], &handle_exception_double_fault);
    SET_IDT_ENTRY(idt[COPROCESSOR_SEGMENT_OVERRUN_CODE], &handle_exception_coprocessor_segment_overrun);
    SET_IDT_ENTRY(idt[INVALID_TSS_CODE], &handle_exception_invalid_task_state_segment);
    SET_IDT_ENTRY(idt[SEGMENT_NOT_PRESENT_CODE], &handle_exception_segment_not_present);
    SET_IDT_ENTRY(idt[STACK_SEG_FAULT_CODE], &handle_exception_stack_segment_fault);
    SET_IDT_ENTRY(idt[GENERAL_PROTECTION_CODE], &handle_exception_general_protection_fault);
    SET_IDT_ENTRY(idt[PAGE_FAULT_CODE], &handle_exception_page_fault);
    SET_IDT_ENTRY(idt[FLOATING_POINT_ERROR_CODE], &handle_exception_exception_floating_point);
    SET_IDT_ENTRY(idt[ALIGNMENT_CHECK_CODE], &handle_exception_alignment_check);
    SET_IDT_ENTRY(idt[MACHINE_CHECK_CODE], &handle_exception_machine_check);
    SET_IDT_ENTRY(idt[SIMD_FLOATING_POINT_CODE], &handle_exception_SIMD_floating_point);

    // Load Interrupt Handlers
    interrupt_entry_init(PIT_IRQ + IRQ_OFFSET);
    interrupt_entry_init(KEYBOARD_IRQ + IRQ_OFFSET);
    interrupt_entry_init(RTC_IRQ + IRQ_OFFSET);
    syscall_entry_init(SYSCALL_INDEX);
    SET_IDT_ENTRY(idt[PIT_IRQ + IRQ_OFFSET], &handle_interrupt_PIT);
    SET_IDT_ENTRY(idt[KEYBOARD_IRQ + IRQ_OFFSET], &handle_interrupt_keyboard);
    SET_IDT_ENTRY(idt[RTC_IRQ + IRQ_OFFSET], &handle_interrupt_RTC);
    SET_IDT_ENTRY(idt[SYSCALL_INDEX], &handle_syscall);

    // Load Syscall Vector
    // NONE in CP1
}

/* 
 * exception_entry_init
 *   DESCRIPTION: Initialize Exception IDT Entry
 *   INPUTS: index - IDT Vector #
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: IDT entry at index will be initialized
 *                 as an entry to exception handler
 */
static void exception_entry_init(const int index)
{
    idt[index].present = 1;
    idt[index].dpl = 0;
    idt[index].reserved1 = 1;
    idt[index].reserved2 = 1;
    idt[index].size = 1;
    idt[index].seg_selector = KERNEL_CS;
}

/* 
 * interrupt_entry_init
 *   DESCRIPTION: Initialize Interrupt IDT Entry
 *   INPUTS: index - IDT Vector #
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: IDT entry at index will be initialized
 *                 as an entry to interrupt handler
 */
static void interrupt_entry_init(const int index)
{
    idt[index].present = 1;
    idt[index].dpl = 0;
    idt[index].reserved1 = 1;
    idt[index].reserved2 = 1;
    idt[index].size = 1;
    idt[index].seg_selector = KERNEL_CS;
}

/* 
 * syscall_entry_init
 *   DESCRIPTION: Initialize System Call IDT Entry
 *   INPUTS: index - IDT Vector #
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: IDT entry at index will be initialized
 *                 as an entry to system call vector
 */

static void syscall_entry_init(const int index)
{
    idt[index].present = 1;
    idt[index].dpl = 3;
    idt[index].reserved1 = 1;
    idt[index].reserved2 = 1;
    idt[index].size = 1;
    idt[index].seg_selector = KERNEL_CS;
}

