#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near)
{
    cpu.esp -= data_size / 8;
    
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.addr = cpu.esp;
    mem.data_size = data_size;
    mem.val = eip + 1 + data_size / 8;
    
    operand_write(&mem);//push the eip in stack
    
    OPERAND offset;
    offset.type = OPR_IMM;
    //add in 3-3
    offset.sreg = SREG_CS;
    offset.addr = eip + 1;
    offset.data_size = data_size;
    operand_read(&offset);//get the offset
    print_asm_1("call"," ",1 + data_size / 8, &offset);
    int rel = sign_ext(offset.val, data_size);
    
    cpu.eip += (1 + data_size / 8 + rel);//modify the eip tobe the called function's position

    return 0;
}
make_instr_func(call_indirct)
{
    cpu.esp -= data_size / 8;
    
    OPERAND mem,dest;
    dest.data_size = data_size;
    int len = 1;
    len += modrm_rm(eip + 1, &dest);
    operand_read(&dest);//get the new next eip
    
    mem.data_size = data_size;//write the return eip in stack
    mem.type = OPR_MEM;
    //add in 3-3
    mem.sreg = SREG_DS;
    mem.addr = cpu.esp;
    mem.val = len + eip;
    operand_write(&mem);
    
    if(data_size == 16)
        cpu.eip = dest.val & 0xFF;
    else
        cpu.eip = dest.val;
    return 0;
}