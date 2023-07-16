#include "cpu/instr.h"
#include "cpu/intr.h"

make_instr_func(int_) 
{
    //printf("into int/n!");
    //assert(0);
    print_asm_0("int", "", 2);
    uint8_t intr_no = vaddr_read(eip + 1, SREG_CS, 1);
    raise_sw_intr(intr_no);
    return 0;
}