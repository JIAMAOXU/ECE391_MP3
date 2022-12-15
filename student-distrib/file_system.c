#include "file_system.h"
#include "syscalls.h"

/*  Sources for file_system.h, file_system.c:
*/

#define BOOT_BLOCK_OFFSET 64
#define TOTAL_BLOCK_SIZE 4096

int32_t file_sys_start_addr;
dentry_t open_file_dentry;

/* Function: file_sys_init 
 * Description: set up file system starting address
 * Inputs: 
 * file_start_addr - file system starting address
 * Outputs:	return 0 
 * Side Effects: none
 */
int32_t file_sys_init(uint32_t file_start_addr)
{
    file_sys_start_addr = file_start_addr;
    return 0;
}

/* Function: file_read
 * Description: read file according to fd number and count the number of bytes we read
 * Inputs: 
 * fd - int32_t representing index to file desc array
 * buf - write data to this buffer
 * nbytes - bytes to be write
 * Outputs:	return read data result
 * Side Effects: none
 */
int32_t file_read (int32_t fd, void* buf, int32_t nbytes)
{
    uint32_t inode_index = (uint32_t) ((uint8_t *) buf)[0];
    return read_data(inode_index, fd, (uint8_t *) buf, nbytes);
}

/* Function: file_write
 * Description: set up file system starting address
 * Inputs: None
 * Outputs:	-1 since we are read-only file system
 * Side Effects: none
 */
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}

/* Function: file_open
 * Description: open the file and populate file name to open file dentry
 * Inputs: 
 * filename - file name of the file
 * Outputs:	-1 if no file name found. 0 if open successful
 * Side Effects: none
 */
int32_t file_open (const uint8_t* filename)
{
    if((read_dentry_by_name((uint8_t *)(filename), &open_file_dentry)) == -1)		/* cast string to uint8_t ptr*/
	{
		printf("file_open: No file with matching name found in file system.\n");
		return -1;
	}
    return 0;
}

/* Function: file_close
 * Description: close the file 
 * Inputs: 
 * fd - int32_t representing index to file desc array
 * Outputs: return 0 if successful
 * Side Effects: none
 */
int32_t file_close (int32_t fd)       
{
    /* to be implemented in cp3*/
    return 0;
}

/* Function: dir_open
 * Description: open the dirctory and populate file name to open file dentry
 * Inputs: 
 * filename - file name of the file
 * Outputs:	-1 if no file name found. 0 if open successful
 * Side Effects: none
 */
int dir_open(const uint8_t* filename)
{
    if((read_dentry_by_name((uint8_t *)(filename), &open_file_dentry)) == -1)		/* cast string to uint8_t ptr*/
	{
		printf("dir_open: No file with matching name found in file system.\n");
		return -1;
	}
    return 0;
}

/* Function: dir_close
 * Description: close the directory
 * Inputs: 
 * fd - int32_t representing index to file desc array
 * Outputs:	0 if open successful
 * Side Effects: none
 */
int dir_close(int32_t fd)
{
    /* to be implemented in cp3*/
    return 0;
}

/* Function: dir_write
 * Description: Can not write since we are read-only file system
 * Inputs: 
 * fd - int32_t representing index to file desc array
 * buf - write data to this buffer
 * nbytes - bytes to be write
 * Outputs:	return -1 since we are read-only file system
 * Side Effects: none
 */
int dir_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}

/* Function: dir_read (index)
 * Description: read an individiual file name in a directory
                given a specific dentry index.
                Prints file information to the screen
 * Inputs: 
 * index - specific dentry index in bootblock
 * Outputs:	return 0 if index is found. return -1 if no matching index dentry found
 * Side Effects: none
 */
int dir_read(int32_t fd, void* buf, int32_t nbytes)
{
    dentry_t currFile;

    // Check if FD is completely invalid (negative)
    if (fd < 0)
    {
        printf("dir_read: Incorrect position %d specified to dir_read.\n", fd);
        return -1;
    }

    // If FD is not found by index, read is completed, return 0
    if (read_dentry_by_index((uint32_t)fd, &currFile) == -1) 
    {
        return 0;
    }

    memcpy(buf, currFile.file_name, nbytes);

    return nbytes;
}

/* Function: dir_read2 (by name)
 * Description: read an individiual file name in a directory
                given a specific dentry index.
                Prints file information to the screen
 * Inputs: 
 * index - specific dentry index in bootblock
 * Outputs:	return 0 if index is found. return -1 if no matching index dentry found
 * Side Effects: none
 */
