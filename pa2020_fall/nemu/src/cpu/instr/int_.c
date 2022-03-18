#include "cpu/intr.h"
#include "cpu/instr.h"

/*
Put the implementations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/
make_instr_func(int_)
{
    int intr_no = instr_fetch(eip + 1, 1);//1 B
    //print_asm("int %x", intr_no);
    raise_sw_intr(intr_no);
    return 0;
    /*OPERAND rel;
    rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip + 1;
    operand_read(&rel);
	print_asm_1("int", "", 2, &rel);
	raise_sw_intr(rel.val);
	return 0;*/
}