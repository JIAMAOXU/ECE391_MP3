/**
 *  syscalls.c - system calls
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Eric Chen, Peizhe Liu, Jiamao Xu
 *  Sources: 
 */

#include "syscalls.h"

// File-scope helper functions
// Helper function to find next available PID in poll
static int find_next_pid();

// File-scope data structures
/* Structs containing pointers to read, write, open, and close funcs */
struct file_op_ptr_t file_sys_calls =
{
    file_open,
    file_close,
    file_read,
    file_write
};

struct file_op_ptr_t rtc_sys_calls =
{
    rtc_open,
    rtc_close,
    rtc_read,
    rtc_write
};

struct file_op_ptr_t dir_sys_calls =
{
    dir_open,
    dir_close,
    dir_read,
    dir_write
};

/* Function: sys_execute
 * Description: takes input command and execute the corresponding program
 * Inputs: 
 * command - uint8_t pointer giving command
 * Outputs - return -1 if unsuccessful
 * Side Effects: none
 */
int32_t sys_execute(const uint8_t* command)
{
    // Set progress flag
    progress = 1;

    // Try to find next available PID
    int available_pid = find_next_pid();

    // Check if can have more programs
    if (available_pid == -1)
    {
        printf("<!> Maximum process limit exceed. Please quit some programs.\n");
        error_sound();
        progress = 0;
        return -1;
    }

    // Sanity check for multiterminal base terminal init

    // Parameter Check
    if (command == NULL)
    {
        printf("<!> Invalid command.\n");
        error_sound();
        progress = 0;
        return -1;
    }

    // Get program name
    unsigned int prog_name_start, prog_name_len;
    for (prog_name_start = 0; prog_name_start < strlen((int8_t*) command); prog_name_start++)
    {
        if (command[prog_name_start] != ' ') { break; }
    }

    command += prog_name_start;

    for (prog_name_len = 0; prog_name_len < strlen((int8_t*) command); prog_name_len++)
    {
        if (command[prog_name_len] == ' ') { break; }
    }

    // Program name check
    if (prog_name_len == 0)
    {
        printf("<!> Program name is empty.\n");
        error_sound();
        progress = 0;
        return -1;
    }    

    // Extract the program names
    uint8_t prog_name[prog_name_len + 1];
    memcpy(prog_name, command, prog_name_len);
    prog_name[prog_name_len] = '\0';

    if (verbose_mode)
    {
        // Prints out program name
        printf("<i> Trying to start program \"");
        int pname_i;
        for (pname_i = 0; pname_i < prog_name_len; pname_i++)
        {
            putc(prog_name[pname_i]);
        }
        printf("\" on terminal_id %u, available_pid %u\n", pcb->terminal_id, available_pid);
    }
    
    

    // Executable check
    if (check_executable((uint8_t *) prog_name) == -1)
    {
        progress = 0;
        return -1;
    }

    /****** Passed all checks, try to start the program ******/
    // Get Arguments
    command += prog_name_len;

    // Create temp arg buffer
    uint8_t arg_buffer_local[128];
    uint32_t arg_len_local = 0;

    if (command[0] != '\0')
    {
        // Trim the start of command
        unsigned int arg_start;
        for (arg_start = 0; arg_start < strlen((int8_t*) command); arg_start++)
        {
            if (command[arg_start] != ' ') { break; }
        }

        command += arg_start;

        if (command[0] !='\0')
        {
            // ^Trim the end of command
            unsigned int arg_end;
            for (arg_end = strlen((int8_t*) command) - 1; arg_end >= 0; arg_end--)
            {
                if (command[arg_end] != ' ') { break; }
            }

            // Copy the argument to buffer
            arg_len_local = ++arg_end;
            memcpy(arg_buffer_local, command, arg_len_local);
            arg_buffer_local[arg_len_local] = '\0';
        }
        else
        {
            // Rest of commands are empty
            arg_len_local = 0;
        }
    }
    else
    {
        // String after program name is empty
        arg_len_local = 0;
    }

    if (verbose_mode)
    {
        // Prints out argument
        if (arg_len_local > 0)
        {
            printf("<i> Get argument \"");
            int arg_i;
            for (arg_i = 0; arg_i < arg_len_local; arg_i++)
            {
                putc(arg_buffer_local[arg_i]);
            }
            printf("\" from command, arg_len_local %u\n", arg_len_local);
        }
    }
    
    // Create page for new program
    reMap4MBPage(available_pid);

    // Load code into memory
    dentry_t prog_dentry;
    uint8_t* prog_page_addr = (uint8_t*) PROGRAM_PAGE_ADDR;
    int32_t prog_size = return_file_size((uint8_t *) prog_name);
    if (read_dentry_by_name((uint8_t *) prog_name, &prog_dentry) == -1 || prog_size == -1)
    {
        progress = 0;
        return -1;
    }
    read_data(prog_dentry.inode_number, 0, prog_page_addr, prog_size);
    uint32_t prog_eip = (prog_page_addr[27] << 24) | (prog_page_addr[26] << 16) | (prog_page_addr[25] << 8) | prog_page_addr[24];

    // Create PCB
    pcb_t* pcb_pointer = (pcb_t*) (KERNEL_STACK_ADDR - (available_pid + 1) * KERNEL_STACK_OFFSET);
    pcb_pointer->process_id = available_pid;
    pcb_pointer->terminal_id = pcb->terminal_id;   // Get from current PCB
    pcb_pointer->previous_id = pcb->process_id;    // Get from current PCB
    pcb_pointer->sig_pending = NULLSIG;
    pcb_pointer->user_esp = NULL;
    pcb_pointer->sig_stacksize = 0;
    pcb_pointer->sig_mask = 0;
    memset(pcb_pointer->sig_stackshot, 0, 80);     // uint32_t is 4 bytes * 20 spaces
    memset(pcb_pointer->sig_handlers, 0, 20);      // void* is 4 bytes * 5 handlers
    memcpy(&(pcb_pointer->arg_buffer), &arg_buffer_local, arg_len_local);
    pcb_pointer->arg_len = arg_len_local;

    // Parse command
    if (prog_name_len > MAX_CMD_LEN)
    {
        prog_name_len = MAX_CMD_LEN;
    }
    memset(&(pcb_pointer->command), '\0', (MAX_CMD_LEN + 1));
    memcpy(&(pcb_pointer->command), &prog_name, prog_name_len);
    (pcb_pointer->command)[MAX_CMD_LEN] = '\0';

    // Initialize file desc array 
    int fd_i;
    for (fd_i = 0; fd_i < 8; fd_i++)
    {
        (pcb_pointer->file_descriptor)[fd_i].flags = FD_FLAG_EMPTY;
    }

    // Save old EBP and return address for halt
    uint32_t ebp, esp;
    asm volatile (
        "movl %%ebp, %0  \n"
        "movl %%esp, %1  \n"
        : "=r"(ebp), "=r"(esp)
    );
    pcb_pointer->ebp = ebp;
    pcb_pointer->esp = esp;
    
    // Save and relocate kernel stack
    tss.esp0 = KERNEL_STACK_ADDR - available_pid * KERNEL_STACK_OFFSET - 4;
    pcb_pointer->tss_esp = tss.esp0;

    // Switch current PCB
    pcb = pcb_pointer;

    // Modify TI
    terminals[pcb->terminal_id].pcb = pcb;
    
    // Reset VRTC alarm counter
    vrtc_alarm[pcb->terminal_id] = 0;

    // Mark the PCB pool position as occupied
    pcb_pool[available_pid] = pcb;

    // Push arguments and call IRET
    uint32_t prog_ss = USER_DS;
    uint32_t prog_esp = PROGRAM_STACK_ADDR - 4;
    uint32_t prog_cs = USER_CS;

    // Clear progress flag
    progress = 0;

    asm volatile (
        "pushl %0  \n"
        "pushl %1  \n"
        "pushfl    \n"
        "pushl %2  \n"
        "pushl %3  \n"
        "iret      \n"
        :
        : "r"(prog_ss), "r"(prog_esp), "r"(prog_cs), "r"(prog_eip)
        : "memory", "cc"
    );

    // Should never return at here
    return -1;
}

