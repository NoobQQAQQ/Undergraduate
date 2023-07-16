#include "cpu/instr.h"

static void instr_execute_1op() {
	operand_read(&opr_src);
    //opr_src.val = paddr_read(cpu.esp, opr_src.data_size / 8);
    opr_src.val = vaddr_read(cpu.esp, SREG_SS, opr_src.data_size / 8);
    cpu.esp += opr_src.data_size / 8;
	operand_write(&opr_src);
}

make_instr_impl_1op(pop, r, v)
make_instr_impl_1op(pop, rm, v)

make_instr_func(popa)
{
     if (data_size == 16)
    {
        for (int i = 7; i >= 0; i--)
        {
            if (i == 4) // skip sp
            {
                cpu.esp += 2;
                continue;
            }
            //cpu.gpr[i]._16 = paddr_read(cpu.esp, 2);
            cpu.gpr[i]._16 = vaddr_read(cpu.esp, SREG_SS, 2);
            cpu.esp += 2;
        }
    }
    else if (data_size == 32)
    {
        for (int i = 7; i >= 0; i--)
        {
            if (i == 4) // skip esp
            {
                cpu.esp += 4;
                continue;
            }
            //cpu.gpr[i]._32 = paddr_read(cpu.esp, 4);
            cpu.gpr[i]._32 = vaddr_read(cpu.esp, SREG_SS, 4);
            cpu.esp += 4;
        }
    }
    return 1;
}