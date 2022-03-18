#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
make_instr_func(leave)
{
    cpu.esp = cpu.ebp;
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.data_size = data_size;
    mem.addr = cpu.esp;
    operand_read(&mem);
    print_asm_0("leave"," ",1);
    cpu.ebp = mem.val;
    cpu.esp += data_size / 8;
    return 1;//lenth is 1
}