/* Function: sys_halt
 * Description: takes status command and halt the program
 * Inputs: 
 * command - uint8_t status command
 * Outputs - return 256 if exception
 * Side Effects: none
 */
int32_t sys_halt(uint8_t status)
{
    // Set progress flag
    progress = 1;

    // Determine halt reason
    halt_status = status;
    if (!status && (((pcb->sig_pending) == DIV_ZERO) || ((pcb->sig_pending) == SEGFAULT)))
    {
        // Halt by exception, set 256 status
        halt_status = 256;
    }

    // Free the PCB pool, following the old logic
    pcb_pool[pcb->process_id] = NULL;

    if (verbose_mode)
    {
        printf("\n<i> Terminating program PID %u on terminal_id %u, halt_status %d\n", pcb->process_id, pcb->terminal_id, halt_status);
    }
    else
    {
        putc('\n');
    }

    if (pcb->process_id > 5)
    {
        printf("<!> Invalid PID %u, system halted.", pcb->process_id);
        error_sound();
        while (1);
    }

    // Tell the user about the information
    if (pcb->process_id < TERMINAL_COUNT)
    {
        printf("<!> Base shell of the terminal_id %u is dead, trying to restart.\n", pcb->terminal_id);
        printf("<!> playing sound...\n");
        OS_start_sound();
        sys_execute((uint8_t *)"shell");

        // This should never be called
        progress = 0;
        return -1;
    }

    // Close all fd. except first two
    int fd_i;
	for(fd_i = 2; fd_i < 7; fd_i++)
    {
        sys_close(fd_i);
    }

    // Tear down vidmap page
    unMap4KBVidMemPage();

    // Remap program page
    reMap4MBPage(pcb->previous_id);

    // Give up current stack frame, restore execute EBP, linkage status
    uint32_t ebp = pcb->ebp;
    uint32_t esp = pcb->esp;

    // Modify TI
    terminals[pcb->terminal_id].pcb = pcb_pool[pcb->previous_id];
    terminals[pcb->terminal_id].vidmap = 0;

    // Reset PCB pointer
    pcb = pcb_pool[pcb->previous_id];

    // Relocate kernel stack
    tss.esp0 = pcb->tss_esp;

    // Clear progress flag
    progress = 0;

    asm volatile (
        "movl %0, %%ebp  \n"
        "movl %1, %%esp  \n"
        :
        : "r"(ebp), "r"(esp)
    );
    return halt_status;
}

