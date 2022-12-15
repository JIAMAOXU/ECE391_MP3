/**
 *  pit.h - Programmable Interval Timer Driver
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: 
 */

#ifndef _PIT_H
#define _PIT_H

// PIT IRQ Code
#define PIT_IRQ 0
#define PIT_IO_0 0x40
#define PIT_IO_1 0x41
#define PIT_IO_2 0x42
#define PIT_IO_3 0x43

#include "i8259.h"
#include "scheduler.h"
#include "keyboard.h"

#ifndef ASM

// We are not using syscall interface on PIT, exclusively for OS
// Handle PIT Interrupts
extern void pit_handle();

#endif /* ASM */
#endif /* _PIT_H */
