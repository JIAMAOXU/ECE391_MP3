// header file for file_system.c

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

// File Types
#define FILE_TYPE_RTC 0
#define FILE_TYPE_DIR 1
#define FILE_TYPE_FILE 2

#ifndef ASM

#include "lib.h"
#include "multiboot.h"

// Data structure for dentry
typedef struct dentry_t
{
    uint8_t file_name[32];
    uint32_t file_type;
    uint32_t inode_number;
    uint8_t reserved[24];
} dentry_t;

/* basic init function to pass in address to start of filesystem */
int32_t file_sys_init(uint32_t file_start_addr);

/* read a specific file */
int32_t file_read (int32_t fd, void* buf, int32_t nbytes);

/* write to file, unused */
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes);

/* open file */
int32_t file_open (const uint8_t* filename);

/* close file */
int32_t file_close (int32_t fd);

/* directory open */
int dir_open(const uint8_t* filename);

/* directory close */
int dir_close(int32_t fd);

/* directory write */
int dir_write(int32_t fd, const void* buf, int32_t nbytes);

/* read individual file name in directory */
int dir_read(int32_t fd, void* buf, int32_t nbytes);

int dir_read2(int32_t fd, void* buf, int32_t nbytes);

/* returns file name to dentry block struct if file is found */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);

/* returns file inode/index to block struct if file is found */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* returns file size of input file string */
int32_t return_file_size(const uint8_t* file_name);

/* returns file size of input fd */
int32_t return_file_size_fd(int32_t fd);

/* returns file type of input inode number */
int32_t return_file_type_fd(uint32_t inode);

/* checks if file is an executable */
int32_t check_executable(const uint8_t* file_name);

#endif /* ASM */
#endif /* _FILESYSTEM_H */