/* Function: sys_open
 * Description: system call for file, rtc, or dir open
 * Inputs: filename - uint8_t* pointer to filename string
 * Outputs:	return 0 if index is found. return -1 if no matching index dentry found
 * Side Effects: none
 */
int32_t sys_open(const uint8_t* filename)
{
    dentry_t currFileDentry;
    int i;
    /* Parameter check */
    if (filename == NULL || *filename =='\0') { return -1; }
    /* File existance check */
    if (read_dentry_by_name((uint8_t *)(filename), &currFileDentry) == -1)
    {
        printf("<!> No matching file was found to open the FD.\n");
        error_sound();
        return -1;
    }
    /* assume that everytime sys_open is called, file is not populated in fd_array */
    /* loop through fd_array to find next avalible spot to open file */
    for (i = 2; i < 8; i++)
    {
        if ((pcb->file_descriptor)[i].flags == FD_FLAG_EMPTY)  /* check for first open slot to open file */
        {
            /*  Check if file is rtc, dir, or file, and populate fd entry.
             *  ========= NOTES FOR FD ENTRY DATASTRUCTURE -- PL =========
             *  file_op_table_ptr: - ALL SUPPORTED TYPE: FOT *.
             *                     - OTHERS: won't open, fill NULL.
             *  inode: - FD_FLAG_FILE: current opened file inode #.
             *         - FD_TYPE_RTC: current VRTC frequency.
             *         - OTHERS: not valid and unused, fill 0.
             *  file_position: - FD_FLAG_DIR: current read file # in file_system.
             *                 - FD_FLAG_FILE: current read file # in file.
             *                 - OTHERS: not valid and unused, fill 0.
             *  flags: - ALL SUPPORTED TYPE: FD_FLAG_RTC, FD_FLAG_DIR, or FD_FLAG_FILE.
             *         - OTHERS: won't open, use FD_FLAG_EMPTY.
             **/
            if (currFileDentry.file_type == FILE_TYPE_RTC)
            {
                /* file is rtc*/
                (pcb->file_descriptor)[i].file_op_table_ptr = &rtc_sys_calls;
                (pcb->file_descriptor)[i].inode = 2;                            // Default Frequency is 2 Hz
                (pcb->file_descriptor)[i].file_position = 0;
                (pcb->file_descriptor)[i].flags = FD_FLAG_RTC;
            }
            else if(currFileDentry.file_type == FILE_TYPE_DIR)
            {
                /* file is directory call */
                (pcb->file_descriptor)[i].file_op_table_ptr = &dir_sys_calls;
                (pcb->file_descriptor)[i].inode = 0;
                (pcb->file_descriptor)[i].file_position = 0;
                (pcb->file_descriptor)[i].flags = FD_FLAG_DIR;
            }
            else if (currFileDentry.file_type == FILE_TYPE_FILE)
            {
                /* populate file operations table pointer */
                (pcb->file_descriptor)[i].file_op_table_ptr = &file_sys_calls;
                (pcb->file_descriptor)[i].inode = currFileDentry.inode_number;
                (pcb->file_descriptor)[i].file_position = 0;
                (pcb->file_descriptor)[i].flags = FD_FLAG_FILE;
            }
            else
            {
                printf("<!> Invalid or unsupported file type to open the FD.\n");
                error_sound();
                return -1;
            }       
            return i;  /* leave for loop and return as file has been populated properly */
        }
    }
    printf("<!> File descriptor array is full.\n");
    error_sound();
    return -1;
}

