/**
 *  keyboard.c - Keyboard Driver
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu, Letian Zhang
 *  Sources: Lecture notes, Intel x86 docs
 */

#include "keyboard.h"

// Initialize Globale Variable
uint8_t terminal_active = 0;
uint8_t verbose_mode = 0;

// File-scope variables
/**
 * Key scancode to ASCII map, support main characters and numeric keys DOWN STRIKE only.
 * Capacity up to 0x39 (0x39 + 1 = 58 entries), unsupported keys are treated as NULL character.
 * See: https://www.plantation-productions.com/Webster/www.artofasm.com/DOS/pdf/apndxc.pdf
 */
static unsigned char lower_scancode_map[58] =
{
    0,   0, '1', '2', '3', '4', '5', '6', '7', '8',  '9', '0', '-',  '=', '\b', /* Unused, Esc, Num row, Bksp*/
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',  'p', '[', ']', '\n', 0, /* Tab, First Row, Enter, Ctrl */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',     /* Second Row, Grave accent, L SHIFT, Backslash*/
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',    0,   0,   0, ' '       /* Third Row, R SHIFT, PrtSc, ALT, SPACE */
};
static unsigned char upper_scancode_map[58] =
{
    0,   0, '!', '@', '#', '$', '%', '^', '&', '*',  '(', ')', '_',  '+', '\b', /* Unused, Esc, Num row, Bksp*/
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',  'P', '{', '}', '\n', 0, /* Tab, First Row, Enter, Ctrl */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',   0, '|',     /* Second Row, Grave accent, L SHIFT, Backslash*/
  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',    0,   0,   0, ' '       /* Third Row, R SHIFT, PrtSc, ALT, SPACE */
};

// Keyboard buffer variables
static unsigned char buf_local[3][KEYBOARD_BUFFER_SIZE];
static unsigned int buf_size[3];

// Caps lock, shift, and enter
static uint8_t caps_lock = 0;
static uint8_t shift = 0;
static uint8_t ctrl = 0;
static uint8_t alt = 0;

// Keyboard wait
static unsigned int keyboard_wait_flag = 0;

// Help message print handler
static void print_help_msg_handler();

// Process manager handler
static void pman_handler();

/* 
 * keyboard_init
 *   DESCRIPTION: Initialize the keyboard buffer
 *   INPUTS: curr_terminal - current terminal ID
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Keyboard buffer will be filled with
 *                 zeros, and buf_size will be reset.
 */
void keyboard_init(int curr_terminal)
{
    memset(&(buf_local[curr_terminal][0]), '\0', KEYBOARD_BUFFER_SIZE);
    buf_size[curr_terminal] = 0;
}

/* 
 * keyboard_handle
 *   DESCRIPTION: Handler to handle keyboard interrupts
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: keyboard interrupt will be received and EOI
 *                 will be send. Will set the trigger flag or
 *                 clear the screen if triggered. Will echo the
 *                 supported key.
 */
