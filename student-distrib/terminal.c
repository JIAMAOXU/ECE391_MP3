/**
 *  terminal.c - Terminal Driver
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Letian Zhang, Peizhe Liu
 *  Sources: Lecture notes, OSDev
 */

#include "terminal.h"

// File-scope variables
// Local terminal buffer
// Altered by copy_buffer
static unsigned char buf_local[3][KEYBOARD_BUFFER_SIZE];
static unsigned int buf_ready[3];
static unsigned int buf_size_in[3];

/* 
 * terminal_open
 *   DESCRIPTION: Initialize the local terminal buffer.
 *                Modified in CP5 to support scheduler.
 *   INPUTS: filename - used to pass terminal ID
 *           if NULL, initialize all terminals.
 *   OUTPUTS: none
 *   RETURN VALUE: 0 - success
 *   SIDE EFFECTS: Local terminal buffer will be reinitialized.
 *                 Not enforced double calls, can be called
 *                 multiple times to initialize the buffer.
 */
int32_t terminal_open(const uint8_t* filename)
{
    if (filename == NULL)
    {
        memset(&buf_local[0][0], '\0', sizeof(KEYBOARD_BUFFER_SIZE));
        buf_ready[0] = 0;
        buf_size_in[0] = 0;
        memset(&buf_local[1][0], '\0', sizeof(KEYBOARD_BUFFER_SIZE));
        buf_ready[1] = 0;
        buf_size_in[1] = 0;
        memset(&buf_local[2][0], '\0', sizeof(KEYBOARD_BUFFER_SIZE));
        buf_ready[2] = 0;
        buf_size_in[2] = 0;
    }
    else
    {
        memset(&(buf_local[*filename][0]), '\0', sizeof(KEYBOARD_BUFFER_SIZE));
        buf_ready[*filename] = 0;
        buf_size_in[*filename] = 0;
    }
    return 0;
}

/* 
 * terminal_read
 *   DESCRIPTION: Read the keyboard buffer until a new line
 *                character was detected.
 *   INPUTS: n - number of bytes should be written to buf
 *           fd - ignored
 *   OUTPUTS: buf - buffer to write
 *            WARNING! The actual size of this buffer should
 *            always larger then specified n.
 *   RETURN VALUE: int32_t - number of bytes actually written to buf
 *                 -1 - failed
 *   SIDE EFFECTS: Local terminal buffer will be reinitialized.
 */
int32_t terminal_read(int32_t fd, void* buf, uint32_t n)
{
    // Parameter safe check
    if (buf == NULL)
    {
        printf("terminal_read: Input buf pointer is not valid.\n");
        return -1;
    }

    // Unfortunately, there are no good way to check buf's actual size...
    if (n > KEYBOARD_BUFFER_SIZE)
    {
    //    printf("terminal_read: Requested size %d is invalid.\n", n);
    //    return -1;
    }

    // Reset terminal local buf by reopening
    terminal_open(&(pcb->terminal_id));

    // Spin and wait, when buf is ready, copy buf
    while(!buf_ready[pcb->terminal_id]);
    memcpy(buf, &(buf_local[pcb->terminal_id][0]), min(buf_size_in[pcb->terminal_id], n));

    return (int32_t) min(buf_size_in[pcb->terminal_id], n);
}

/* 
 * terminal_write
 *   DESCRIPTION: Write the supplied buf to screen.
 *   INPUTS: n - number of bytes should be written to screen
 *           buf - buffer to read
 *           fd - ignored
 *           WARNING! The actual size of this buffer should
 *           always larger than specified n.
 *           WARNING! n is unsigned. This means passing a
 *           negative value will be treated as a extreme large
 *           value and is possible to crash the driver (and
 *           will crash the kernel)!
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t - number of bytes actually written to screen
 *                 -1 - failed
 *   SIDE EFFECTS: Supplied buf will be written to screen.
 */
int32_t terminal_write(int32_t fd, const void* buf, uint32_t n)
{
    // Parameter safe check
    if (buf == NULL)
    {
        printf("terminal_write: Input buf pointer is not valid.\n");
        return -1;
    }
    // Unfortunately, there are no good way to check buf's actual size...

    // Loop and print requested buffer
    int i;
    for (i = 0; i < n; i++)
    {
        putc(*((unsigned char*) buf + i));
    }

    return n;
}

/* 
 * terminal_close
 *   DESCRIPTION: Do nothing.
 *   INPUTS: ignored
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int32_t terminal_close(int32_t fd)
{
    return -1;
}

/* 
 * enable_cursor
 *  DESCRIPTION: Enables the cursor.
 *  INPUTS: uint8_t - cursor_start, uint8_t - cursor_end
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECTS: enables the cursor.
 *  REFERENCES: https://wiki.osdev.org/Text_Mode_Cursor
 */
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
    // 0x0A: the index of cursor start register
    // 0x3D4: the index port of CRTC registers
    // 0x3D5: the data port of CRTC registers
    // 0xC0: bit mask to enable cursor and clear cursor scan line start
    outb(0x0A, 0x3D4);
    outb((inb(0x3D5) & 0xC0) | cursor_start, 0x3D5);

    outb(0x0B, 0x3D4);
    outb((inb(0x3D5) & 0xE0) | cursor_end, 0x3D5);
}

/* 
 * copy_buffer
 *   DESCRIPTION: Copy the supplied external buffer.
 *                Modified in CP5 to support scheduler.
 *   INPUTS: size - number of bytes should be copied
 *           buf - buffer to copy
 *           terminal_id - terminal buffer ID
 *           NOTE: this function is called exclusively by
 *           keyboard driver. Its size will not exceed 128,
 *           and the buf size will always be 128.
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Local buf will be replaced with the supplied buf.
 */
void copy_buffer(unsigned char* buf, unsigned int size, uint8_t terminal_id)
{
    // Parameter safe check
    if (buf == NULL)
    {
        printf("copy_buffer: Input buf pointer is not valid.\n");
        buf_ready[terminal_id] = 1;
        return;
    }

    // Copy the buffer
    buf_size_in[terminal_id] = size;
    memcpy(&(buf_local[terminal_id][0]), buf, buf_size_in[terminal_id]);
    buf_ready[terminal_id] = 1;
    return;
}
