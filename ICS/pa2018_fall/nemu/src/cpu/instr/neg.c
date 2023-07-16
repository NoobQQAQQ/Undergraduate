#include "cpu/instr.h"

static void instr_execute_1op() {
    operand_read(&opr_src);
    /*
    uint32_t temp = opr_src.val;
    opr_src.val = alu_sub(0, opr_src.val, opr_src.data_size);
    if (temp == 0)
        cpu.eflags.CF = 0;
    else
        cpu.eflags.CF = 1;
    */
    if(opr_src.val == 0)
		cpu.eflags.CF = 0;
	else
		cpu.eflags.CF = 1;
	opr_src.val = -opr_src.val;
    operand_write(&opr_src);
}

make_instr_impl_1op(neg, rm, b)
make_instr_impl_1op(neg, rm, v)