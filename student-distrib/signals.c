/**
 *  signals.c - signals support
 *  Copyright (C) 2022 lenovohpdellasus. All Rights Reserved.
 *  Author: Peizhe Liu
 *  Sources: 
 */

#include "signals.h"

/* Function: sig_set
 * Description: Set a signal to a PCB
 * Inputs: pcb - PCB pointer, sig_num - signal number
 * Outputs: none
 * Side Effects: none
 */
void sig_set(pcb_t* pcb, uint8_t sig_num)
{
    // Check if the signal is masked.
    // Unlike the sepcification, I treat sig_num with an order.
    if ((pcb->sig_pending <= sig_num) || (pcb->sig_mask))
    {
        if (verbose_mode)
        {
            printf("\n<!> Unable to deliver the signal.\n");
        }
        return;
    }

    // Set the signal
    if (verbose_mode)
    {
        printf("\n<i> Program %s on terminal %u received sig_num %u\n", pcb->command, pcb->terminal_id, sig_num);
    }
    pcb->sig_pending = sig_num;
}

/* Function: sig_dispatch
 * Description: Dispatch the current signal to handler in current PCB.
 *              This is called by all linkage return.
 * Inputs: eax - used to preserve eax return code from syscall
 * Outputs: none
 * Side Effects: setup stackframe and dispatch to handler accordingly
 */
uint32_t sig_dispatch(uint32_t eax)
{
    // Called from kernel-user linkage, handle the signal
    // If it is handling, exit
    if (pcb->sig_mask)
    {
        return eax;
    }

    // Get the pending signal
    uint32_t pending = pcb->sig_pending;

    // Check if the program ever went to user space
    if (pcb->user_esp == NULL)
    {
        return eax;
    }

    // Determine if handler is custom
    if ((pending < 5) && (pcb->sig_handlers[pending] != NULL))
    {
        // Load the last user esp
        uint32_t user_esp = pcb->user_esp;

        // No user esp available
        if (user_esp <= KERNEL_STACK_ADDR)
        {
            return eax;
        }

        // Call custom handler
        // Clear the pending signal and mask the line
        printf("\n<i> Calling custom sig_num %u handler 0x%#x for program %s.\n", pending, pcb->sig_handlers[pending], pcb->command);
        pcb->sig_pending = NULLSIG;
        pcb->sig_mask = 1;

        // Save eax
        pcb->sig_eax = eax;

        // Save current context
        uint32_t ebp, esp;
        asm volatile (
            "movl %%ebp, %0  \n"
            "movl %%esp, %1  \n"
            : "=r"(ebp), "=r"(esp)
        );
        pcb->sig_ebp = ebp;
        pcb->sig_esp = esp;

        // Build the stack frame and back to user space
        // Save kernel stack
        pcb->sig_stacksize = pcb->tss_esp - ebp;
        memcpy((void*) &(pcb->sig_stackshot), (void*) ebp, pcb->sig_stacksize);

        // Push signal number and return address
        asm volatile (
            "movl  %1, %%esp  \n"
            "pushl %2         \n"
            "pushl %3         \n"
            "movl  %%esp, %0  \n"
            "movl  %4, %%esp  \n"
            : "=r"(user_esp)
            : "r"(user_esp), "r"(pending), "r"(&sig_linkage), "r"(pcb->sig_esp)
            : "memory", "cc"
        );

        // Push arguments and call IRET
        uint32_t prog_ss = USER_DS;
        uint32_t prog_cs = USER_CS;

        // Switch to user-defined handler
        asm volatile (
            "pushl %0  \n"
            "pushl %1  \n"
            "pushfl    \n"
            "pushl %2  \n"
            "pushl %3  \n"
            :
            : "r"(prog_ss), "r"(user_esp), "r"(prog_cs), "r"(pcb->sig_handlers[pending])
            : "memory", "cc"
        );

        // Disable sys protection
        pageDir[1].US = 1;
        flushTLB();

        asm volatile (
            "iret      \n"
        );
    }
    else
    {
        // Clear the pending signal
        pcb->sig_pending = NULLSIG;
        pcb->sig_mask = 1;

        // Call the default handler
        sig_handle(pending);
    }

    // Unmask the interrupt line
    pcb->sig_mask = 0;
    
    // eax is used to preserve last eax from syscall, not used at here just passthrough
    // This is a dummy data for linkage other than syscall, it is garbage so whatever
    return eax;
}

/* Function: sig_collect_esp
 * Description: Collect user esp
 * Inputs: esp - user stack esp pushed by user's linkage code
 * Outputs: none
 * Side Effects: none
 */
void sig_collect_esp(uint32_t esp)
{
    // Record last user ESP
    if ((esp > KERNEL_STACK_ADDR) && (!(pcb->sig_mask)))
    {
        pcb->user_esp = esp;
    }
}

/* Function: sig_linkage
 * Description: Linkage between user space handler return and sigreturn syscall
 * Inputs: none
 * Outputs: none
 * Side Effects: call sigreturn
 */
void sig_linkage()
{
    // Make sys_sigreturn syscall
    asm volatile (
        "movl $10, %eax  \n"
        "int $0x80       \n"
    );
}

/* Function: sig_handle
 * Description: Call the default signal handler
 * Inputs: sig_num - signal number
 * Outputs: none
 * Side Effects: default action will be executed
 */
void sig_handle(uint8_t sig_num)
{
    // Handle accroding to sig_num
    switch (sig_num)
    {
        case DIV_ZERO:
        case SEGFAULT:
        case INTERRUPT:
        case SYSKILL:
            // Kill the process
            printf("\n<!> Killing the process by default signal handler.\n");
            sys_halt(0);
            break;

        default:
            break;
    }
}
