#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "terminal.h"
#include "paging.h"
#include "file_system.h"
#include "syscalls.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test 1
 * 
 * Asserts that first 14 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S, idt.h/c
 */
int idt_test_1()
{
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 14; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* IDT Test 2
 * 
 * Asserts 16-19 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S, idt.h/c
 */
int idt_test_2()
{
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 16; i < 20; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Divide by 0 Test - Exception
 * 
 * 
 * Inputs: None
 * Outputs: Should trigger an exception
 * Side Effects: None
 * Coverage: Paging functionality
 * Files: paging.c, paging.h, paging_asm.S
 */
int div_0_test()
{
	TEST_HEADER;
	//int a = 20 / 0 + 1;
	return FAIL;
}

/* Page Fault Test - Null ptr exception
 * 
 * 
 * Inputs: None
 * Outputs: Should trigger an exception
 * Side Effects: None
 * Coverage: Paging functionality
 * Files: paging.c, paging.h, paging_asm.S
 */
int def_page_fault_test()
{
	TEST_HEADER;
	int* a = NULL;
	printf("Trying to dereference a NULL pointder: %d\n", *a);
	return FAIL;
}

/* Page Fault Test - Exception outside video mem
 * 
 * 
 * Inputs: None
 * Outputs: Should trigger an exception since the address is outside the paged area
 * Side Effects: None
 * Coverage: Paging functionality
 * Files: paging.c, paging.h, paging_asm.S
 */
int page_fault_outside_video_mem()
{
	TEST_HEADER;
	int* tmpPtr;
	tmpPtr = (int*)0xB7000;	/* test one memory address under video mem */
	printf("Trying to dereference a pointer outside of video memory: %d\n", *tmpPtr);
	return FAIL;
}

/* Page Fault Test - Check valid address inside video mem
 * 
 * 
 * Inputs: None
 * Outputs: Should PASS since the address is valid
 * Side Effects: None
 * Coverage: Paging functionality
 * Files: paging.c, paging.h, paging_asm.S
 */
int page_fault_inside_video_mem()
{
	TEST_HEADER;
	int* tmpPtr;
	tmpPtr = (int*)(0xB8000 + 1); /* test in video memory*/
	printf("Trying to dereference a pointer inside of video memory: %d\n", *tmpPtr);
	return PASS;
}

/* Page Fault Test - Exception between video mem and kernel mem
 * 
 * 
 * Inputs: None
 * Outputs: Should trigger an exception since the address is outside the paged area
 * Side Effects: None
 * Coverage: Paging functionality
 * Files: paging.c, paging.h, paging_asm.S
 */
int page_fault_between_vid_and_kernel_mem()
{
	TEST_HEADER;
	int* tmpPtr;
	tmpPtr = (int*)(0x400000 - 1); /* test one memory address below kernal mem */
	printf("Trying to dereference a pointer outsdie of kernel memory: %d\n", *tmpPtr);
	return FAIL;
}

/* Page Fault Test - Check valid address inside kernel mem
 * 
 * 
 * Inputs: None
 * Outputs: Should PASS since the address is valid
 * Side Effects: None
 * Coverage: Paging functionality
 * Files: paging.c, paging.h, paging_asm.S
 */
int page_fault_inside_kernel_mem()
{
	TEST_HEADER;
	int* tmpPtr;
	tmpPtr = (int*)(0x400000 + 1);
	printf("Trying to derefence a pointer inside kernel memory: %d\n", *tmpPtr);
	return PASS;
}

/* Page Fault Test - Exception outside kernel mem
 * 
 * 
 * Inputs: None
 * Outputs: Should trigger an exception since the address is outside the paged area
 * Side Effects: None
 * Coverage: Paging functionality
 * Files: paging.c, paging.h, paging_asm.S
 */
int page_fault_outside_kernel_mem()
{
	TEST_HEADER;
	int* tmpPtr;
	tmpPtr = (int*)0x800001;
	printf("Trying to derefence a pointer outside kernel memory: %d\n", *tmpPtr);
	return FAIL;
}

/* Checkpoint 2 tests */

/* Directory Read Test - mimics ls command (by index)
 * 
 * 
 * Inputs: None
 * Outputs: Should print all known files to the screen ls style
 * Side Effects: None
 * Coverage: Directory reading functionality + read_dentry_by_index helper func
 * Files: file_system.h, file_system.c
 */
int directory_read()
{
	clear();
	uint8_t buf[4096];
	int i, fileCounter;
	fileCounter = 0;

	for (i = 0; i < 63; i++)
	{
		dir_read(0, buf, i);
	}
	return PASS;
}

/* Directory Read Test - mimics ls command (by name) <--- USE THIS ONE
 * 
 * 
 * Inputs: None
 * Outputs: Should print all known files to the screen ls style
 * Side Effects: None
 * Coverage: Directory reading functionality + read_dentry_by_index helper func
 * Files: file_system.h, file_system.c
 */
int directory_read_test()
{
	clear();
	uint8_t buf[4096];

	dir_open((uint8_t *)".");
	dir_read2(0, buf, 64);					/* dummy values for nbytes and fd since they are usused */
	dir_open((uint8_t *)"sigtest");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"shell");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"grep");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"syserr");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"rtc");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"fish");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"counter");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"pingpong");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"cat");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"frame0.txt");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"verylargetextwithverylongname.tx");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"ls");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"testprint");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"created.txt");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"frame1.txt");
	dir_read2(0, buf, 64);
	dir_open((uint8_t *)"hello");
	dir_read2(0, buf, 64);
	
	return PASS;
}

