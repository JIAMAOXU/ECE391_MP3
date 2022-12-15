/**
 *  linkage.h - Assembly linkage between IDT and handler
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu, Eric Chen
 */

#ifndef _LINKAGE_H
#define _LINKAGE_H

#include "exceptions.h"
#include "interrupts.h"
#include "syscalls.h"
#include "signals.h"

#ifndef ASM

// ASM linkage to exception handling of division by zero
extern void handle_exception_division_by_zero();

// ASM linkage to exception handling of reserved
extern void handle_exception_reserved();

// ASM linkage to exception handling of NMI
extern void handle_exception_NMI();

// ASM linkage to exception handling of breakpoint
extern void handle_exception_breakpoint();

// ASM linkage to exception handling of overflow
extern void handle_exception_overflow();

// ASM linkage to exception handling of bound range exceeded
extern void handle_exception_bound_range_exceeded();

// ASM linkage to exception handling of invalid opcode
extern void handle_exception_invalid_opcode();

// ASM linkage to exception handling of coprocessor not available
extern void handle_exception_coprocessor_not_available();

// ASM linkage to exception handling of double fault
extern void handle_exception_double_fault();

// ASM linkage to exception handling of segment overrun
extern void handle_exception_coprocessor_segment_overrun();

// ASM linkage to exception handling of invalid TSS
extern void handle_exception_invalid_task_state_segment();

// ASM linkage to exception handling of segment not present
extern void handle_exception_segment_not_present();

// ASM linkage to exception handling of stack segment fault
extern void handle_exception_stack_segment_fault();

// ASM linkage to exception handling of general protection fault
extern void handle_exception_general_protection_fault();

// ASM linkage to exception handling of page fault
extern void handle_exception_page_fault();

// ASM linkage to exception handling of floating point
extern void handle_exception_exception_floating_point();

// ASM linkage to exception handling of alignment check
extern void handle_exception_alignment_check();

// ASM linkage to exception handling of machine check
extern void handle_exception_machine_check();

// ASM linkage to exception handling of SIMD floating point
extern void handle_exception_SIMD_floating_point();

// ASM linkage to interrupt handling of PIT
extern void handle_interrupt_PIT();

// ASM linkage to interrupt handling of keyboard
extern void handle_interrupt_keyboard();

// ASM linkage to interrupt handling of RTC
extern void handle_interrupt_RTC();

// ASM linkage to interrupt handling of system calls
extern void handle_syscall();

#endif /* ASM */
#endif /* _LINKAGE_H */
