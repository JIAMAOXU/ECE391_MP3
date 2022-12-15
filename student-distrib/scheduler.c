/**
 *  scheduler.c - process switch and scheduler
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: 
 */

#include "scheduler.h"

// Initialize global variable
char* video_mem = (char *) VIDEO_MEM_ADDR;
uint8_t scheduler_enable = 0;

/* void switchEnvironmentInit()
 * Inputs: none
 * Return Value: none
 * Function: initialize scheduler environment
 */
extern void switchEnvironmentInit()
{
    terminals[0].video_backup_addr = (void*) VIDEO_BACKUP_ADDR0;
    terminals[0].video_backup_page = VIDEO_BACKUP_PAGE0;
    terminals[0].initialized = 0;
    terminals[0].screen_x = 0;
    terminals[0].screen_y = 0;
    terminals[0].echo = 0;
    terminals[0].vidmap = 0;
    
    terminals[1].video_backup_addr = (void*) VIDEO_BACKUP_ADDR1;
    terminals[1].video_backup_page = VIDEO_BACKUP_PAGE1;
    terminals[1].initialized = 0;
    terminals[1].screen_x = 0;
    terminals[1].screen_y = 0;
    terminals[1].echo = 0;
    terminals[1].vidmap = 0;

    terminals[2].video_backup_addr = (void*) VIDEO_BACKUP_ADDR2;
    terminals[2].video_backup_page = VIDEO_BACKUP_PAGE2;
    terminals[2].initialized = 0;
    terminals[2].screen_x = 0;
    terminals[2].screen_y = 0;
    terminals[2].echo = 0;
    terminals[2].vidmap = 0;
}

/* void switchTerminalInit(unsigned int terminal_id)
 * Inputs: terminal_id - terminal ID
 * Return Value: none
 * Function: initialize the specified terminal
 */
void switchTerminalInit(unsigned int terminal_id)
{
    if (terminal_id >= TERMINAL_COUNT)
    {
        printf("switchTerminalInit: Illegal terminal # specified: %d. Returning.", terminal_id);
        return;
    }

    progress = 1;

    pcb = (pcb_t*) (KERNEL_STACK_ADDR - (terminal_id + 1) * KERNEL_STACK_OFFSET);
    pcb->terminal_id = terminal_id;
    pcb->previous_id = terminal_id;

    terminals[terminal_id].initialized = 1;
    terminals[terminal_id].pcb = pcb;

    progress = 0;

    sys_execute((uint8_t*)("shell"));
}

/* void switchVidMem(unsigned int terminal_id)
 * Inputs: terminal_id - terminal ID
 * Return Value: none
 * Function: switch visible video memory page
 */
void switchVidMem(unsigned int terminal_id)
{
    if (terminal_id >= TERMINAL_COUNT)
    {
        printf("switchVidMem: Illegal terminal # specified: %d. Returning.", terminal_id);
        return;
    }

    progress = 1;

    // Backup the current VRAM
    memcpy(terminals[terminal_active].video_backup_addr, (void*) VIDEO_MEM_ADDR, VIDEO_MEM_BYTES);

    // Restore the backup
    memcpy((void*) VIDEO_MEM_ADDR, terminals[terminal_id].video_backup_addr, VIDEO_MEM_BYTES);

    // Reset video memory and coordinates to VRAM if now writing to its backup space
    if (video_mem == ((char *) (terminals[terminal_id].video_backup_addr)))
    {
        video_mem = (char *) VIDEO_MEM_ADDR;
        pageTableHigh[0].physicalAddress = VIDEO_MEM_PAGE;
        flushTLB();
    }

    // Change active terminal ID
    terminal_active = terminal_id;

    progress = 0;
}

/* void switchContext(unsigned int terminal_id)
 * Inputs: terminal_id - terminal ID to switch to
 * Return Value: none
 * Function: handle background context switch, switch VRAM and PCB
 */
void switchContext(unsigned int terminal_id)
{
    if (terminal_id >= TERMINAL_COUNT)
    {
        printf("switchContext: Illegal terminal # specified: %d. Returning.", terminal_id);
        return;
    }

    // Save VRAM information
    int current_terminal = pcb->terminal_id;
    terminals[current_terminal].screen_x = screen_x;
    terminals[current_terminal].screen_y = screen_y;

    // Save current PCB
    terminals[current_terminal].pcb = pcb;

    // Save current process context
    uint32_t ebp, esp;
    asm volatile (
        "movl %%ebp, %0  \n"
        "movl %%esp, %1  \n"
        : "=r"(ebp), "=r"(esp)
    );
    terminals[current_terminal].ebp = ebp;
    terminals[current_terminal].esp = esp;
    terminals[current_terminal].tss_esp = tss.esp0;

    // Reset screen coordinates
    screen_x = terminals[terminal_id].screen_x;
    screen_y = terminals[terminal_id].screen_y;

    // Determine If 4KB Vidpage needs to be mapped
    if (terminals[terminal_id].vidmap)
    {
        map4KBVidMemPage();
    }
    else
    {
        unMap4KBVidMemPage();
    }

    // Switch current video ram
    /*
     *  We map the video_mem to actual VRAM if and only if the
     *  target terminal ID matches the terminal ID of next process.
     * 
     *  Without scheduler, if the target terminal is not initialized,
     *  we change the mapping anyway. Pending changes here. --PL
     */
    if ((scheduler_enable && terminals[terminal_id].initialized && (terminal_active == ((terminals[terminal_id].pcb)->terminal_id)))
          || ((!scheduler_enable) && ((!terminals[terminal_id].initialized) || (terminal_active == ((terminals[terminal_id].pcb)->terminal_id)))))
    {
        // Next terminal is the active terminal, switch video memory to VRAM
        video_mem = (char *) VIDEO_MEM_ADDR;
        pageTableHigh[0].physicalAddress = VIDEO_MEM_PAGE;
        set_cursor_loc(screen_x, screen_y);
    }
    else
    {
        // Switch video memory to backup space.
        video_mem = (char *) (terminals[terminal_id].video_backup_addr);
        pageTableHigh[0].physicalAddress = terminals[terminal_id].video_backup_page;
    }
    
    // Check if the target terminal is initialized
    if (!(terminals[terminal_id].initialized))
    {
        // Not initialized, initialize VRAM
        clear();

        // Initialize the new terminal
        switchTerminalInit(terminal_id);
    }
    else
    {
        // Initialized, then we do the same thing in halt()...
        // Give up the current stack frame
        ebp = terminals[terminal_id].ebp;
        esp = terminals[terminal_id].esp;

        // Reset PCB pointer
        pcb = terminals[terminal_id].pcb;

        // Remap Program Page
        reMap4MBPage(pcb->process_id);
        flushTLB();

        // Relocate kernel stack
        tss.esp0 = terminals[terminal_id].tss_esp;

        // Do context switch
        asm volatile (
            "movl %0, %%ebp  \n"
            "movl %1, %%esp  \n"
            :
            : "r"(ebp), "r"(esp)
        );
    }

    /*
     *  Multiterminal was working for the first time on Nov. 27, 2022 over the Thanksgiving by Peizhe Liu.
     *  Even without going to the office hour, I designed the terminal_t, pcb_pool structure, and managed to figure out everything.
     *  Honestly I am so proud of my achievement. -- PL
     */
    // Should not return back to here anymore
    return;
}