/* Read frame0.txt Test - Reads and Displays File Contents
 * 
 * 
 * Inputs: None
 * Outputs: Should display file contents to screen
 * Side Effects: None
 * Coverage: File read functionality + length sanity check edge case 
 * + read_dentry_by_name func + read_data func
 * Files: file_system.h, file_system.c
 */
int read_frame0_txt()
{
	int i;
	dentry_t currFile;
	uint8_t buf[187];															/* buffer created to support size of frame0.txt*/

	if((read_dentry_by_name((uint8_t *)("frame0.txt"), &currFile)) == -1)		/* cast string to uint8_t ptr*/
	{
		printf("No file with matching name found in file system.\n");
		return FAIL;
	}
	
	read_data(currFile.inode_number, 0, (uint8_t *)buf, 4096);
	clear();
	for (i = 0; i < 187; i++)
	{
		putc(buf[i]);
	}
	printf("\n");
	return PASS;
}

/* Read Very Large Text File Test - Reads and Displays File Contents
 * 
 * 
 * Inputs: None
 * Outputs: Should return FAIL since input text file does not meet file_name length reqs
 * Side Effects: None
 * Coverage: file_name length sanity checks, edge case
 * Files: file_system.h, file_system.c
 */
int file_name_char_limit()
{
	int i;
	dentry_t currFile;
	uint8_t buf[5277]; /* 5277 buffer size to meet file length requirements */
	if((read_dentry_by_name((uint8_t *)("verylargetextwithverylongname.txt"), &currFile)) == -1)		/* cast string to uint8_t ptr*/
	{
		printf("No file with matching name found in file system.\n");
		return FAIL;
	}

	read_data(currFile.inode_number, 0, (uint8_t *)buf, 5277); /* 5277 buffer size to meet file length requirements */

	clear();
	for (i = 0; i < 5277; i++) /* 5277 buffer size to meet file length requirements */
	{
		putc(buf[i]);
	}
	printf("\n");
	return PASS;
}

/* Read Very Large Text File Test - Reads and Displays File Contents
 * 
 * 
 * Inputs: None
 * Outputs: Should by pass edge case file_name length test and show multi block reading
 * Side Effects: None
 * Coverage: File read functionality + read_dentry_by_name func + read_data func
 * Files: file_system.h, file_system.c
 */