void keyboard_handle()
{
    // Receive the keystrike
    unsigned char scan_code = inb(0x60);

    // Send EOI
    send_eoi(KEYBOARD_IRQ);

    // Restore wait flag
    if (keyboard_wait_flag && scan_code == 0x1C)
    {
        keyboard_wait_flag = 0;
        return;
    }

    // Set the shift flag upon press & release of both shift
    if (scan_code == LSHIFT_PRESS || scan_code == RSHIFT_PRESS)
    {
        shift = 1;
    }
    if (scan_code == LSHIFT_REL || scan_code == RSHIFT_REL)
    {
        shift = 0;
    }

    // Set the ctrl flag upon press & release of both control
    // Although right control will generate an additional scancode E0 before 1D,
    // There's no need to distinguish it here.
    if (scan_code == CTRL)
    {
        ctrl = 1;
    }
    else if (scan_code == CTRL_REL)
    {
        ctrl = 0;
    }

    // Set the alt flag upon press & release of both alt
    if (scan_code == ALT)
    {
        alt = 1;
    }
    else if (scan_code == ALT_REL)
    {
        alt = 0;
    }

    // Caps lock (scancode 0x3A)
    if (scan_code == CAPS_LOCK && caps_lock == 0)
    {
        caps_lock = 1;
    }
    else if (scan_code == CAPS_LOCK && caps_lock == 1)
    {
        caps_lock = 0;
    }

    // Check supported main keystrokes (index < 58 and not NULL character)
    if ((scan_code < 58) && (ctrl != 1) && (alt != 1) && (terminals[terminal_active].echo) && (!keyboard_wait_flag))
    {
        unsigned char character = lower_scancode_map[scan_code];

        // Handle new line
        if (character == '\n')
        {
            // Call terminal driver to handle the buffer
            copy_buffer(&(buf_local[terminal_active][0]), buf_size[terminal_active], terminal_active);

            // Reset the buffer and print character
            keyboard_init(terminal_active);
            char character_string[2] = {character, '\0'};
            keyboard_put_active(character_string);
        }

        // Handle backspace
        else if (character == '\b')
        {
            // Backspace is valid only if buf_size > 0
            if (buf_size[terminal_active] > 0)
            {
                // Update the buffer and print character
                buf_size[terminal_active]--;
                if (buf_local[terminal_active][buf_size[terminal_active]] == '\t')
                {
                    // Check if the previous char is a tab
                    char character_string[2] = {character, '\0'};
                    keyboard_put_active(character_string);
                    keyboard_put_active(character_string);
                    keyboard_put_active(character_string);
                }
                buf_local[terminal_active][buf_size[terminal_active]] = '\0';
                char character_string[2] = {character, '\0'};
                keyboard_put_active(character_string);
            }
        }

        // Handle all other printable keystrokes if there are still space in buf
        else if ((character != 0) && (buf_size[terminal_active] < KEYBOARD_BUFFER_SIZE))
        {
            // Determine if the input is an alphabet
            unsigned char is_alpha = (scan_code >= 0x10 && scan_code <= 0x19) || (scan_code >= 0x1E && scan_code <= 0x26) || (scan_code >= 0x2C && scan_code <= 0x32);

            // Determine the correct case character
            if (is_alpha && ((caps_lock == 1 && shift == 0) || (caps_lock == 0 && shift == 1)))
            {
                // If it is alpha, print uppercase if CAPS && !SHIFT, or !CAPS && SHIFT
                character = upper_scancode_map[scan_code];
            }
            else if (!is_alpha && shift == 1)
            {
                // If it is not an alpha, print uppercase if SHIFT
                character = upper_scancode_map[scan_code];
            }

            // Update the buffer and print character
            buf_local[terminal_active][buf_size[terminal_active]] = character;
            buf_size[terminal_active]++;
            char character_string[2] = {character, '\0'};
            keyboard_put_active(character_string);
        }

        // Take the shortcut to handle the new key if not currently in the specified terminal
        if (pcb->terminal_id != terminal_active)
        {
            switchContext(terminal_active);
        }
    }

    // Combinational key support
    if ((!keyboard_wait_flag) && (!progress))
    {
        /*
         *  ------  Warning!  ------
         *  Combinational keys should happen on the active terminal.
         *  video_mem and coords are not necessarily correnspond to the active terminal.
         *  This should be adapted accordingly. -- PL
         */
        // Press CTRL+L to clear screen
        if ((scan_code == 0x26) && ctrl)
        {
            // Rise progress flag
            progress = 1;

            // Clear the active terminal VRAM and clear buffer
            // Save current video mem pointer
            char* vram_ptr_backup = video_mem;
            video_mem = (char *) VIDEO_MEM_ADDR;

            // Save coordinates
            int x_backup = screen_x;
            int y_backup = screen_y;

            // Clear VRAM
            clear();

            // Clear buffer
            keyboard_init(terminal_active);

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
            if (video_mem == ((char *) VIDEO_MEM_ADDR))
            {
                set_cursor_loc(screen_x, screen_y);
            }

            // Clear progress flag
            progress = 0;
        }

        // Press CTRL+C to send interrupt signal
        if ((scan_code == 0x2E) && ctrl)
        {
            char prompt[] = "\n<!> Interrupt.\n";
            keyboard_put_active(prompt);
            
            // Rise progress flag
            progress = 1;

            // Save current video mem pointer
            char* vram_ptr_backup = video_mem;
            video_mem = (char *) VIDEO_MEM_ADDR;

            // Save coordinates
            int x_backup = screen_x;
            int y_backup = screen_y;

            // Set coordinates accordingly
            if (pcb->terminal_id != terminal_active)
            {
                screen_x = terminals[terminal_active].screen_x;
                screen_y = terminals[terminal_active].screen_y;
            }

            // Deliver signal
            sig_set(terminals[terminal_active].pcb, INTERRUPT);

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
            if (video_mem == ((char *) VIDEO_MEM_ADDR))
            {
                set_cursor_loc(screen_x, screen_y);
            }

            // Clear progress flag
            progress = 0;
        }

        // Press CTRL+S to enable scheduler
        if ((scan_code == 0x1F) && ctrl)
        {
            if (scheduler_enable)
            {
                char prompt[] = "\n<i> Scheduler disabled.\n";
                keyboard_put_active(prompt);
                scheduler_enable = 0;
            }
            else
            {
                char prompt[] = "\n<i> Scheduler enabled.\n";
                keyboard_put_active(prompt);
                scheduler_enable = 1;
            }
        }

        // Press CTRL+V to enable verbose mode
        if ((scan_code == 0x2F) && ctrl)
        {
            if (verbose_mode)
            {
                char prompt[] = "\n<i> Verbose mode disabled.\n";
                keyboard_put_active(prompt);
                verbose_mode = 0;
            }
            else
            {
                char prompt[] = "\n<i> Verbose mode enabled.\n";
                keyboard_put_active(prompt);
                verbose_mode = 1;
            }
        }

        // Press CTRL+P to use process manager
        // Called blocking function, send EOI and return inside
        if ((scan_code == 0x19) && ctrl)
        {
            // Process manager
            char prompt[] = "Press ENTER to return.\nPress CTRL+ENTER to kill the current process.\n";
            keyboard_clear_and_wait(pman_handler, prompt, 2);

            // Hold Ctrl to quit the program
            if (ctrl)
            {
                // Send kill signal
                sig_set(pcb, SYSKILL);
            }

            // Return
            return;
        }

        // Press CTRL+H to display help message
        // Called blocking function, send EOI and return inside
        if ((scan_code == 0x23) && ctrl)
        {
            // Display message
            char prompt[] = "Press ENTER to return.\n";
            keyboard_clear_and_wait(print_help_msg_handler, prompt, 1);

            // Return
            return;
        }

        // Press ALT combinations to switch terminal
        // Called blocking function, send EOI and return inside
        if ((scan_code == 0x3B) && alt)
        {
            // Switch to Terminal 1
            switchVidMem(0);

            // Always switch immediately to handle the video
            switchContext(0);

            return;
        }
        else if ((scan_code == 0x3C) && alt)
        {
            // Switch to Terminal 2
            switchVidMem(1);

            // Always switch immediately to handle the video
            switchContext(1);

            return;
        }
        else if ((scan_code == 0x3D) && alt)
        {
            // Switch to Terminal 3
            switchVidMem(2);

            // Always switch immediately to handle the video
            switchContext(2);

            return;
        }
    }

    // Press CTRL+R to reboot the machine
    // Do something wild here, trigger a triple fault
    if ((scan_code == 0x13) && ctrl)
    {
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
    }
    
    return;
}

