/**
 *  rtc.c - Virtualized RTC Driver
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: Lecture notes, OSDev
 */

#include "rtc.h"

/* File-scope variables */
static int open = 0;

/* 
 * rtc_handle
 *   DESCRIPTION: Handler to handle RTC interrupts.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: RTC interrupt will be received and EOI
 *                 will be send. Decrease the VRTC counters.
 */
void rtc_handle()
{
    // Receive the data
    outb(0x0C, RTC_IO_0);
    inb(RTC_IO_1);

    // Decrease the VRTC counters
    if (vrtc_counter[0] > 0) { vrtc_counter[0] = vrtc_counter[0] - RTC_FACTOR; }
    if (vrtc_counter[1] > 0) { vrtc_counter[1] = vrtc_counter[1] - RTC_FACTOR; }
    if (vrtc_counter[2] > 0) { vrtc_counter[2] = vrtc_counter[2] - RTC_FACTOR; }

    // Increase all alarm counters and ensure multitaskibility
    vrtc_alarm[0] += RTC_FACTOR;
    vrtc_alarm[1] += RTC_FACTOR;
    vrtc_alarm[2] += RTC_FACTOR;

    // Send alarm signal
    if (vrtc_alarm[0] >= RTC_ALARM_THERSHOLD)
    {
        vrtc_alarm[0] = 0;
        sig_set(terminals[0].pcb, ALARM);
    }
    if (vrtc_alarm[1] >= RTC_ALARM_THERSHOLD)
    {
        vrtc_alarm[1] = 0;
        sig_set(terminals[1].pcb, ALARM);
    }
    if (vrtc_alarm[2] >= RTC_ALARM_THERSHOLD)
    {
        vrtc_alarm[2] = 0;
        sig_set(terminals[2].pcb, ALARM);
    }

    // Send EOI
    send_eoi(RTC_IRQ);
    return;
}

/* 
 * rtc_open
 *   DESCRIPTION: Initialize and enable RTC interrupts, set
 *                frequency to 2 Hz.
 *   INPUTS: ignored
 *   OUTPUTS: none
 *   RETURN VALUE: 0 - success, -1 - failed
 *   SIDE EFFECTS: RTC interrupt will be enabled immidiately,
 *                 and rate will be default rate.
 */
int32_t rtc_open(const uint8_t* filename)
{
    if (open)
    {
        // Prevents double initialize
        printf("rtc_open: RTC is already opened.\n");
        return -1;
    }

    // Enable interrupt
    outb(0x8B, RTC_IO_0);
    char prev = inb(RTC_IO_1);
    outb(0x8B, RTC_IO_0);
    outb(prev | 0x40, RTC_IO_1);
    open = 1;

    // Reset all VRTC counters as we adapt multiterminal
    vrtc_counter[0] = 0;
    vrtc_counter[1] = 0;
    vrtc_counter[2] = 0;

    // We will use the default frequency, 1024Hz. No further initialization necessary.
    char freq_rate = 6;
    freq_rate &= 0x0F;
    outb(0x8A, RTC_IO_0);
    prev = inb(RTC_IO_1);
    outb(0x8A, RTC_IO_0);
    outb(((prev & 0xF0) | freq_rate), RTC_IO_1);

    return 0;
}

/* 
 * rtc_read
 *   DESCRIPTION: Spin until next RTC interrupt.
 *   INPUTS: fd - VRTC frequency, others - ignored
 *   OUTPUTS: none
 *   RETURN VALUE: 0 - success, -1 - failed
 *   SIDE EFFECTS: Will spin and wait for next interrupt.
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
    if (!open)
    {
        // Prevents error if RTC is not opened
        printf("rtc_read: RTC is not open.\n");
        return -1;
    }

    // Set VRTC counter
    vrtc_counter[pcb->terminal_id] = 1024 / fd;

    // Spin while waiting for interrupt
    while (vrtc_counter[pcb->terminal_id] > 0);

    // Return when VRTC counter is reset
    return 0;
}

/* 
 * rtc_write
 *   DESCRIPTION: As with the implementation of VRTC, this function
 *                no longer changes the RTC frequency. Instead,
 *                it will do a sanity check on the incoming new frequency.
 *   INPUTS: fd, nbytes - ignored
 *           buf - pointer to an int variable,
 *                 containing a new frequency.
 *                 Should be power of 2 and <= 1024 Hz.
 *   OUTPUTS: none
 *   RETURN VALUE: 0 - success, -1 - failed
 *   SIDE EFFECTS: RTC frequency will change.
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
    if (!open)
    {
        // Prevents error if RTC is not opened
        printf("rtc_write: RTC is not open.\n");
        return -1;
    }

    // Parameter check. Freq can only be power of 2 and cannot exceed 1024.
    if (!buf)
    {
        printf("rtc_write: Input pointer is not valid.\n");
        return -1;
    }

    int new_freq = *((int*) buf);
    char freq_rate = 15;
    int freq_valid = 0;
    while (freq_rate >= 6)
    {
        if (new_freq == (32768 >> (freq_rate - 1)))
        {
            freq_valid = 1;
            break;
        }
        freq_rate -= 1;
    }
    if (!freq_valid)
    {
        printf("rtc_write: New frequency %d Hz is not valid.\n", new_freq);
        return -1;
    }

    return 0;
}

/* 
 * rtc_write
 *   DESCRIPTION: Do nothing.
 *   INPUTS: ignored
 *   OUTPUTS: none
 *   RETURN VALUE: 0 - success, -1 - failed
 *   SIDE EFFECTS: none
 */
int32_t rtc_close(int32_t fd)
{
    if (!open)
    {
        // Prevents error if RTC is not opened
        printf("rtc_close: RTC is not open.\n");
        return -1;
    }

    // Do nothing.
    return 0;
}