int read_verylarge_txt_pass()
{
	int i;
	dentry_t currFile;
	uint8_t buf[5277]; /* 5277 buffer size to meet file length requirements */
	if((read_dentry_by_name((uint8_t *)("verylargetextwithverylongname.tx"), &currFile)) == -1)		/* cast string to uint8_t ptr*/
	{
		printf("No file with matching name found in file system.\n");
		return FAIL;
	}

	read_data(currFile.inode_number, 0, (uint8_t *)buf, 5277); /* 5277 buffer size to meet file length requirements */

	clear();
	for (i = 0; i < 5277; i++) /* 5277 buffer size to meet file length requirements */
	{
		putc(buf[i]);
	}
	printf("\n");
	return PASS;
}

/* Read Cat File - Reads and Displays File Contents
 * 
 * 
 * Inputs: None
 * Outputs: Should display "ELF" starting characters followed by gibberish
 * and ends with magic number string "0123456789ABCDEFGHIJKLMNOPQRSTU"
 * Side Effects: None
 * Coverage: File read functionality (non text file)
 * Files: file_system.h, file_system.c
 */
int read_cat()
{
	int i;
	dentry_t currFile;
	uint8_t buf[5445]; /* 5445 buffer size to meet file length requirements */
	if((read_dentry_by_name((uint8_t *)("cat"), &currFile)) == -1)		/* cast string to uint8_t ptr*/
	{
		printf("No file with matching name found in file system.\n");
		return FAIL;
	}
	
	read_data(currFile.inode_number, 0, (uint8_t *)buf, 5445); /* 5445 buffer size to meet file length requirements */
	clear();
	
	for (i = 0; i < 500; i++) /* since file is super large and will fill entire screen only read first 500 and last 100 bytes */
	{							/* for proving magic string and ELF functionality */
		putc(buf[i]);
	}
	printf("\n");
	for (i = 5200; i < 5445; i++)
	{
		putc(buf[i]);
	}
	printf("\n");
	return PASS;
}

/* RTC Test
 * 
 * 
 * Inputs: None
 * Outputs: Should print * on screen with varying RTC frequency.
 *          Should print message when input frequency is not valid.
 * Side Effects: None
 * Coverage: RTC Driver
 * Files: rtc.c, rtc.h
 */
int rtc_test()
{
	TEST_HEADER;

	int freq;
	int i;
	int ret_success = 0; // RET value to check the driver return
	int ret_fail = 5;

	printf("Trying to print * with 2 Hz:\n");
	freq = 2;
	ret_success += rtc_write(NULL, &freq, NULL);
	for (i = 0; i < 10; i++)
	{
		ret_success += rtc_read(NULL, NULL, NULL);
		putc('*');
	}
	
	printf("\nTrying to print * with 16 Hz:\n");
	freq = 16;
	ret_success += rtc_write(NULL, &freq, NULL);
	for (i = 0; i < 50; i++)
	{
		ret_success += rtc_read(NULL, NULL, NULL);
		putc('*');
	}

	printf("\nTrying to print * with 128 Hz:\n");
	freq = 128;
	ret_success += rtc_write(NULL, &freq, NULL);
	for (i = 0; i < 200; i++)
	{
		ret_success += rtc_read(NULL, NULL, NULL);
		putc('*');
	}

	printf("\nTrying to print * with 1024 Hz:\n");
	freq = 1024;
	ret_success += rtc_write(NULL, &freq, NULL);
	for (i = 0; i < 600; i++)
	{
		ret_success += rtc_read(NULL, NULL, NULL);
		putc('*');
	}
	printf("\nTrying to set RTC to a negative frequency...\n");
	freq = -512;
	ret_fail += rtc_write(NULL, &freq, NULL);

	printf("Trying to set RTC to an extreme small frequency...\n");
	freq = 1;
	ret_fail += rtc_write(NULL, &freq, NULL);

	printf("Trying to set RTC to an extreme large frequency...\n");
	freq = 8192;
	ret_fail += rtc_write(NULL, &freq, NULL);

	printf("Trying to set RTC to an invalid frequency...\n");
	freq = 666;
	ret_fail += rtc_write(NULL, &freq, NULL);
	
	printf("Trying to set RTC to an NULL frequency...\n");
	ret_fail += rtc_write(NULL, NULL, NULL);

	printf("Restoring RTC frequency...\n");
	freq = 2;
	ret_success += rtc_write(NULL, &freq, NULL);
	return (ret_success + ret_fail) ? FAIL : PASS;
}

