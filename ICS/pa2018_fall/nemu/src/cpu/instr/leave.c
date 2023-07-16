#include "cpu/instr.h"

make_instr_func(leave) 
{
    cpu.esp = cpu.ebp;
    //cpu.ebp = paddr_read(cpu.esp, data_size / 8);
    cpu.ebp = vaddr_read(cpu.esp, SREG_SS, data_size / 8);
    cpu.esp += data_size / 8;
    print_asm_0("leave", data_size == 8 ? "b" : (data_size == 16 ? "w" : "l"), 1);
    return 1;
}