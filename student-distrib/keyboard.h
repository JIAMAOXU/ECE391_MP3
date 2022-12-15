/**
 *  keyboard.h - Keyboard Driver
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu, Letian Zhang
 *  Sources: Lecture notes, Intel x86 docs
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

// KB IRQ Code
#define KEYBOARD_IRQ 1

#define LSHIFT_PRESS 0x2A
#define RSHIFT_PRESS 0x36
#define LSHIFT_REL   0xAA
#define RSHIFT_REL   0xB6
#define CAPS_LOCK    0x3A
#define ESCAPE       0x01
#define CTRL         0x1D
#define CTRL_REL     0x9D
#define ALT          0x38
#define ALT_REL      0xB8
#define UPPER_LOWER_DIFF 32 /* the difference in ascii values of a lowercase character and its uppercase variant */

#include "terminal.h"
#include "i8259.h"
#include "syscalls.h"
#include "scheduler.h"
#include "color.h"
#include "signals.h"

#ifndef ASM

#include "lib.h"

// Global Variable
// Currently active terminal
uint8_t terminal_active;

// Enable Verbose Mode
uint8_t verbose_mode;

// Initialize the keyboard buffer
extern void keyboard_init(int curr_terminal);

// Handler to handle keyboard interrupts
extern void keyboard_handle();

// Keyboard wait to wait for user confirmation
extern void keyboard_wait(char* prompt);

// Put string to active terminal
extern void keyboard_put_active(char* prompt);

// Clear screen, execute a handler and wait for user confirmation
extern void keyboard_clear_and_wait(void (*handler)(), char* prompt, unsigned int lines);

#endif /* ASM */
#endif /* _KEYBOARD_H */