/* Terminal Null Test
 * 
 * 
 * Inputs: None
 * Outputs: Try to trigger various edge cases.
 *          Should not crash the kernel and print error
 *          message accordingly.
 * Side Effects: none
 * Coverage: Terminal Driver, Keyboard Driver
 * Files: terminal.c, terminal.h, keyboard.c, keyboard.h
 */
int terminal_null_test()
{
	TEST_HEADER;
	int ret_zero = 0; // RET value to check the driver return
	int ret_small = 0;
	int ret_fail = 3;
	unsigned char buf[KEYBOARD_BUFFER_SIZE];
	printf("Try to read from the terminal with NULL buffer...\n", sizeof(buf));
	ret_fail += terminal_read(NULL, NULL, KEYBOARD_BUFFER_SIZE);
	printf("Try to read from the terminal with extreme large byte count...\n", sizeof(buf));
	ret_fail += terminal_read(NULL, buf, 9999);
	printf("Try to read from the terminal with zero byte count...\n", sizeof(buf));
	ret_zero = terminal_read(NULL, buf, 0);
	printf("Try to write to the terminal with the previous buffer (should print nothing)...\n", sizeof(buf));
	if (terminal_write(NULL, buf, ret_zero))
	{
		return FAIL;
	}
	printf("\nTry to read from the terminal with a small byte count (please type more then 5 chars)...\n", sizeof(buf));
	ret_small = terminal_read(NULL, buf, 5);
	printf("Try to write to the terminal with the previous buffer (should print 5 chars only)...\n", sizeof(buf));
	if (terminal_write(NULL, buf, ret_small) != 5)
	{
		return FAIL;
	}
	printf("\nTry to write to the terminal with NULL buffer...\n", sizeof(buf));
	ret_fail += terminal_write(NULL, NULL, KEYBOARD_BUFFER_SIZE);
	printf("Try to write to the terminal with zero byte count (should print nothing)...\n", sizeof(buf));
	if (terminal_write(NULL, buf, ret_zero))
	{
		return FAIL;
	}
	return ret_fail ? FAIL : PASS;
}

/* Terminal Infiniate Test
 * 
 * 
 * Inputs: None
 * Outputs: Read and write constantly to the screen,
 *          utilizing Terminal syscalls.
 * Side Effects: Will print the received keyboard buffer.
 * Coverage: Terminal Driver, Keyboard Driver
 * Files: terminal.c, terminal.h, keyboard.c, keyboard.h
 */
int terminal_inf_test()
{
	TEST_HEADER;
	int ret;
	unsigned char buf[KEYBOARD_BUFFER_SIZE];
	while (1)
	{
		printf("Reading from the terminal...\n", sizeof(buf));
		ret = terminal_read(NULL, buf, KEYBOARD_BUFFER_SIZE);
		if (ret == -1)
		{
			break;
		}
		printf("Writing the buffer with size %d...\n", ret);
		if (terminal_write(NULL, buf, ret) != ret)
		{
			putc('\n');
			break;
		}
		printf("\nDone and return value is valid.\n");
	}
	return FAIL;
}

/* Checkpoint 3 tests */

/* Return File Size Test
 * 
 * 
 * Inputs: None
 * Outputs: Returns size of a specific file to user in bytes
 * Side Effects: None
 * Coverage: Filesystems, Execute Helper
 * Files: file_system.h, file_system.c
 */
int file_size_test()
{
	/* tests file size for ls */
	uint32_t file_size1;

	file_size1 = return_file_size((uint8_t *)"ls");
	printf("Testing file size test functionality.\n");
	if (file_size1 == -1)
	{
		printf("File not found.");
		return FAIL;
	}
	printf("ls file size: %d\n", file_size1);
	return PASS;
}

