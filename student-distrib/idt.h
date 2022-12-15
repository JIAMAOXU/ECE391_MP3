/**
 *  idt.h - IDT initialization
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: Lecture notes, Intel x86 docs
 */

#ifndef _IDT_H
#define _IDT_H

// Offset to translate IRQ # to IDT vector #
#define IRQ_OFFSET 0x20

#include "x86_desc.h"
#include "linkage.h"

#ifndef ASM

// Initialize the IDT
extern void idt_init();

#endif /* ASM */
#endif /* _IDT_H */