/* 
 * keyboard_wait
 *   DESCRIPTION: Keyboard wait to wait for user confirmation
 *   INPUTS: prompt - prompt string
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prompt and wait until user press ENTER. Warning: this function wIll put IF.
 */
void keyboard_wait(char* prompt)
{
    // Print the prompt
    printf(prompt);

    // Wait until flag is down
    keyboard_wait_flag = 1;
    sti();
    while (keyboard_wait_flag);
}

/* 
 * keyboard_put_active
 *   DESCRIPTION: Put string to active terminal
 *   INPUTS: prompt - prompt string
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Put string to active terminal
 */
void keyboard_put_active(char* prompt)
{
    /*
     *  ------  Warning!  ------
     *  This kernel debug tool will override whatever current PCB,
     *  and put stuff on the active terminal. --PL
     */
    // Rise progress flag
    progress = 1;

    // Check if can print directly
    if (terminal_active == (pcb->terminal_id))
    {
        printf(prompt);
        progress = 0;
        return;
    }

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
    
    // Print the prompt
    printf(prompt);

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
    if (video_mem == ((char *) VIDEO_MEM_ADDR))
    {
        set_cursor_loc(screen_x, screen_y);
    }

    // Clear Progress Flag
    progress = 0;
}

/* 
 * keyboard_clear_and_wait
 *   DESCRIPTION: Clear screen, execute a handler and wait for user confirmation
 *   INPUTS: handler - function pointer to be executed
 *           prompt - prompt string
 *           lines - prompt lines
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: clear screen, execute a handler, prompt on the last line,
 *                 and wait until user press ENTER. Warning: this function wIll put IF.
 *                 The handler message should not exceed a whole video page, or it will be incomplete.
 */