/* Return File Size Test Fail
 * 
 * 
 * Inputs: None
 * Outputs: Should return PASS if the requested
 * file is not found. (a fake file is sent in)
 * Side Effects: None
 * Coverage: Filesystems, Execute Helper
 * Files: file_system.h, file_system.c
 */
int file_size_test_fail()
{
	uint32_t file_size1;
	file_size1 = return_file_size((uint8_t *)"lst");
	if (file_size1 == -1)
	{
		printf("Requested file not found.\n");
		return PASS;
	}
	return FAIL;
}


/* Check if File is an Executable Test
 * 
 * 
 * Inputs: None
 * Outputs: Returns 1 if file is an exec
 * and 0 otherwise
 * Side Effects: None
 * Coverage: Filesystems, Execute Helper
 * Files: file_system.h, file_system.c
 */
int check_file_executable_test()
{
	if(check_executable((uint8_t *)("cat")) == 1)
		return PASS;
	else
		return FAIL;
}

/* Paging test for creating a new 4MB page
 * 
 * 
 * Inputs: None
 * Outputs: Returns PASS if no page faults
 * Side Effects: None
 * Coverage: Paging, Execute Helper
 * Files: paging.c, paging.h
 */
/*
int paging_create_4MB_page_test()
{
	int8_t pid = 0;
	int* tmpPtr;
	create4MBPage(pid);
	tmpPtr = (int*)(0x8000000 + 1);
	printf("Trying to deref pointer inside lower bound process block 1: %d\n", *tmpPtr);
	tmpPtr = (int*)(0x8400000 - 5); 
	printf("Trying to deref pointer inside upper bound process block 1: %d\n", *tmpPtr);
	return PASS;
}
Not usable anymore after multiterminal modification. --PL */

/* Paging test for creating a new 4MB page (page fault creation)
 * 
 * 
 * Inputs: None
 * Outputs: Should page fault as you are accessing page outside of proper memory
 * Side Effects: None
 * Coverage: Paging, Execute Helper
 * Files: paging.c, paging.h
 */
/*
int paging_create_4MB_fault_below_page()
{
	int8_t pid = 0;
	int* tmpPtr;
	create4MBPage(pid);
	tmpPtr = (int*)(0x8000000 - 1);
	printf("Trying to deref pointer outside lower bound process block 1: %d\n", *tmpPtr);
	return FAIL;
}
Not usable anymore after multiterminal modification. --PL */

/* Paging test for creating a new 4MB page (page fault creation)
 * 
 * 
 * Inputs: None
 * Outputs: Should page fault as you are accessing page outside of proper memory
 * Side Effects: None
 * Coverage: Paging, Execute Helper
 * Files: paging.c, paging.h
 */
/*
int paging_create_4MB_fault_above_page()
{
	int8_t pid = 0;
	int* tmpPtr;
	create4MBPage(pid);
	tmpPtr = (int*)(0x8400000 + 1);
	printf("Trying to deref pointer outside upper bound process block 1: %d\n", *tmpPtr);
	return FAIL;
}
Not usable anymore after multiterminal modification. --PL */

/* Test for sys_open
 * Open all types of files into fd_array, uses self validation from user
 * to display individual data from each sys_open call
 * 
 * Inputs: None
 * Outputs: Should return PASS if all tests complete
 * Side Effects: None
 * Coverage: Execute Helper, System Call
 * Files: syscalls.c, syscalls.h
 */
/*
int sys_open_test()
{
	file_desc_array_init();
	printf("Testing rtc sys file call\n");
	sys_open((uint8_t *)("rtc"));
	printf("RTC file descriptor info:\n Inode: %d\n File Position: %d\n Present: %d\n", fd_array[2].inode, fd_array[2].file_position, fd_array[2].flags);
	printf("Testing directory sys file call\n");
	sys_open((uint8_t*)("."));
	printf("Directory file descriptor info:\n Inode: %d\n File Position: %d\n Present: %d\n", fd_array[3].inode, fd_array[3].file_position, fd_array[3].flags);
	printf("Testing normal sys file call\n");
	sys_open((uint8_t *)("cat"));
	printf("Directory file descriptor info:\n Inode: %d\n File Position: %d\n Present: %d\n", fd_array[4].inode, fd_array[4].file_position, fd_array[4].flags);
	return PASS;
}
Not usable anymore after multiterminal modification. --PL */

