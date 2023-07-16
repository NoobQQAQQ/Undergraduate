#include "cpu/instr.h"

make_instr_func(lidt) 
{
    OPERAND rm;
    int len = 1;
    len += modrm_rm(eip + 1, &rm);

    print_asm_1("lidt", "l", 7, &rm);

    cpu.idtr.limit = vaddr_read(rm.addr, SREG_DS, 2);
    cpu.idtr.base = vaddr_read(rm.addr + 2, SREG_DS, 4);
    //printf("idtr base = %x", cpu.idtr.base);
    //assert(0);
    return len;
}