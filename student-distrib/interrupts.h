/**
 *  interrupts.h - device enablers and interrupt handling
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: Lecture notes, Intel x86 docs
 */

#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include "i8259.h"
#include "rtc.h"
#include "keyboard.h"
#include "pit.h"

#ifndef ASM

#include "lib.h"

// An unified handler entry point to dispatch the interrupt
extern void unified_interrupt_handler(const int irq);

#endif /* ASM */
#endif /* _INTERRUPTS_H */
