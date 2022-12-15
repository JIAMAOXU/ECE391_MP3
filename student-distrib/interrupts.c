/**
 *  interrupts.c - device enablers and interrupt handling
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: Lecture notes, Intel x86 docs
 */

#include "interrupts.h"

/* 
 * unified_interrupt_handler
 *   DESCRIPTION: An unified handler entry point to dispatch to
 *                interrupt handler accordingly
 *   INPUTS: irq - IRQ # given by linkage.
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: interrupt will be dispatched to appropriate
 *                 handler accordingly.
 */
void unified_interrupt_handler(const int irq)
{
    switch (irq)
    {
        case PIT_IRQ:
            pit_handle();
            break;
        case KEYBOARD_IRQ:
            keyboard_handle();
            break;
        case RTC_IRQ:
            rtc_handle();
            break;
        default:
            break;
    }
    return;
}