/* Test for sys_open overflow mechanic
 * 
 * Inputs: None
 * Outputs: Should return PASS if all tests complete
 * Side Effects: None
 * Coverage: Execute Helper, System Call
 * Files: syscalls.c, syscalls.h
 */
int sys_open_test_overflow()
{
	sys_open((uint8_t *)("rtc"));
	sys_open((uint8_t *)("."));
	sys_open((uint8_t *)("cat"));
	sys_open((uint8_t *)("ls"));
	sys_open((uint8_t *)("shell"));
	sys_open((uint8_t *)("grep"));
	/* file descriptor array should be maxed out by here*/
	printf("Next sys open call attempt should fail.\n");
	sys_open((uint8_t *)("hello"));
	return PASS;
}

/* Test for sys_close
 * Open all types of files into fd_array, uses self validation from user
 * to display individual data from each sys_close call
 * 
 * Inputs: None
 * Outputs: Should return PASS if all tests complete
 * Side Effects: None
 * Coverage: Execute Helper, System Call
 * Files: syscalls.c, syscalls.h
 */
int sys_close_test()
{
	printf("FD array indicies 2, 3, and 4 will be populated.\n");
	sys_open((uint8_t *)("rtc"));
	sys_open((uint8_t *)("."));
	sys_open((uint8_t *)("cat"));

	printf("Normal sys close call...\n");
	sys_close(3);
	printf("Sys close accessing an already empty fd...\n");
	sys_close(6);
	printf("Sys close using an invalid fd index...\n");
	sys_close(1);
	return PASS;
}

/* Test for file_size_fd helper function
 * Validates that file size can be returned properly from a given fd input
 * 
 * Inputs: None
 * Outputs: Should return PASS if all tests complete
 * Side Effects: None
 * Coverage: Execute Helper, System Call, Filesystems
 * Files: syscalls.c, syscalls.h, filesystems.c, filesystems.h
 */
/*
int file_size_fd_test()
{
	file_desc_array_init();
	printf("File cat will be stored at fd index 2.\n");
	sys_open((uint8_t *)("cat"));
	printf("File size of cat is: %d\n", return_file_size_fd(2));
	return PASS;
}
Not usable anymore after multiterminal modification. --PL */

/* Checkpoint 4 tests */

/* Paging test for mapping program video memory
 * 
 * 
 * Inputs: None
 * Outputs: Returns PASS if no page faults (self validation)
 * Side Effects: None
 * Coverage: Paging, Execute Helper
 * Files: paging.c, paging.h
 */
int paging_map_vid_mem_test()
{
	uint8_t* dummyParam;
	int* tmpPtr;
	map4KBVidMemPage(dummyParam);
	tmpPtr = (int*)(0x84b8000 + 1);
	printf("Trying to deref a pointer inside lower bound of video memory: %d\n", *tmpPtr);
	tmpPtr = (int *)(0x84b9000 - 5);
	printf("Trying to deref a pointer inside upper bound of video memory: %d\n", *tmpPtr);
	return PASS;
}

/* Paging test testing page fault outside of vid mem (below)
 * 
 * 
 * Inputs: None
 * Outputs: Returns FAIL if no page faults (self validation)
 * Side Effects: None
 * Coverage: Paging, Execute Helper
 * Files: paging.c, paging.h
 */
int paging_map_vid_mem_below_pagefault()
{
	uint8_t* dummyParam;
	int* tmpPtr;
	map4KBVidMemPage(dummyParam);
	tmpPtr = (int*)(0x84b8000 - 1);
	printf("Trying to deref a pointer inside lower bound of video memory: %d\n", *tmpPtr);
	return FAIL;
}

