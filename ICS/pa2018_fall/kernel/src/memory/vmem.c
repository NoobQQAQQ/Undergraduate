#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PT_SIZE - 1) / PT_SIZE)  // number of page tables to cover the vmem

PDE* get_updir();
static PTE vptable[(VMEM_ADDR + SCR_SIZE) / PAGE_SIZE + 1] align_to_page; // vmem page tables

void create_video_mapping() {
	
	/* TODO: create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memeory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */

	//panic("please implement me");
	PDE *updir = va_to_pa(get_updir());
    PTE *uptable = (PTE *)va_to_pa(vptable);

    uint32_t idx = VMEM_ADDR / PAGE_SIZE; // get the index of the first vmem page
	uint32_t addr = VMEM_ADDR;

    memset(uptable, 0, sizeof(uptable)); // make all PTE invalid 

	updir[0].val = make_pde(uptable); // set pde (0xa0000 belong to 0 page directory)

	int page_nr = (SCR_SIZE / PAGE_SIZE) + 1; // the number of pages needed to be set
    for(int i = 0; i < page_nr; i++)
	{
        uptable[idx + i].val = make_pte(addr);
        addr += PAGE_SIZE;
    }
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

