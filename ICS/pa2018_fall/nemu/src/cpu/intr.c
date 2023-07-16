#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"


void raise_intr(uint8_t intr_no) {
#ifdef IA32_INTR
	//printf("Please implement raise_intr()");
	//assert(0);

	// Trigger an exception/interrupt with 'intr_no'
	// 'intr_no' is the index to the IDT
	// Push EFLAGS, CS, and EIP
	// Find the IDT entry using 'intr_no'
	// Clear IF if it is an interrupt
	// Set EIP to the entry of the interrupt handler

	// push eflags
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eflags.val);
	// push CS
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, (uint32_t)cpu.segReg[1].val);
	// push eip
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eip);

	//printf("intr_no is %x\n", intr_no);


	// get content of idt entry
	uint32_t temp[2];
	//printf("before get idt entry\n");
	temp[0] = laddr_read(cpu.idtr.base + (intr_no << 3), 4);
	temp[1] = laddr_read(cpu.idtr.base + (intr_no << 3) + 4, 4);
	//printf("after get idt entry\n");
	// get addr of idt[intr_no]
	GateDesc *target_entry = (void*) temp;
	// load cs 
	cpu.segReg[1].val = target_entry->selector;
	load_sreg(1);
	// copy with interrupt
	if (target_entry->type == 0xE) // it is an interrupt
		cpu.eflags.IF = 0;
	// goto interrupt handler()
	cpu.eip = cpu.segReg[1].base + (target_entry->offset_15_0 | (target_entry->offset_31_16 << 16));
#endif
}

void raise_sw_intr(uint8_t intr_no) {
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