/* Paging test testing page fault outside of vid mem (above)
 * 
 * 
 * Inputs: None
 * Outputs: Returns FAIL if no page faults (self validation)
 * Side Effects: None
 * Coverage: Paging, Execute Helper
 * Files: paging.c, paging.h
 */
int paging_map_vid_mem_above_pagefault()
{
	uint8_t* dummyParam;
	int* tmpPtr;
	map4KBVidMemPage(dummyParam);
	tmpPtr = (int*)(0x84b9000 + 1);
	printf("Trying to deref a pointer inside lower bound of video memory: %d\n", *tmpPtr);
	return FAIL;
}

/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests()
{
	// TEST_OUTPUT("IDT Test 1", idt_test_1());
	// TEST_OUTPUT("IDT Test 2", idt_test_2());
	// TEST_OUTPUT("Divide zero test", div_0_test()); // Please uncomment the code in the function..
	// TEST_OUTPUT("Page fault test, dereference null pointer", def_page_fault_test());
	// TEST_OUTPUT("Page fault test, inside vid memory", page_fault_inside_video_mem());
	// TEST_OUTPUT("Page fault test, inside kernel memory", page_fault_inside_kernel_mem());
	// TEST_OUTPUT("Page fault test, outside vid memory", page_fault_outside_video_mem());
	// TEST_OUTPUT("Page fault test, inbetween video and kernel memory", page_fault_between_vid_and_kernel_mem());
	// TEST_OUTPUT("Page fault test, outside kernel memory", page_fault_outside_kernel_mem());
	// TEST_OUTPUT("Directory read test", directory_read()); // This won't work due to conventions -- PL
	// TEST_OUTPUT("Reading frame0.txt test", read_frame0_txt()); // This won't work due to conventions -- PL
	// TEST_OUTPUT("Directory read revised test", directory_read_test()); // This won't work due to conventions -- PL
	// TEST_OUTPUT("Reading from verylargetextwithverylongname.txt", read_verylarge_txt_pass()); // This won't work due to conventions -- PL
	// TEST_OUTPUT("Testing file_name length check", file_name_char_limit()); // This won't work due to conventions -- PL
	// TEST_OUTPUT("Reading cat file", read_cat()); // This won't work due to conventions -- PL
	// TEST_OUTPUT("terminal inf line test", terminal_inf_test());
	// TEST_OUTPUT("terminal null test", terminal_null_test());
	// TEST_OUTPUT("rtc test", rtc_test());
	// TEST_OUTPUT("File size test (ls)", file_size_test());
	// TEST_OUTPUT("File size test fail", file_size_test_fail());
	// TEST_OUTPUT("Check if file is an exectuable", check_file_executable_test());
	// TEST_OUTPUT("Check if creating new process block 4mb page is functioning", paging_create_4MB_page_test());  // This won't work after multiterminal modifications. --PL
	// TEST_OUTPUT("Check VM location outside (below) of intial page block created", paging_create_4MB_fault_below_page());  // This won't work after multiterminal modifications. --PL
	// TEST_OUTPUT("Check VM location outside (above) of intial page block created", paging_create_4MB_fault_above_page());  // This won't work after multiterminal modifications. --PL
	// TEST_OUTPUT("Check all types of input to sys open call", sys_open_test()); // This won't work after multiterminal modifications. --PL
	// TEST_OUTPUT("Check sys open call fd array overflow mechanic", sys_open_test_overflow());
	// TEST_OUTPUT("Check all paths in system close call", sys_close_test());
	// TEST_OUTPUT("File size test (cat) using fd", file_size_fd_test()); // This won't work after multiterminal modifications. --PL
	// TEST_OUTPUT("Map video memory for program use test", paging_map_vid_mem_test());
	// TEST_OUTPUT("Map video mem page fault accessing below page", paging_map_vid_mem_below_pagefault());
	// TEST_OUTPUT("Map video mem page fault accessing above page", paging_map_vid_mem_above_pagefault());
}
