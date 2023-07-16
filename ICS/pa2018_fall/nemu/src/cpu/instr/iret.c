#include "cpu/instr.h"

make_instr_func(iret) 
{
    print_asm_0("iret", "", 1);
    // get next instruction's addr by popping eip
    cpu.eip = vaddr_read(cpu.esp, SREG_SS, 4);
    cpu.esp += 4;
    // pop cs
    cpu.segReg[1].val = vaddr_read(cpu.esp, SREG_SS, 2);
    cpu.esp += 4;
    load_sreg(1); // load invisible part
    // pop eflags
    cpu.eflags.val = vaddr_read(cpu.esp, SREG_SS, 4);
    cpu.esp += 4;
    return 0;
}