/* Function: sys_close
 * Description: system call for closing a file
 * and removing its fd from the file desc array
 * Inputs: fd - int32_t representing index to file desc array
 * Outputs:	return 0 on a successful close, return -1 if fd is invalid or no file found
 * Side Effects: none
 */
int32_t sys_close(int32_t fd)
{
    if ((fd < 2) || (fd > 7))    /* initial fd sanity check*/
    {
        printf("<!> Invalid file descriptor index to close the FD.\n");
        error_sound();
        return -1;
    }
    if ((pcb->file_descriptor)[fd].flags == FD_FLAG_EMPTY)
    {
        // Already closed
        return -1;
    }
    (pcb->file_descriptor)[fd].file_op_table_ptr = 0;
    (pcb->file_descriptor)[fd].file_position = 0;
    (pcb->file_descriptor)[fd].inode = 0;
    (pcb->file_descriptor)[fd].flags = FD_FLAG_EMPTY;
    return 0;
}

/* Function: sys_read
 * Description: system call for read a file
 * and return number of bytes read
 * Inputs: fd - int32_t representing index to file desc array
 *         buf - write data to this buffer
 *         nbytes - bytes to be read
 * Outputs:	return bytes been read on a successful read, return -1 if fd is invalid or no file found
 * Side Effects: none
 */
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes)
{
    /* ========= NOTES FOR ARGUMENT AND RETURN VALUE CONVENTIONS -- PL =========
    *  FD_STDIN: - Accept ECHO in the current terminal.
    *            - Not based on file_system. Call terminal_read directly.
    *            - Pass all arguments directly.
    *            - Return directly outside sys_read.
    *            - file_position is not available, do nothing.
    *  FD_STDOUT: - Cannot read from an output stream.
    *             - Impossible to reach as we do sanity check.
    *             - Return -1.
    *  FD_FLAG_RTC: - Decline ECHO in the current terminal.
    *               - Based on file_system. Call based on FOT (File Operation Table) in FDE (FD Entry).
    *               - Pass inode in FDE to arg0 as we handle them in VRTC driver.
    *               - Return directly outside sys_read.
    *               - file_position is unused, do nothing.
    *  FD_FLAG_DIR: - Decline ECHO in the current terminal.
    *               - Based on file_system. Call based on FOT in FDE.
    *               - Pass file_position in FDE to arg0.
    *               - Return directly outside sys_read.
    *               - file_position increase by 1 if not returning 0.
    *  FD_FLAG_FILE: - Decline ECHO in the current terminal.
    *                - Based on file_system. Call based on FOT in FDE.
    *                - Pass file_position in FDE to arg0, inode in FDE to buf[0].
    *                - Return directly outside sys_read.
    *                - file_position increase by bytes_read.
    *  FD_FLAG_EMPTY: - Unopened FDE, why are we reading from it?
    *                 - Impossible to reach as we do sanity check.
    *                 - Return -1.
    **/
    sti();
    /* Initial sanity checks */ 
    if (buf == NULL || fd > 7 || fd < 0 || nbytes < 0 || fd == FD_STDOUT)
    {
        printf("<!> Invalid function parameter for reading.\n");
        error_sound();
        return -1;
    }
    if ((fd >= 2) && ((pcb->file_descriptor)[fd].flags == FD_FLAG_EMPTY))
    {
        printf("<!> File descriptor is not open for reading.\n");
        error_sound();
        return -1;
    }

    /* To hold return values from child syscalls */
    int32_t bytes_read;

    /* STDIN case */
    if (fd == FD_STDIN)
    {
        terminals[pcb->terminal_id].echo = 1;
        bytes_read = terminal_read(fd, buf, nbytes);
        terminals[pcb->terminal_id].echo = 0;
        return bytes_read;
    }

    /* FD_FLAG_RTC case */
    else if ((pcb->file_descriptor)[fd].flags == FD_FLAG_RTC)
    {
        bytes_read = (pcb->file_descriptor)[fd].file_op_table_ptr -> read((pcb->file_descriptor)[fd].inode, buf, nbytes);
        return bytes_read;
    }

    /* FD_FLAG_DIR case */
    else if ((pcb->file_descriptor)[fd].flags == FD_FLAG_DIR)
    {
        bytes_read = (pcb->file_descriptor)[fd].file_op_table_ptr -> read((pcb->file_descriptor)[fd].file_position, buf, nbytes);
        (pcb->file_descriptor)[fd].file_position += (bytes_read == 0 ? 0 : 1);
        return bytes_read;
    }

    /* FD_FLAG_FILE case */
    else if ((pcb->file_descriptor)[fd].flags == FD_FLAG_FILE)
    {
        if (nbytes == 0) { return 0; }
        uint8_t inode = (uint8_t) ((pcb->file_descriptor)[fd].inode);
        ((uint8_t *) buf)[0] = inode;
        bytes_read = (pcb->file_descriptor)[fd].file_op_table_ptr -> read((pcb->file_descriptor)[fd].file_position, buf, nbytes);
        (pcb->file_descriptor)[fd].file_position += bytes_read;
        return bytes_read;
    }

    /* Should never reach here as we do sanity check in open call */
    printf("<!> Invalid or unsupported FDE flag for reading.\n");
    error_sound();
    return -1;
}

