/**
 *  syscalls.h - system calls
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Eric Chen, Peizhe Liu
 *  Sources: 
 */

#ifndef _SYSCALLS_H
#define _SYSCALLS_H

// Syscall Vector Index on IDT
#define SYSCALL_INDEX 0x80

// Program Load, Stack Address
#define PROGRAM_PAGE_ADDR 0x08048000
#define PROGRAM_STACK_ADDR 0x08400000

// Kernel Stack Address, Offset
#define KERNEL_STACK_ADDR 0x00800000
#define KERNEL_STACK_OFFSET 0x2000

// FD #
#define FD_STDIN 0
#define FD_STDOUT 1

// FD Flags
#define FD_FLAG_EMPTY 0
#define FD_FLAG_RTC 1
#define FD_FLAG_DIR 2
#define FD_FLAG_FILE 3

// Maximum Command Length
#define MAX_CMD_LEN 31

#ifndef ASM

#include "types.h"
#include "lib.h"
#include "paging.h"
#include "color.h"
#include "file_system.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"
#include "signals.h"

// Global Variables
// PCB of current process
struct pcb_t* pcb;

// In Progress Flag
volatile uint8_t progress;

// Used by halt-execution return routine
int32_t halt_status;

// PCB pool for execute to find next available PID
struct pcb_t* pcb_pool[MAX_PID_COUNT];

// Takes input command and execute the corresponding program
extern int32_t sys_execute(const uint8_t* command);

// Takes status command and halt the program
extern int32_t sys_halt(uint8_t status);

// System call for file, rtc, or dir open
extern int32_t sys_open(const uint8_t* filename);

// System call for closing a file
extern int32_t sys_close(int32_t fd);

// System call for read a file
extern int32_t sys_read(int32_t fd, void* buf, int32_t nbytes);

// System call for write a file
extern int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes);

// Get the trimmed argument after the command name
extern int32_t sys_getargs(uint8_t* buf, int32_t nbytes);

// Map the video memory to 0x8400000 to 0x8401000
extern int32_t sys_vidmap(uint8_t** screen_start);

// Set new handler when a signal is received
extern int32_t sys_set_handler(int32_t signum, void* handler_address);

// Return from user space signal handler
extern int32_t sys_sigreturn(void);

// Print out # for invalid syscall
extern int32_t sys_invalid(unsigned int callnum);

extern void play_sound (int freq_number, uint32_t duration);

extern void error_sound();

extern void OS_start_sound();
#endif /* ASM */
#endif /* _SYSCALLS_H */
