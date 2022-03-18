#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
make_instr_func(ret_near)
{
    print_asm_0("ret"," ",1);
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.data_size = data_size;
    mem.addr = cpu.esp;
    operand_read(&mem);//get the return eip's position
    
    cpu.esp += data_size / 8;//pop the stack

    cpu.eip = mem.val;//modify the eip's position tobe the return position
    
    return 0;
}
make_instr_func(ret_near_imm16)
{
    print_asm_0("ret"," ",3);
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.data_size = data_size;
    
    mem.addr = cpu.esp;
    operand_read(&mem);//get the return eip's position
    
    cpu.esp += data_size / 8;//pop the stack
    cpu.eip = mem.val;
    
    OPERAND imm;
    imm.type = OPR_IMM;
    imm.sreg = SREG_CS;
    imm.addr = eip + 1;
    imm.data_size = 16;
    operand_read(&imm);
    int offset = sign_ext(imm.val,imm.data_size);
    cpu.esp += offset;//pop the parameter in stack
    
    return 0;
}