/* Function: sys_write
 * Description: system call for write a file
 * and return number of bytes read
 * Inputs: fd - int32_t representing index to file desc array
 *         buf - write data to this buffer
 *         nbytes - bytes to be write
 * Outputs:	return -1 on failed operation
 * Side Effects: none
 */
int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes)
{
    // Sanity Checks
    if (buf == NULL || fd > 7 || fd < 0 || nbytes < 0 || fd == FD_STDIN) /* NULL buffer, out-bound fd, stdin are invalid */
    {
        printf("<!> Invalid function parameter for writing.\n");
        error_sound();
        return -1;
    }
    if ((fd >= 2) && ((pcb->file_descriptor)[fd].flags == FD_FLAG_EMPTY))
    {
        printf("<!> File descriptor is not open for writing.\n");
        error_sound();
        return -1;
    }

    /* Handle STDOUT as a special case as terminal write */
    if (fd == FD_STDOUT) { return terminal_write(fd, buf, nbytes); }

    /* Handle VRTC as a special case, change inode number as VRTC frequency */
    if ((pcb->file_descriptor)[fd].flags == FD_FLAG_RTC)
    {
        int rtc_check_return = rtc_write(fd, buf, nbytes);
        if (!rtc_check_return)
        {
            (pcb->file_descriptor)[fd].inode = *((int*) buf);
        }
        return rtc_check_return;
    }

    /* fd write call */
    return (pcb->file_descriptor)[fd].file_op_table_ptr -> write(fd, buf, nbytes);
}

/* Function: sys_getargs
 * Description: Get the trimmed argument after the command name.
 * Inputs: buf - buffer, nbytes - buffer size
 * Outputs: 0 - success, -1 - failed
 * Side Effects: argument will be load to the buffer
 */
int32_t sys_getargs (uint8_t* buf, int32_t nbytes)
{
    // Check the buffer and length
    if (pcb->arg_len == 0)
    {
        printf("<!> Not specified any argument for current program.\n");
        error_sound();
        return -1;
    }
    else if ((pcb->arg_len + 1) > nbytes)
    {
        printf("<!> Insufficient buffer space to get arguments.\n");
        error_sound();
        return -1;
    }

    // Copy the argument to buffer
    memcpy(buf, pcb->arg_buffer, pcb->arg_len);
    buf[pcb->arg_len] = '\0';
    return 0;
}

/* Function: sys_vidmap
 * Description: Map the video memory to 0x8400000 to 0x8401000
 * Inputs: screen_start - place to store the mapped video memory pointer
 * Outputs: 0 - success, -1 - failed
 * Side Effects: video memory will be mapped to 0x8400000 to 0x8401000
 */
