#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
    operand_read(&opr_dest);
	if (opr_src.data_size < opr_dest.data_size) // do sign extend
    {
        opr_src.val = sign_ext(opr_src.val & (0xFFFFFFFF >> (32 - opr_src.data_size)), opr_src.data_size);
        opr_src.val = opr_src.val & (0xFFFFFFFF >> (32 - opr_dest.data_size));
    }
    uint32_t temp = opr_dest.val;
    opr_dest.val = opr_src.val;
    opr_src.val = temp;
    operand_write(&opr_src);
    operand_write(&opr_dest);
}

make_instr_impl_2op(xchg, r, rm, b)
make_instr_impl_2op(xchg, r, rm, v)
