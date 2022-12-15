/**
 *  exception.c - exception handling
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: Lecture notes, Intel x86 docs
 */

#include "exceptions.h"

/* 
 * unified_exception_handler
 *   DESCRIPTION: An unified handler entry point to show the
 *                exception message
 *   INPUTS: code - exception code given by linkage.
 *           error - exception error code given by processor or
 *                   linkage.
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: exception message will show and will halt
 *                 the kernel.
 */
void unified_exception_handler(const int code, const int error)
{
    /*
     *  ------  Warning!  ------
     *  This kernel debug tool will override whatever current PCB,
     *  and put stuff on the active terminal. --PL
     */
    // Backup progress flag
    uint8_t progress_indicator = progress;

    // Set Progress Flag
    progress = 1;

    // Mask the signal
    pcb->sig_mask = 1;

    // Save current video mem pointer
    char* vram_ptr_backup = video_mem;
    video_mem = (char *) VIDEO_MEM_ADDR;

    // Save VRAM information
    int x_backup = screen_x;
    int y_backup = screen_y;
    
    // Set coordinates accordingly
    if (pcb->terminal_id != terminal_active)
    {
        screen_x = terminals[terminal_active].screen_x;
        screen_y = terminals[terminal_active].screen_y;
    }

    // Print the exception message
    printf("\n<!> ");
    switch (code)
    {
        case DIV_BY_ZERO_CODE:
            printf("Division by Zero");
            break;
        case RESERVED_CODE:
            printf("RESERVED");
            break;
        case NMI_CODE:
            printf("Non-maskable Interrupt");
            break;
        case BREAKPOINT_CODE:
            printf("Breakpoint");
            break;
        case OVERFLOW_CODE:
            printf("Overflow");
            break;
        case BOUND_RANGE_EXCEEDED_CODE:
            printf("Bound Range Exceeded");
            break;
        case INVALID_OPCODE_CODE:
            printf("Invalid Opcode");
            break;
        case COPROCESSOR_NOT_AVAILABLE_CODE:
            printf("Coprocessor Not Available");
            break;
        case DOUBLE_FAULT_CODE:
            printf("Double Fault");
            break;
        case COPROCESSOR_SEGMENT_OVERRUN_CODE:
            printf("Coprocessor Segment Overrun");
            break;
        case INVALID_TSS_CODE:
            printf("Invalid Task State Segment");
            break;
        case SEGMENT_NOT_PRESENT_CODE:
            printf("Segment Not Present");
            break;
        case STACK_SEG_FAULT_CODE:
            printf("Stack Segment Fault");
            break;
        case GENERAL_PROTECTION_CODE:
            printf("General Protection Fault");
            break;
        case PAGE_FAULT_CODE:
            printf("Page Fault");
            break;
        case FLOATING_POINT_ERROR_CODE:
            printf("x87 Floating Point");
            break;
        case ALIGNMENT_CHECK_CODE:
            printf("Alignment Check");
            break;
        case MACHINE_CHECK_CODE:
            printf("Machine Check");
            break;
        case SIMD_FLOATING_POINT_CODE:
            printf("SIMD Floating-Point");
            break;
        default:
            printf("UNKNOWN");
    }

    printf(" Exception");
    error_sound();
    // Print error code if it holds a message
    if (error != -1)
    {
        printf(" 0x%#x\n", error);
    }
    else
    {
        putc('\n');
    }

    // Prints necessary information
    printf("Active TID %u, Running %s, PID %u, TID %u, ", terminal_active, pcb->command, pcb->process_id, pcb->terminal_id);
    uint32_t theory_tss = KERNEL_STACK_ADDR - pcb->process_id * KERNEL_STACK_OFFSET - 4;
    printf("KSP 0x%#x ", tss.esp0);
    if ((pcb->tss_esp == tss.esp0) && (tss.esp0 == theory_tss))
    {
        printf("Verified\n");
    }
    else
    {
        printf("Unverified\n", theory_tss);
    }

    // Halt the program or OS
    // Determine if the exception happened is recoverable
    if (progress_indicator)
    {
        // Do something wild here, trigger a triple fault
        char prompt[] = "<!> Exception happened in kernel. Press any key to reboot the OS.\n";
        keyboard_wait(prompt);

        // Put a wrong KSP here
        uint32_t ebp = 0xFFFFFFFF;
        uint32_t esp = 0xFFFFFFFF;
        asm volatile (
            "movl %0, %%ebp  \n"
            "movl %1, %%esp  \n"
            :
            : "r"(ebp), "r"(esp)
        );

        // Trigger the bomb
        ebp = *((uint32_t*) 0x00000000);

        // Should never go through here.
    }
    else
    {
        char prompt[] = "Press ENTER to continue.\n";
        keyboard_wait(prompt);

        // Deliver the signal
        pcb->sig_mask = 0;
        if (code == DIV_BY_ZERO_CODE)
        {
            sig_set(pcb, DIV_ZERO);
        }
        else
        {
            sig_set(pcb, SEGFAULT);
        }
        
        // Restore coordinates accordingly
        if (pcb->terminal_id != terminal_active)
        {
            terminals[terminal_active].screen_x = screen_x;
            terminals[terminal_active].screen_y = screen_y;
            screen_x = x_backup;
            screen_y = y_backup;
        }

        // Restore video mem pointer and reset cursor
        video_mem = vram_ptr_backup;
        if ((video_mem == ((char *) VIDEO_MEM_ADDR)))
        {
            set_cursor_loc(screen_x, screen_y);
        }

        // Clear progress flag and kill the process
        progress = 0;
    }

    // Should never return here
    return;
}
