#include "cpu/instr.h"

make_instr_func(lgdt) 
{
    OPERAND rm;
    int len = 1;
    len += modrm_rm(eip + 1, &rm);
    //rm.data_size = 32;
    //operand_read(&rm);
    //printf("len = %d\n", len);
    print_asm_1("lgdt", "l", 7, &rm);
    //printf("rm.val is %x", (uint32_t)(rm.val));
    //printf("hw_mem is %x\n", (uint32_t) hw_mem);
    //printf("rm.addr is %x\n", rm.addr);
    cpu.gdtr.limit = vaddr_read(rm.addr, SREG_DS, 2);
    cpu.gdtr.base = vaddr_read(rm.addr + 2, SREG_DS, 4);
    //printf("len = %d\n", len);
    //printf("cpu.gdtr.base is %x\n", cpu.gdtr.base);
    //printf("cpu.gdtr.limit is %x\n", cpu.gdtr.limit);
    return len;
}