int dir_read2(int32_t fd, void* buf, int32_t nbytes) // use open_file_dentry
{
    int i;
    uint32_t* inode_ptr;
    uint32_t file_size;

    inode_ptr = (uint32_t *)(file_sys_start_addr + ((open_file_dentry.inode_number + 1) * TOTAL_BLOCK_SIZE));
    file_size = *((uint32_t *)(inode_ptr));                                         /* grab file size */

    printf("file_name: ");
    for (i = 0; i < 32; i++)                                                        /* loop through string to print out indiv chars */
    {
        printf("%c", open_file_dentry.file_name[i]);                                        /* prints up to 32 chars */
    }
    printf(", file_type: %d, file_size: %d\n", open_file_dentry.file_type, file_size);      /* print file type to screen */
    return 0;
}



/* Function: read_dentry_by_name
 * Description: compare file name with boot block. If name matching found
                call "read_dentry_by_index" function
 * Inputs:
 * fname - filename
 * dentry - directory entry pointer
 * Outputs:	return 0 if name matching found. return -1 if no matching found
 * Side Effects: none
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)                /* uint8_t ptr to an array of characters */
{
    int i;
    uint32_t currAddr;

    if (strlen((int8_t*)fname) > 32)
    { 
        printf("read_dentry_by_name: Invaid name of file given.\n");
        return -1; 
    }

    /* loop over directory entries in boot block to find name matching object */
    for(i = file_sys_start_addr + BOOT_BLOCK_OFFSET; i < file_sys_start_addr + TOTAL_BLOCK_SIZE; i += 64)
    {
        currAddr = i;
        if((strncmp((int8_t*)(fname), (int8_t*)currAddr, 32)) == 0)                 /* compare fname string filename to current entry filename */
        {
            strncpy((int8_t*)(dentry -> file_name), (int8_t*)(fname), 32);          /* copy file name to struct */

            currAddr = currAddr + 32;                                               /* updating currAddr to get to start of file type */

            dentry -> file_type = *((uint32_t *) currAddr);                         /* copy file type to struct */

            currAddr += 4;                                                          /* increment current address by 4 bytes to get to start of inode_number address */
    
            dentry -> inode_number = *((uint32_t *) currAddr);                      /* copy inode number to struct */
            return 0;
        }
    }
    return -1;
}

/* Function: read_dentry_by_index
 * Description: copy the filename, filetype, and inode information to dentry struct
 * Inputs:
 * index - directory index in boot block
 * dentry - directory entry pointer
 * Outputs:	return 0 if copy suceess. return -1 if unsuccess
 * Side Effects: none
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
    // Check if the index exists
    unsigned int entries_count = *((unsigned int*) file_sys_start_addr);
    if (index >= entries_count)
    {
        return -1;
    }

    // Else, get the file on that index
    uint32_t boot_block_start_addr = file_sys_start_addr + BOOT_BLOCK_OFFSET + 64 * index;
    uint32_t inode_num = boot_block_start_addr + 36;
    strncpy((int8_t*)(dentry -> file_name), (int8_t*)boot_block_start_addr, 32);
    boot_block_start_addr += 32;
    dentry -> file_type = *((uint32_t *) boot_block_start_addr);
    dentry -> inode_number = *((uint32_t *) inode_num);
    return 0;
}

/* Function: read_data
 * Description: read file information from memory
                check that the given inode is within thevalid range
 * Inputs:
 * inode - inode number of the file been readed
 * offset - position of the file
 * buf - write data to this buffer
 * length - THIS IS THE SIZE OF BUFFER IN BYTES.
 * Outputs - return 0 if valid inode number, return -1 if invalid inode number
 * Side Effects: none
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{   
    // Parameter check
    if (buf == NULL)
    {
        printf("read_data: Invalid buffer pointer.\n");
        return -1;
    }

    // Get the count of inode
    uint32_t inode_num;
    inode_num = *((uint32_t *)(file_sys_start_addr + 4));
    if (!(inode_num > inode))
    {
        printf("read_data: Invalid buffer pointer.\n");
        return -1;
    }

    // Pointer to the data block area in FS
    uint32_t datablock_start_addr =  file_sys_start_addr + (TOTAL_BLOCK_SIZE * (inode_num + 1));

    // Pointer to the inode of the file
    uint32_t inode_addr = file_sys_start_addr + (TOTAL_BLOCK_SIZE * (inode + 1));

    // Pointer to the first data block # with offset
    uint32_t datablock_index_start_addr = inode_addr + (4 * (offset / TOTAL_BLOCK_SIZE + 1));

    // File size remaining
    int32_t file_size_remaining = ((int32_t) *((uint32_t *) inode_addr)) - offset;

    // Data block internal address offset with offset
    uint32_t datablock_internal_offset = offset % TOTAL_BLOCK_SIZE;
    
    // Read the file until buffer run out or file finished
    uint32_t datablock_index_addr, datablock_index, datablock_addr;
    int32_t bytes_read = 0;
    while (length > 0 && file_size_remaining > 0)
    {
        // Pointer to the current data block # with offset
        datablock_index_addr = datablock_index_start_addr + (datablock_internal_offset + bytes_read) / TOTAL_BLOCK_SIZE * 4;

        // Current data block #
        datablock_index = *((uint32_t *) datablock_index_addr);

        // Pointer to the current data block with offset
        datablock_addr = datablock_start_addr + (TOTAL_BLOCK_SIZE * datablock_index) + ((datablock_internal_offset + bytes_read) % TOTAL_BLOCK_SIZE);

        // Copy the data
        memcpy((uint8_t *) (buf + bytes_read), (uint32_t *) datablock_addr, 1);

        length--;
        file_size_remaining--;
        bytes_read++;
    }
    
    return bytes_read;
}


/* Function: return_file_size
 * Description: takes input file and returns size of file is possible
 * Inputs:
 * file_name - uint8_t pointer giving user file_name 
 * Outputs - return file size if valid file, return -1 if invalid file
 * Side Effects: none
 */
