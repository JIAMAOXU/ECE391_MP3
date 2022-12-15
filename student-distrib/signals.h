/**
 *  signals.h - signals support
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: 
 */

#ifndef _SIGNALS_H
#define _SIGNALS_H

// Supported 5+1 signals
#define DIV_ZERO    0     // Division by Zero
#define SEGFAULT    1     // All Other Exceptions
#define INTERRUPT   2     // CTRL+C User Interrupt
#define ALARM       3     // RTC Alarm Every 10 Seconds
#define USER1       4     // User Defined Signal
#define SYSKILL     5     // Task Kill by Kernel
#define NULLSIG     255   // Null Signal

// Maximum PID count
#define MAX_PID_COUNT 6

#include "types.h"

#ifndef ASM

#include "lib.h"

// Global Variables
// File structure for FD operations jump table
typedef struct file_op_ptr_t
{
    int32_t (*open)(const uint8_t* filename);
    int32_t (*close)(int32_t fd);
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
} file_op_ptr_t;

// File structure for File desciptor
typedef struct file_desc_t
{
    struct file_op_ptr_t* file_op_table_ptr;
    uint32_t inode;
    int32_t file_position;
    uint32_t flags;
} file_desc_t;

// File structure for PCB
typedef struct pcb_t
{
    uint8_t process_id;                 // Process ID, PID
    uint8_t terminal_id;                // Terminal ID, TID
    uint8_t previous_id;                // Previous PID, P_PID
    uint8_t sig_pending;                // Pending Signal
    uint8_t command [32];               // Command
    uint8_t arg_buffer [128];           // Argument
    uint8_t sig_stacksize;              // Signal linkage stacksize
    uint8_t sig_mask;                   // Signal Mask
    uint32_t arg_len;                   // Argument Length
    uint32_t ebp;                       // Parent EBP
    uint32_t esp;                       // Parent ESP
    uint32_t tss_esp;                   // Kernel Stack ESP, KSP
    uint32_t sig_ebp;                   // Signal linkage EBP
    uint32_t sig_esp;                   // Signal linkage ESP
    uint32_t sig_eax;                   // Signal linkage EAX
    uint32_t user_esp;                  // Last user ESP from linkage
    void* sig_handlers[5];              // Signal Handlers
    uint32_t sig_stackshot[27];         // Signal linkage stackshot
    file_desc_t file_descriptor [8];    // File Descriptor
} pcb_t;

/* Set a signal to a PCB */
extern void sig_set(pcb_t* pcb, uint8_t sig_num);

/* Dispatch the current signal to handler in current PCB */
extern uint32_t sig_dispatch(uint32_t eax);

/* Collect user esp */
extern void sig_collect_esp(uint32_t esp);

/* Linkage between user space handler return and sigreturn syscall */
extern void sig_linkage();

/* Call the default signal handler */
extern void sig_handle(uint8_t sig_num);

#endif /* ASM */
#endif /* _SIGNALS_H */
