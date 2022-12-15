/**
 *  exception.h - exception handling
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: Lecture notes, Intel x86 docs
 */

#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

// Exception codes
#define DUMMY -1
#define DIV_BY_ZERO_CODE  0
#define RESERVED_CODE 1
#define NMI_CODE  2
#define BREAKPOINT_CODE  3
#define OVERFLOW_CODE  4
#define BOUND_RANGE_EXCEEDED_CODE  5
#define INVALID_OPCODE_CODE  6
#define COPROCESSOR_NOT_AVAILABLE_CODE  7
#define DOUBLE_FAULT_CODE  8
#define COPROCESSOR_SEGMENT_OVERRUN_CODE  9
#define INVALID_TSS_CODE  10
#define SEGMENT_NOT_PRESENT_CODE  11
#define STACK_SEG_FAULT_CODE  12
#define GENERAL_PROTECTION_CODE  13
#define PAGE_FAULT_CODE  14
#define FLOATING_POINT_ERROR_CODE  16
#define ALIGNMENT_CHECK_CODE  17
#define MACHINE_CHECK_CODE  18
#define SIMD_FLOATING_POINT_CODE  19

#ifndef ASM

#include "lib.h"
#include "syscalls.h"
#include "keyboard.h"
#include "signals.h"

// An unified handler entry point to show the exception message
extern void unified_exception_handler(const int code, const int error);

#endif /* ASM */
#endif /* _EXCEPTIONS_H */