void keyboard_clear_and_wait(void (*handler)(), char* prompt, unsigned int lines)
{
    /*
     *  ------  Warning!  ------
     *  This kernel debug tool will override whatever current PCB,
     *  and put stuff on the active terminal. --PL
     */
    progress = 1;
    // Save current video mem pointer
    char* vram_ptr_backup = video_mem;
    video_mem = (char *) VIDEO_MEM_ADDR;

    // Save VRAM information
    memcpy((void *) VIDEO_BACKUP_ADDR_EXTRA, (void *) VIDEO_MEM_ADDR, VIDEO_MEM_BYTES);
    int x_backup = screen_x;
    int y_backup = screen_y;

    // Clear VRAM
    clear();

    // Execute handler
    handler();

    // Print the prompt at the last line
    while (screen_y <= (NUM_ROWS - lines - 2))
    {
        putc('\n');
    }
    
    // Print the prompt and wait
    keyboard_wait(prompt);

    // Restore VRAM
    memcpy((void *) VIDEO_MEM_ADDR, (void *) VIDEO_BACKUP_ADDR_EXTRA, VIDEO_MEM_BYTES);
    screen_x = x_backup;
    screen_y = y_backup;

    // Restore video mem pointer and reset cursor
    video_mem = vram_ptr_backup;
    if (video_mem == ((char *) VIDEO_MEM_ADDR))
    {
        set_cursor_loc(screen_x, screen_y);
    }

    progress = 0;
}

