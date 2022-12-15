// descripter
#include "paging.h"
#include "file_system.h"
#include "x86_desc.h"
#include "lib.h"
#include "types.h"


#define ADDRESS_SHIFT 14
#define KERNAL_PAGE_DIR_ENTRY 0x400000
#define PAGE_DIR_STARTING_ADDRESS 0x000000
#define UNUSED(pid) (void)(pid)

/*  Sources for paging.c, paging.h, paging_asm.S:
*   https://wiki.osdev.org/Paging
*   Lecture Paging Notes
*/

/* void paging_init()
 * Inputs: None
 * Return Value: void
 * Function: Hub function to perform all paging helper
 * calls. Initializes directories, tables + loads
 * page directory and enables paging.
 */
void paging_init()  // enables paging, sets cr3 register
{
    // Initialize PD
    set_page_directory();

    // Initialize PT for vid mem
    set_page_table(pageTableLow);

    // Initialize PT for vidmap
    set_page_table(pageTableHigh);

    // Set an entry in PD for PT for video mem
    pageDir[0].P = 1;
    pageDir[0].pd_address = (((uint32_t)pageTableLow) >> 12);

    // Set an entry in PD for a large kernel page
    pageDir[1].P = 1;
    pageDir[1].US = 0;
    pageDir[1].PS = 1;
    pageDir[1].pd_address = 1 << 10;        /* size 0x00400, bottom 10 bits are reserved top 10 bits used for actual addressing */

    // Set an entry in PD for a large user page, but do not specify address
    pageDir[32].P = 1;
    pageDir[32].US = 1;
    pageDir[32].PS = 1;

    // Set an entry in PD for PT for vidmap, but do not enable it
    pageDir[33].US = 1;
    pageDir[33].pd_address = (((uint32_t)pageTableHigh) >> 12);

    // Set entries in PT for vid mem
    // Direct Map to VRAM
    pageTableLow[0xB8].P = 1;
    pageTableLow[0xB8].physicalAddress = VIDEO_MEM_PAGE;

    // Backup spaces for multiterminal
    pageTableLow[0xB9].P = 1;
    pageTableLow[0xB9].physicalAddress = VIDEO_BACKUP_PAGE0;

    pageTableLow[0xBA].P = 1;
    pageTableLow[0xBA].physicalAddress = VIDEO_BACKUP_PAGE1;

    pageTableLow[0xBB].P = 1;
    pageTableLow[0xBB].physicalAddress = VIDEO_BACKUP_PAGE2;

    // Extra backup space for kernel
    pageTableLow[0xBC].P = 1;
    pageTableLow[0xBC].physicalAddress = VIDEO_BACKUP_PAGE_EXTRA;

    // Set an entry in PT for vidmap
    pageTableHigh[0].US = 1;
    pageTableHigh[0].P = 1;
    pageTableHigh[0].physicalAddress = VIDEO_MEM_PAGE;

    // Enable paging
    load_page_dir((unsigned int*)pageDir);
}

/* void set_page_directory()
 * Inputs: None
 * Return Value: void
 * Function: Initializes all entries in page
 * directory to default address and flags.
 * Sets 4kb video mem entry + 4mb kernel mem entry
 */
void set_page_directory()
{
    /* variable definitions */
    int i;

    /* initialize page directory, initialize to 4kb alignment by default */
    for (i = 0; i < PAGE_DIR_SIZE; i++)
    {
        pageDir[i].P = 0;
        pageDir[i].RW = 1;
        pageDir[i].US = 1;
        pageDir[i].PWT = 0;
        pageDir[i].PCD = 0; 
        pageDir[i].A = 0;
        pageDir[i].Z = 0;
        pageDir[i].PS = 0;
        pageDir[i].G = 0;           /* set for kernal page only, not here basically */
        pageDir[i].AVL = 0;
        pageDir[i].pd_address = 0x00000000;
    }
}

/* void set_page_table()
 * Inputs: None
 * Return Value: void
 * Function: Initializes all entries in page
 * table. Links video memory virtual address
 * to video memory physical address.
 */
void set_page_table(pageTable_t* pageTable)
{
    /* variable definitions */
    int i;

    /* initialize page tables, set default flags */
    for (i = 0; i < PAGE_TABLE_SIZE; i++)
    {
        pageTable[i].P = 0;
        pageTable[i].RW = 1;
        pageTable[i].US = 0;
        pageTable[i].PWT = 0;
        pageTable[i].PCD = 0;
        pageTable[i].A = 0;
        pageTable[i].D = 0;
        pageTable[i].PAT = 0;
        pageTable[i].G = 0;
        pageTable[i].AVL = 0;
        pageTable[i].physicalAddress = 0x00000000;
    }
}

/* void reMap4MBPage()
 * Inputs: int8_t pid
 * Return Value: none
 * Function: Initializes all a page for user program
 */
void reMap4MBPage(int8_t pid)
{
    pageDir[32].pd_address = (0x800000 + pid * 0x400000) >> 10;
    flushTLB();
}

/* void map4KBVidMemPage()
 * Inputs: none
 * Return Value: none
 * Function: helper function to map a new 4kb page for program video mem in user level
 */
void map4KBVidMemPage()
{
    pageDir[33].P = 1;
    flushTLB();
}

/* void unMap4KBVidMemPage()
 * Inputs: none
 * Return Value: none
 * Function: helper function to unmap the new 4kb page for program video mem in user level
 */
void unMap4KBVidMemPage()
{
    pageDir[33].P = 0;
    flushTLB();
}

/* void set_page_table()
 * Inputs: None
 * Return Value: void
 * Function: Flushes TLB :)
 */
void flushTLB()
{
    asm volatile(
        "movl %%cr3, %%eax    \n"
        "movl %%eax, %%cr3    \n"
        :
        : 
        : "memory", "cc", "%eax"
    );
    return;
}


