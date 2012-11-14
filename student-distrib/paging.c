/* paging.c - the paging initialization part of the kernel
 */

#include "lib.h"
#include "paging.h"
#include "files.h"
#include "syscalls.h"

int process_number;
uint32_t new_page_dir_addr;

/*
 * init_paging()
 *
 * Called from kernel.c to initialize paging.
 *
 * Retvals
 * 
 */
int32_t init_paging(void)
{
	int i;
	int page_table_holder;
	
	process_number = 0;

	/* Initialize page table for initial space pages. */
	/* Set all to present except for the page at address 0. */
	for( i = 0; i < MAX_PAGE_TABLE_SIZE; i++ ) {
		page_table[i].present = (i == 0) ? 0 : 1;
		page_table[i].read_write = 0;
		page_table[i].user_supervisor = 0;
		page_table[i].write_through = 0;
		page_table[i].cache_disabled = 0;
		page_table[i].accessed = 0;
		page_table[i].dirty = 0;
		page_table[i].pat = 0;
		page_table[i].global = 0;
		page_table[i].avail = 0;
		page_table[i].page_addr = i;
	}

	/* Initialize first page directory entry. */
	page_table_holder = (int)page_table;
	page_directories[0].dentries[0].KB.present = 1;
	page_directories[0].dentries[0].KB.read_write = 0;
	page_directories[0].dentries[0].KB.user_supervisor = 0;
	page_directories[0].dentries[0].KB.write_through = 0;
	page_directories[0].dentries[0].KB.cache_disabled = 0;
	page_directories[0].dentries[0].KB.accessed = 0;
	page_directories[0].dentries[0].KB.page_size = 0;
	page_directories[0].dentries[0].KB.global = 0;
	page_directories[0].dentries[0].KB.avail = 0;
	page_directories[0].dentries[0].KB.table_addr = page_table_holder >> 12;

	/* Initialize the kernel page directory entry. */
	page_directories[0].dentries[1].MB.present = 1;
	page_directories[0].dentries[1].MB.read_write = 1;
	page_directories[0].dentries[1].MB.user_supervisor = 0;
	page_directories[0].dentries[1].MB.write_through = 0;
	page_directories[0].dentries[1].MB.cache_disabled = 0;
	page_directories[0].dentries[1].MB.accessed = 0;
	page_directories[0].dentries[1].MB.dirty = 0;
	page_directories[0].dentries[1].MB.page_size = 1;
	page_directories[0].dentries[1].MB.global = 1;
	page_directories[0].dentries[1].MB.avail = 0;
	page_directories[0].dentries[1].MB.pat = 0;
	page_directories[0].dentries[1].MB.page_addr = 1;

	/* Initialize the remaining page directory entries to absent. */
	for( i = 2; i < MAX_PAGE_DIRECTORY_SIZE; i++ ) {
	page_directories[0].dentries[i].MB.present = 0;
	page_directories[0].dentries[i].MB.read_write = 1;
	page_directories[0].dentries[i].MB.user_supervisor = 0;
	page_directories[0].dentries[i].MB.write_through = 0;
	page_directories[0].dentries[i].MB.cache_disabled = 0;
	page_directories[0].dentries[i].MB.accessed = 0;
	page_directories[0].dentries[i].MB.dirty = 0;
	page_directories[0].dentries[i].MB.page_size = 0;
	page_directories[0].dentries[i].MB.global = 0;
	page_directories[0].dentries[i].MB.avail = 0;
	page_directories[0].dentries[i].MB.pat = 0;
	page_directories[0].dentries[i].MB.page_addr = i;
	}

	/* Set control registers to enable paging correctly. */
	asm (
	"movl $page_directories, %%eax   ;"
	"andl $0xFFFFFFE7, %%eax          ;"
	"movl %%eax, %%cr3                ;"
	"movl %%cr4, %%eax                ;"
	"orl $0x00000010, %%eax           ;"
	"movl %%eax, %%cr4                ;"
	"movl %%cr0, %%eax                ;"
	"orl $0x80000000, %%eax 	      ;"
	"movl %%eax, %%cr0                 "
	: : : "eax", "cc" );
	
	return 0;
}

/*
 * setup_new_task()
 *
 * Called from 'execute' to set up a new page directory.
 *
 * Retvals
 * 
 */
