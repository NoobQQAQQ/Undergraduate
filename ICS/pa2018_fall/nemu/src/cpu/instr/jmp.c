#include "cpu/instr.h"

make_instr_func(jmp_near) {
        OPERAND rel;
        rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = cpu.eip + 1;

        operand_read(&rel);

	int offset = sign_ext(rel.val, data_size);
	print_asm_1("jmp", "", 2, &rel);

	cpu.eip += offset;

        return 1 + data_size / 8;
}


make_instr_func(jmp_near_indirect)
{
        OPERAND rm;
        rm.data_size = data_size;
        int len = 1 + modrm_rm(cpu.eip + 1, &rm);
        rm.sreg = SREG_SS;
        operand_read(&rm);
        print_asm_1("jmp_near_indirect", rm.data_size == 16 ? "w" : "l", len, &rm);
        if (rm.data_size == 16)
                cpu.eip = rm.val & 0x0000FFFF;
        else
                cpu.eip = rm.val;
        return 0;
}

make_instr_func(jmp_short)
{
        OPERAND rel;
        rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
        rel.data_size = 8;
        rel.addr = cpu.eip + 1;

        operand_read(&rel);

	int offset = sign_ext(rel.val, rel.data_size);
	print_asm_1("jmp", rel.data_size ==8 ? "b":(rel.data_size ==16?"w":"l"), 2, &rel);

	cpu.eip += offset;

        return 1 + rel.data_size / 8;
}

make_instr_func(jmp_far_imm)
{
        //int len = 1;
        if (data_size == 32) 
        {
                cpu.eip = paddr_read(eip + 1, 4);
                cpu.segReg[1].val = paddr_read(eip + 5, 2);
                //len += 6;
        }
        else
        {
                cpu.eip = paddr_read(eip + 1, 2) & 0xffff;
                cpu.segReg[1].val = paddr_read(eip + 3, 2);
                //len += 4;
        }
        //print_asm_2("ljmp", "l", 6, )
       //return len;
       return 0;
}