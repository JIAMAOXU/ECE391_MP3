/**
 *  scheduler.h - process switch and scheduler
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: 
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define TERMINAL_COUNT 3

#define VIDEO_MEM_BYTES 4096
#define VIDEO_MEM_ADDR 0xB8000
#define VIDEO_BACKUP_ADDR0 0xB9000
#define VIDEO_BACKUP_ADDR1 0xBA000
#define VIDEO_BACKUP_ADDR2 0xBB000
#define VIDEO_BACKUP_ADDR_EXTRA 0xBC000

#ifndef ASM

#include "paging.h"
#include "syscalls.h"
#include "lib.h"

// Global Variables
// Port the lib.c with multiterminal
char* video_mem;
int screen_x;
int screen_y;

// File structure for Terminal information
typedef struct terminal_t
{
    // Initialized once
    void* video_backup_addr;
    uint32_t video_backup_page;
    uint8_t initialized;

    // Saved on context switching
    uint32_t ebp;
    uint32_t esp;
    uint32_t tss_esp;
    int screen_x;
    int screen_y;

    // Saved when necessary
    struct pcb_t* pcb;
    uint8_t echo;
    uint8_t vidmap;
} terminal_t;

// Terminal table
terminal_t terminals[3];

// Scheduler enable
uint8_t scheduler_enable;

/* initialize environment */
extern void switchEnvironmentInit();

/* initialize the specified terminal */
extern void switchTerminalInit(unsigned int terminal_id);

/* switch visible video memory page */
extern void switchVidMem(unsigned int terminal_id);

/* handle context switch */
extern void switchContext(unsigned int terminal_id);

#endif /* ASM */
#endif /* _SCHEDULER_H */
