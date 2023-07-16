#include "cpu/cpu.h"
#include "memory/memory.h"

// wang lao shi zei qiang
typedef union {
	struct {
		uint32_t pf_off		:12;
		uint32_t pt_idx		:10;
		uint32_t pdir_idx	:10;
	};
	uint32_t addr;
} PageAddr;

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr) {
#ifndef TLB_ENABLED
	//printf("\nPlease implement page_translate()\n");
	//assert(0);
	PageAddr *addr = (void*) &laddr;
	paddr_t pdir_base = cpu.cr3.val & ~PAGE_MASK;
	
	// get target pde
	PDE pde;
	pde.val	= paddr_read(pdir_base + addr->pdir_idx * 4, 4);
	assert(pde.present);

	// get target pte
	paddr_t pt_base = pde.val & ~PAGE_MASK;
	PTE pte;
	pte.val = paddr_read(pt_base + addr->pt_idx * 4, 4);
	assert(pte.present);

	return (pte.val & ~PAGE_MASK) + addr->pf_off;
#else	
	return tlb_read(laddr) | (laddr & PAGE_MASK);;
#endif
}
