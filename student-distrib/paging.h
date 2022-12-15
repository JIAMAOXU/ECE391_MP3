// header file for paging.c
#ifndef PAGING_H
#define PAGING_H

#include "x86_desc.h"

#define PAGE_DIR_SIZE 1024
#define PAGE_TABLE_SIZE 1024

#define VIDEO_MEM_PAGE 0xB8
#define VIDEO_BACKUP_PAGE0 0xB9
#define VIDEO_BACKUP_PAGE1 0xBA
#define VIDEO_BACKUP_PAGE2 0xBB
#define VIDEO_BACKUP_PAGE_EXTRA 0xBC

#ifndef ASM

/* individual struct for page directory aligned 4kb */
typedef struct __attribute__((packed)) pageDir_t 
{
    uint8_t P : 1;                              /* Present, means page is in physical memory */
    uint8_t RW : 1;                             /* Read/Write */
    uint8_t US : 1;                             /* If set to 1, anyone can access page, 0 for only supervise access*/
    uint8_t PWT : 1;                            /* Enable Write to Cache, perma set to 0 */
    uint8_t PCD : 1;                            /* Cache Disable Bit, perma set to 0 */
    uint8_t A : 1;                              /* Accessed */
    uint8_t Z : 1;                              /* Zero bit */
    uint8_t PS : 1;                             /* Page Size, page size of this entry, 4mb, so set bit to 1 */
    uint8_t G : 1;                              /* Global*/
    uint8_t AVL : 3;                            /* AVL bit, unused */
    uint32_t pd_address : 20;                   /* address input bits */
} pageDir_t;

/* individual struct for page table aligned 4kb */
typedef struct __attribute__ ((packed)) pageTable_t
{
    uint8_t P : 1;                              /* flags function the same as above struct */
    uint8_t RW : 1;
    uint8_t US : 1;
    uint8_t PWT : 1;
    uint8_t PCD : 1;
    uint8_t A : 1;
    uint8_t D : 1;
    uint8_t PAT : 1; 
    uint8_t G : 1;
    uint8_t AVL : 3;
    uint32_t physicalAddress : 20;
} pageTable_t;

/* array of page directory entries, aligned to 4kb */
struct pageDir_t pageDir[PAGE_DIR_SIZE]__attribute__((aligned(4096)));   

/* array of page table entries, aligned to 4kb */  
struct pageTable_t pageTableLow[PAGE_TABLE_SIZE]__attribute__((aligned(4096)));
struct pageTable_t pageTableHigh[PAGE_TABLE_SIZE]__attribute__((aligned(4096)));

/* main hub function that calls all other paging helpers */
extern void paging_init();             

/* helper to initialize page directory */
extern void set_page_directory();      

/* helper to initialize page table */
extern void set_page_table(pageTable_t* pageTable);  

/* helper function to create a 4MB page for system call execute processes */
extern void reMap4MBPage(int8_t pid);

/* helper function to map a new 4kb page for program video mem in user level*/
extern void map4KBVidMemPage();

/* helper function to unmap the new 4kb page for program video mem in user level*/
extern void unMap4KBVidMemPage();

/* helper function to flushTLB on a context switch */
extern void flushTLB();

/* takes care of loading page directory + setting enable paging register */
extern void load_page_dir(unsigned int * pageDir);

#endif
#endif

