#include "cpu/cpu.h"
#include "memory/memory.h"
#include <stdio.h>

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg) {
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	// check boundary
	//if (cpu.segReg[sreg].limit < offset)
	//{
	//	printf("segment translation fault: cross boundary\n");
	//	return 0;
	//}
	// return linear addr
	return cpu.segReg[sreg].base + offset;
}

SegDesc gdt[3];
// load the invisible part of a segment register
void load_sreg(uint8_t sreg) {
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */

	//memcpy(gdt, hw_mem + cpu.gdtr.base, 24);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 2; j++)
			gdt[i].val[j] = laddr_read(cpu.gdtr.base + i * 8 + j * 4, 4);
	uint32_t gdt_index = cpu.segReg[sreg].index;
	//printf("cpu.gdtr.base is %u\n", cpu.gdtr.base);
	//for (int i = 0 ; i < 3; i++)
		//printf("gdt[%d] is %x and %x\n", i, gdt[i].val[0], gdt[i].val[1]);
	//printf("index = %u\n", gdt_index);
	//printf("gdt[2] is %x and %x\n", gdt[2].val[0], gdt[2].val[1]);

	// load invisible part
	cpu.segReg[sreg].base = gdt[gdt_index].base_15_0 | (gdt[gdt_index].base_23_16 << 16) | (gdt[gdt_index].base_31_24 << 24);
	cpu.segReg[sreg].limit = gdt[gdt_index].limit_15_0 | (gdt[gdt_index].limit_19_16 << 16);
	cpu.segReg[sreg].type = gdt[gdt_index].type;
	cpu.segReg[sreg].privilege_level = gdt[gdt_index].privilege_level;
	cpu.segReg[sreg].soft_use = gdt[gdt_index].soft_use;
	//printf("limit = %u\n", cpu.segReg[sreg].limit);
	//printf("base = %u\n", cpu.segReg[sreg].base);
	
	// check loss of seg
	assert(gdt[gdt_index].present == 1);
	// check granularity
	assert(gdt[gdt_index].granularity == 1);
	// check priority
	assert(gdt[gdt_index].privilege_level >= cpu.segReg[sreg].rpl);
}