/* 
 * pman_handler
 *   DESCRIPTION: Process manager handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void pman_handler()
{
    set_color(INV_LIGHT_CYAN);
    printf("                            391OS-36 Process Manager                            \n");
    unset_color();

    printf("Terminal Info:\n");
    if (terminals[0].initialized)
    {
        printf("TI0 0x%#x, PID %u, ECHO %u, VMAP %u, COOR (%u, %u), %s\n", &terminals[0], terminals[0].pcb->process_id, terminals[0].echo, terminals[0].vidmap, terminals[0].screen_x, terminals[0].screen_y, terminals[0].pcb->command);
    }
    else
    {
        printf("TI0 0x%#x Uninitialized\n", &terminals[0]);
    }
    if (terminals[1].initialized)
    {
        printf("TI1 0x%#x, PID %u, ECHO %u, VMAP %u, COOR (%u, %u), %s\n", &terminals[1], terminals[1].pcb->process_id, terminals[1].echo, terminals[1].vidmap, terminals[1].screen_x, terminals[1].screen_y, terminals[1].pcb->command);
    }
    else
    {
        printf("TI1 0x%#x Uninitialized\n", &terminals[0]);
    }
    if (terminals[2].initialized)
    {
        printf("TI2 0x%#x, PID %u, ECHO %u, VMAP %u, COOR (%u, %u), %s\n", &terminals[2], terminals[2].pcb->process_id, terminals[2].echo, terminals[2].vidmap, terminals[2].screen_x, terminals[2].screen_y, terminals[2].pcb->command);
    }
    else
    {
        printf("TI2 0x%#x Uninitialized\n", &terminals[0]);
    }

    printf("\nPCB Pool:\n");
    if (pcb_pool[0] != NULL)
    {
        printf("PCB0 0x%#x, PID %u, TID %u, PPID %u, KSP 0x%#x, FD 0x%#x, %s\n", pcb_pool[0], pcb_pool[0]->process_id, pcb_pool[0]->terminal_id, pcb_pool[0]->previous_id, pcb_pool[0]->tss_esp, &(pcb_pool[0]->file_descriptor), pcb_pool[0]->command);
    }
    else
    {
        printf("PCB0 Vacant\n");
    }
    if (pcb_pool[1] != NULL)
    {
        printf("PCB1 0x%#x, PID %u, TID %u, PPID %u, KSP 0x%#x, FD 0x%#x, %s\n", pcb_pool[1], pcb_pool[1]->process_id, pcb_pool[1]->terminal_id, pcb_pool[1]->previous_id, pcb_pool[1]->tss_esp, &(pcb_pool[1]->file_descriptor), pcb_pool[1]->command);
    }
    else
    {
        printf("PCB1 Vacant\n");
    }
    if (pcb_pool[2] != NULL)
    {
        printf("PCB2 0x%#x, PID %u, TID %u, PPID %u, KSP 0x%#x, FD 0x%#x, %s\n", pcb_pool[2], pcb_pool[2]->process_id, pcb_pool[2]->terminal_id, pcb_pool[2]->previous_id, pcb_pool[2]->tss_esp, &(pcb_pool[2]->file_descriptor), pcb_pool[2]->command);
    }
    else
    {
        printf("PCB2 Vacant\n");
    }
    if (pcb_pool[3] != NULL)
    {
        printf("PCB3 0x%#x, PID %u, TID %u, PPID %u, KSP 0x%#x, FD 0x%#x, %s\n", pcb_pool[3], pcb_pool[3]->process_id, pcb_pool[3]->terminal_id, pcb_pool[3]->previous_id, pcb_pool[3]->tss_esp, &(pcb_pool[3]->file_descriptor), pcb_pool[3]->command);
    }
    else
    {
        printf("PCB3 Vacant\n");
    }
    if (pcb_pool[4] != NULL)
    {
        printf("PCB4 0x%#x, PID %u, TID %u, PPID %u, KSP 0x%#x, FD 0x%#x, %s\n", pcb_pool[4], pcb_pool[4]->process_id, pcb_pool[4]->terminal_id, pcb_pool[4]->previous_id, pcb_pool[4]->tss_esp, &(pcb_pool[4]->file_descriptor), pcb_pool[4]->command);
    }
    else
    {
        printf("PCB4 Vacant\n");
    }
    if (pcb_pool[5] != NULL)
    {
        printf("PCB5 0x%#x, PID %u, TID %u, PPID %u, KSP 0x%#x, FD 0x%#x, %s\n", pcb_pool[5], pcb_pool[5]->process_id, pcb_pool[5]->terminal_id, pcb_pool[5]->previous_id, pcb_pool[5]->tss_esp, &(pcb_pool[5]->file_descriptor), pcb_pool[5]->command);
    }
    else
    {
        printf("PCB5 Vacant\n");
    }

    // Scheduler Section
    printf("\nScheduler:\n");
    if (scheduler_enable)
    {
        printf("Enabled, ");
    }
    else
    {
        printf("Disabled, ");
    }
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
}

/* 
 * print_help_msg_handler
 *   DESCRIPTION: Help message print handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void print_help_msg_handler()
{
    set_color(INV_LIGHT_CYAN);
    printf("                              391OS-36 Help Center                              \n");
    unset_color();

    printf("Combinational Keys:\n");
    printf("ALT+F1   Switch to Terminal 1\n");
    printf("ALT+F2   Switch to Terminal 2\n");
    printf("ALT+F3   Switch to Terminal 3\n");
    printf("CTRL+C   Interrupt\n");
    printf("CTRL+L   Clear Screen\n");
    printf("CTRL+S   Enable/Disable Scheduler\n");
    printf("CTRL+V   Enable/Disable Verbose Mode\n");
    printf("CTRL+P   Start 391OS-36 Process Manager\n");
    printf("CTRL+H   Start 391OS-36 Help Center\n");
    printf("CTRL+R   Reboot the OS\n");

    printf("\nAbout 391OS-36:\n");
    printf("391OS-36 Milestone 3++, Build Wed. Dec. 7, 2022\n");
    printf("(C) Copyright 2022 Group 36. All Rights Reserved.\n\n");
    printf("This Opreating System is the Third MP of ECE 391 Fall 2022,\nComputer Systems Enginering,\n");
    printf("Department of Electrical and Computer Engineering,\n");
    printf("University of Illinois, Urbana-Champaign.\n");
}
