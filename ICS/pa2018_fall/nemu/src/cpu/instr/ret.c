#include "cpu/instr.h"

make_instr_func(ret_near)
{
    print_asm_0("ret_near", data_size == 16 ? "w" : "l", 1);
    cpu.eip = vaddr_read(cpu.esp, SREG_SS, data_size / 8);
    //cpu.eip = paddr_read(cpu.esp, data_size / 8);
    cpu.esp += data_size / 8;
    if (data_size == 16)
        cpu.eip &= 0x0000FFFF;
    return 0;
}

make_instr_func(ret_near_imm16)
{
    OPERAND imm16;
    imm16.type = OPR_IMM;
    imm16.data_size = data_size;
    imm16.addr = cpu.eip + 1;
    operand_read(&imm16);
    print_asm_1("ret_near_imm16", data_size == 16 ? "w" : "l", 3, &imm16);
    cpu.eip = vaddr_read(cpu.esp, SREG_SS, data_size / 8);
    //cpu.eip = paddr_read(cpu.esp, data_size / 8);
    cpu.esp += data_size / 8;
    if (data_size == 16)
        cpu.eip &= 0x0000FFFF;
    cpu.esp += imm16.val;
    return 0;
}