int32_t setup_new_task( uint8_t process_number )
{
	uint32_t i;
	int new_page_table_holder;
	
	/* Reject the request if it is out of range. */
	if( process_number >= 8 )
	{
		return -1;
	}
	
	new_page_dir_addr = (uint32_t)(&page_directories[process_number]);
	
	/* Initialize page table for initial space pages. */
	/* Set all to present except for the page at address 0. */
	for( i = 0; i < MAX_PAGE_TABLE_SIZE; i++ ) {
		new_page_table[i].present = (i == 0) ? 0 : 1;
		new_page_table[i].read_write = 0;
		new_page_table[i].user_supervisor = 0;
		new_page_table[i].write_through = 0;
		new_page_table[i].cache_disabled = 0;
		new_page_table[i].accessed = 0;
		new_page_table[i].dirty = 0;
		new_page_table[i].pat = 0;
		new_page_table[i].global = 1;
		new_page_table[i].avail = 0;
		new_page_table[i].page_addr = i;
	}

	/* Initialize first page directory entry. */
	new_page_table_holder = (int)new_page_table;
	page_directories[process_number].dentries[0].KB.present = 1;
	page_directories[process_number].dentries[0].KB.read_write = 0;
	page_directories[process_number].dentries[0].KB.user_supervisor = 0;
	page_directories[process_number].dentries[0].KB.write_through = 0;
	page_directories[process_number].dentries[0].KB.cache_disabled = 0;
	page_directories[process_number].dentries[0].KB.accessed = 0;
	page_directories[process_number].dentries[0].KB.page_size = 0;
	page_directories[process_number].dentries[0].KB.global = 0;
	page_directories[process_number].dentries[0].KB.avail = 0;
	page_directories[process_number].dentries[0].KB.table_addr = new_page_table_holder >> 12;
	
	/* Initialize the kernel page directory entry. */
	page_directories[process_number].dentries[1].MB.present = 1;
	page_directories[process_number].dentries[1].MB.read_write = 1;
	page_directories[process_number].dentries[1].MB.user_supervisor = 1;
	page_directories[process_number].dentries[1].MB.write_through = 0;
	page_directories[process_number].dentries[1].MB.cache_disabled = 0;
	page_directories[process_number].dentries[1].MB.accessed = 0;
	page_directories[process_number].dentries[1].MB.dirty = 0;
	page_directories[process_number].dentries[1].MB.page_size = 1;
	page_directories[process_number].dentries[1].MB.global = 1;
	page_directories[process_number].dentries[1].MB.avail = 0;
	page_directories[process_number].dentries[1].MB.pat = 0;
	page_directories[process_number].dentries[1].MB.page_addr = 1;
	
	/* Set up a directory entry for the program image. */
	page_directories[process_number].dentries[0x20].MB.present = 1;
	page_directories[process_number].dentries[0x20].MB.read_write = 1;
	page_directories[process_number].dentries[0x20].MB.user_supervisor = 1;
	page_directories[process_number].dentries[0x20].MB.write_through = 0;
	page_directories[process_number].dentries[0x20].MB.cache_disabled = 0;
	page_directories[process_number].dentries[0x20].MB.accessed = 0;
	page_directories[process_number].dentries[0x20].MB.dirty = 0;
	page_directories[process_number].dentries[0x20].MB.page_size = 1;
	page_directories[process_number].dentries[0x20].MB.global = 0;
	page_directories[process_number].dentries[0x20].MB.avail = 0;
	page_directories[process_number].dentries[0x20].MB.pat = 0;
	page_directories[process_number].dentries[0x20].MB.page_addr = process_number+1;
	
	/* Set control registers to enable paging correctly. */
	asm (
	"movl new_page_dir_addr, %%eax    ;"
	"andl $0xFFFFFFE7, %%eax          ;"
	"movl %%eax, %%cr3                ;"
	"movl %%cr4, %%eax                ;"
	"orl $0x00000090, %%eax           ;"
	"movl %%eax, %%cr4                ;"
	"movl %%cr0, %%eax                ;"
	"orl $0x80000000, %%eax 	      ;"
	"movl %%eax, %%cr0                 "
	: : : "eax", "cc" );
	
	return 0;
}
