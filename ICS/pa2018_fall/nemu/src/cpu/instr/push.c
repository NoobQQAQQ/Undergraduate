#include "cpu/instr.h"

static void instr_execute_1op() {
	operand_read(&opr_src);
    if (opr_src.data_size == 8)
    {
        opr_src.val = sign_ext(opr_src.val & (0xFFFFFFFF >> (32 - opr_src.data_size)), opr_src.data_size);
        opr_src.data_size = 32;
    }
    cpu.esp -= opr_src.data_size / 8;
    //paddr_write(cpu.esp, opr_src.data_size / 8, opr_src.val);
    vaddr_write(cpu.esp, SREG_SS, opr_src.data_size / 8, opr_src.val);
}

make_instr_impl_1op(push, r, v)
make_instr_impl_1op(push, i, b)
make_instr_impl_1op(push, i, v)
make_instr_impl_1op(push, rm, v)

make_instr_func(pusha)
{
    if (data_size == 16)
    {
        uint16_t temp = cpu.gpr[4]._16; // sp
        for (int i = 0; i < 8; i++)
        {
            cpu.esp -= 2;
            if (i == 4) // push old sp
            {
                //paddr_write(cpu.esp, 2, temp);
                vaddr_write(cpu.esp, SREG_SS, 2, temp);
                continue;
            }
            //paddr_write(cpu.esp, 2, cpu.gpr[i]._16);
            vaddr_write(cpu.esp, SREG_SS, 2, cpu.gpr[i]._16);
        }
    }
    else if (data_size == 32)
    {
        uint32_t temp = cpu.gpr[4]._32; // esp
        for (int i = 0; i < 8; i++)
        {
            cpu.esp -= 4;
            if (i == 4) // push old esp
            {
                //paddr_write(cpu.esp, 4, temp);
                vaddr_write(cpu.esp, SREG_SS, 4, temp);
                continue;
            }
            //paddr_write(cpu.esp, 4, cpu.gpr[i]._32);
            vaddr_write(cpu.esp, SREG_SS, 4, cpu.gpr[i]._32);
        }
    }
    return 1;
}