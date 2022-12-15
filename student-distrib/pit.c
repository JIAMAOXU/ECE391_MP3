/**
 *  pit.c - Programmable Interval Timer Driver
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: 
 */

#include "pit.h"

void pit_handle()
{
    // Send EOI
    send_eoi(PIT_IRQ);

    // Determine work environment
    if (progress || ((!scheduler_enable) && (pcb->terminal_id == terminal_active)))
    {
        return;
    }

    // Switch the process
    switch (pcb->terminal_id)
    {
        case 0:
            switchContext(1);
            break;

        case 1:
            switchContext(2);
            break;

        case 2:
            switchContext(0);
            break;

        default:
            break;
    }
}