int32_t sys_vidmap (uint8_t** screen_start)
{
    // Sanity check
    if (screen_start == NULL || (uint32_t) screen_start < PROGRAM_PAGE_ADDR || (uint32_t) screen_start > (PROGRAM_STACK_ADDR - 4))
    {
        printf("<!> Specified vidmap address 0x%#x is not valid.\n", (uint32_t) screen_start);
        error_sound();
        return -1;
    }

    // Modify TI
    terminals[pcb->terminal_id].vidmap = 1;

    // Call the mapper function accordingly
    if (pcb->terminal_id == terminal_active)
    {
        map4KBVidMemPage();
    }
    
    
    // Pass the pointer
    *screen_start = (uint8_t *) PROGRAM_STACK_ADDR;
    
    return 0;
}

/* Function: sys_set_handler
 * Description: Set new handler when a signal is received
 * Inputs: signum - signal#, handler_address - user space handler pointer
 * Outputs: -1 - failed, 0 - success
 * Side Effects: none
 */
int32_t sys_set_handler (int32_t signum, void* handler_address)
{
    if (signum > 4)
    {
        printf("<!> Invalid signum %u specified to set new handler.\n", signum);
        return -1;
    }
    pcb->sig_handlers[signum] = handler_address;
    return 0;
}

/* Function: sys_sigreturn
 * Description: Return from user space signal handler
 * Inputs: none
 * Outputs: Previous EAX from ASM linkage call
 * Side Effects: Context switch back to partent ASM linkage call
 */
int32_t sys_sigreturn (void)
{
    // Enable sys protection
    pageDir[1].US = 0;
    flushTLB();

    // Restore context
    uint32_t ebp = pcb->sig_ebp;
    uint32_t esp = pcb->sig_esp;
    asm volatile (
        "movl %0, %%ebp  \n"
        "movl %1, %%esp  \n"
        :
        : "r"(ebp), "r"(esp)
    );

    // Restore kernel stack
    memcpy((void*) (pcb->sig_ebp), (void*) &(pcb->sig_stackshot), pcb->sig_stacksize);
    
    // Unmask the interrupt line
    pcb->sig_mask = 0;

    // Return backup EAX
    return pcb->sig_eax;
}

/* Function: sys_invalid
 * Description: print out # for invalid syscall
 * Inputs: callnum - syscall #
 * Outputs: always return -1
 * Side Effects: print out the error message on screen
 */
int32_t sys_invalid(unsigned int callnum)
{
    printf("<!> System call #%u is not valid.\n", callnum);
    error_sound();
    return -1;
}

/* Function: find_next_pid
 * Description: find next available pid in pcb pool
 * Inputs: none
 * Outputs: -1 - pcb pool full, int - next available pid
 * Side Effects: none
 */
int find_next_pid()
{
    int i;
    for (i = 0; i < MAX_PID_COUNT; i++)
    {
        if (pcb_pool[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

/* Function: play_sound
 * Description: play sound at specific frequency and duration
 * Inputs: frequency - sound frequency
 *         duratioin - sound play time
 * Outputs: None
 * Side Effects: none
 * Reference: OSdev
 */
void play_sound (int frequency, uint32_t play_duration){
    //Calculate frequency
    uint32_t Div = 1193180 / frequency;
    //Select PIT channel 2
    outb(0xb6, 0x43);
    //Set PIT channel 2 to desired frequency
	outb((uint8_t)(Div & 0xff), 0x42);
	outb((uint8_t)((Div >> 8)& 0xff), 0x42);
    //play sound
	uint8_t tmp = inb(0x61);
	if (tmp != (tmp | 3)){
		outb(tmp | 3, 0x61);
	}
    
    //This loop is just for sound to play a short period of time. 
    //Not accurate on time. 
	int32_t i = play_duration * play_duration;
    while (i>0){
        i--;
    }
    // PC speaker stop
	uint8_t temp = inb(0x61) & 0xFC;
	outb(temp, 0x61);
}

/* Function: error_sound
 * Description: play error sound message
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 */
void error_sound(){
    //piano key C#4
    play_sound(262, 9000); 
}

/* Function: OS_start_sound
 * Description: play system start sound
 * Inputs: None
 * Outputs: None
 * Side Effects: none
 */
void OS_start_sound(){
    // C4
    play_sound(262, 8500);
    // F4
    play_sound(349, 8500);
    // C4
    play_sound(262, 8500);
    // A4
    play_sound(440, 8500);
    // F4
    play_sound(349, 8500);
    // C4
    play_sound(523, 13000);
}
