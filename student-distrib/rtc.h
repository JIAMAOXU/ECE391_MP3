/**
 *  rtc.h - Virtualized RTC Driver
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: Lecture notes, OSDev
 */

#ifndef _RTC_H
#define _RTC_H

// RTC IRQ Code
#define RTC_IRQ 8
#define RTC_IO_0 0x70
#define RTC_IO_1 0x71

// RTC Factor
#define RTC_FACTOR 1.75
#define RTC_ALARM_THERSHOLD 10240


#include "types.h"
#include "i8259.h"
#include "scheduler.h"
#include "signals.h"

#ifndef ASM

#include "lib.h"

// VRTC Counter for each terminal
float vrtc_counter[3];

// VRTC Counter for alarm signal
float vrtc_alarm[3];

// Handler to handle RTC interrupts.
extern void rtc_handle();

// Initialize and enable RTC interrupts, set frequency to 2 Hz.
extern int32_t rtc_open(const uint8_t* filename);

// Spin until next RTC interrupt.
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

// Change RTC frequency.
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

// Do nothing.
extern int32_t rtc_close(int32_t fd);

#endif /* ASM */
#endif /* _RTC_H */
