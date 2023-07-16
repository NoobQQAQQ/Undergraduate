#include "cpu/instr.h"

make_instr_func(call_near)
{
    OPERAND rel;
    rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = cpu.eip + 1;

    operand_read(&rel);
    cpu.eip += 1 + data_size / 8;
    int len = 1 + rel.data_size / 8;
    print_asm_1("call_near", data_size == 16 ? "w" : "l", len, &rel);
    cpu.esp -= data_size / 8;
    //paddr_write(cpu.esp, data_size / 8, cpu.eip);
    vaddr_write(cpu.esp, SREG_SS, data_size / 8, cpu.eip);
    cpu.eip += rel.val;
    if (data_size == 16)
        cpu.eip &= 0x0000ffff;
    return 0;
}

make_instr_func(call_near_indirect)
{
    OPERAND rm;
    rm.data_size = data_size;

    int len = 1 + modrm_rm(cpu.eip + 1, &rm);
    rm.sreg = SREG_SS;
    operand_read(&rm);
    cpu.eip += len;

    print_asm_1("call_near_indirect", data_size == 16 ? "w" : "l", len, &rm);
    cpu.esp -= data_size / 8;
    //paddr_write(cpu.esp, data_size / 8, cpu.eip);
    vaddr_write(cpu.esp, SREG_SS, data_size / 8, cpu.eip);
    if (data_size == 16)
        cpu.eip = rm.val & 0x0000ffff;
    else
        cpu.eip = rm.val;
    return 0;
}