int32_t return_file_size(const uint8_t* file_name)
{

    if (read_dentry_by_name((uint8_t *)file_name, &open_file_dentry) == -1) 
    {
        printf("return_file_size: No matching file found.\n");
        return -1;
    }

    uint32_t* inode_ptr;
    uint32_t file_size;

    inode_ptr = (uint32_t *)(file_sys_start_addr + ((open_file_dentry.inode_number + 1) * TOTAL_BLOCK_SIZE));
    file_size = *((uint32_t *)(inode_ptr));                                         /* grab file size */
    return file_size;
}

/* Function: return_file_size_fd
 * Description: takes fd and returns size of file
 * Inputs:
 * fd - uint32_t fd number
 * Outputs - return file size if valid file
 * Side Effects: none
 */
int32_t return_file_size_fd(int32_t fd)
{
    uint32_t* inode_ptr;
    uint32_t file_size;

    inode_ptr = (uint32_t *)(file_sys_start_addr + (((pcb->file_descriptor)[fd].inode + 1) * TOTAL_BLOCK_SIZE));
    file_size = *((uint32_t *)(inode_ptr));                                         /* grab file size */
    return file_size;
}


/* Function: return_file_type_fd (index)
 * Description: given an inode number from fd
                return file type
 * Inputs: 
 * index - specific dentry index in bootblock
 * Outputs:	return file type
 * Side Effects: none
 */
int return_file_type_fd(uint32_t inode)
{
    uint32_t file_type, currAddr, boot_inode;
    int i;
    /* loop over directory entries in boot block to find index matching objectt */
    for(i = file_sys_start_addr + BOOT_BLOCK_OFFSET; i < file_sys_start_addr + TOTAL_BLOCK_SIZE; i += 64)
    {
        uint32_t boot_block_start_addr = i;                                         /* set boot_block_start_addr to newest i increment */
        currAddr = i + 36;                                                          /* set current address to first dir entry inode block */
        boot_inode = *((uint32_t *)currAddr);
        if(boot_inode == inode)                                                     /* compare boot block curr directory entry inode to index */
        {
            boot_block_start_addr += 32;                                            /* add 32 bytes to get to next feature in block */
            file_type = *((uint32_t *) boot_block_start_addr);                      /* find the file type */
            return file_type;
        }
    }
    return -1;
}


/* Function: check_executable
 * Description: takes input file and returns 1 if file is an executable
 * Inputs:
 * file_name - uint8_t pointer giving user file_name 
 * Outputs - return 1 if valid exec file, otherwise return -1 if file is invalid or not exec
 * Side Effects: none
 */
int32_t check_executable(const uint8_t* file_name)
{
    uint8_t buf[4];            /* temp buffer size to find ELF */
    uint8_t bufTrue[3];
    int i;
    /* initial check for file validity */
    if (read_dentry_by_name((uint8_t *)file_name, &open_file_dentry) == -1) 
    {
        printf("check_executable: No matching file found.\n");
        return -1;
    }

    /* grab first 3 chars of file data */
    read_data(open_file_dentry.inode_number, 0, (uint8_t *)buf, 4);
    for (i = 1; i < 4; i++) { bufTrue[i - 1] = buf[i]; } 

    if ((strncmp((int8_t *)(bufTrue), (int8_t *)("ELF"), 3)) == 0)       /* compare first 3 chars of file data to ELF */
    {
        return 1;
    }
    else 
    {
        printf("check_executable: Selected file is not an executable.\n");
        return -1;
    }
}


