/**
 *  terminal.h - Terminal Driver
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Letian Zhang, Peizhe Liu
 *  Sources: Lecture notes, OSDev
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#define CURSOR_START 14
#define CURSOR_END   15
#define KEYBOARD_BUFFER_SIZE 128

#ifndef ASM

#include "lib.h"

// Initialize the local terminal buffer.
extern int32_t terminal_open();

// Read the keyboard buffer until a new line character was detected.
extern int32_t terminal_read(int32_t fd, void* buf, uint32_t n);

// Write the supplied buf to screen.
extern int32_t terminal_write(int32_t fd, const void* buf, uint32_t n);

// Do nothing.
extern int32_t terminal_close(int32_t fd);

// Enables the cursor.
extern void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

// Copy the supplied external buffer.
extern void copy_buffer(unsigned char* buf, unsigned int size, uint8_t terminal_id);

#endif /* ASM */
#endif /* _TERMINAL_H */
