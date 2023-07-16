#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "memory/cache.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

extern CacheLine Cache[line_num];

uint8_t hw_mem[MEM_SIZE_B];
	
uint32_t hw_mem_read(paddr_t paddr, size_t len) {
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data) {
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len) {
	uint32_t ret = 0;

	if (is_mmio(paddr) == -1)
	{
		#ifdef CACHE_ENABLED
			ret = cache_read(paddr, len, Cache);
		#else
			ret = hw_mem_read(paddr, len);
		#endif
	}
	else
		ret = mmio_read(paddr, len, is_mmio(paddr));

	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data) {

	if (is_mmio(paddr) == -1)
	{
		#ifdef CACHE_ENABLED
			cache_write(paddr, len, data, Cache);
		#else
			hw_mem_write(paddr, len, data);
		#endif
	}
	else
		mmio_write(paddr, len, data, is_mmio(paddr));
}


uint32_t laddr_read(laddr_t laddr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_PAGE
	return paddr_read(laddr, len);
#else
	if (cpu.cr0.pe && cpu.cr0.pg)
	{
		if ((laddr & 0xfff) + len > 0x1000) // greater than 4kb, cross page
		{
			//printf("read cross page!\n");
			size_t len_in_page = 0x1000 - (laddr & 0xfff);
			paddr_t paddr_in_page = page_translate(laddr);
			paddr_t paddr_out_page = page_translate(laddr + len_in_page);
			//printf("laddr = %x, laddr + len_in_page = %x\n", (uint32_t)laddr, (uint32_t)laddr + len_in_page);
			//printf("paddr_in_page = %x, paddr_out_page = %x\n", paddr_in_page, paddr_out_page);
			return paddr_read(paddr_in_page, len_in_page) + (paddr_read(paddr_out_page, len - len_in_page) << (len_in_page << 3));
		}
		else
		{
			paddr_t paddr = page_translate(laddr);
			return paddr_read(paddr, len);
		}
	}
	else
		return paddr_read(laddr, len);
#endif
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_PAGE
	paddr_write(laddr, len, data);
#else
	if (cpu.cr0.pe && cpu.cr0.pg)
	{
		if ((laddr & 0xfff) + len > 0x1000) // greater than 4kb, cross page
		{
			//printf("write cross page!\n");
			size_t len_in_page = 0x1000 - (laddr & 0xfff);
			paddr_t paddr_in_page = page_translate(laddr);
			paddr_t paddr_out_page = page_translate(laddr + len_in_page);
			paddr_write(paddr_in_page, len_in_page, data);
			paddr_write(paddr_out_page, len - len_in_page, data >> (len_in_page << 3));
		}
		else
		{
			paddr_t paddr = page_translate(laddr);
			paddr_write(paddr, len, data);
		}
	}
	else
		return paddr_write(laddr, len, data);
#endif
}


uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	return laddr_read(vaddr, len);
#else
	uint32_t laddr = vaddr;
	if (cpu.cr0.pe)
		laddr = segment_translate(vaddr, sreg);
	return laddr_read(laddr, len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG	
	laddr_write(vaddr, len, data);
#else
	uint32_t laddr = vaddr;
	if (cpu.cr0.pe)
		laddr = segment_translate(vaddr, sreg);
	laddr_write(laddr, len, data);
#endif
}

void init_mem() {
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);

	#ifdef CACHE_ENABLED
		init_cache();
	#endif

#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t * get_mem_addr() {
	return hw_mem;
}
