#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_DS;
    opr_dest.addr = cpu.esp;
    operand_read(&opr_dest);
    opr_src.val = opr_dest.val;
    operand_write(&opr_src);
    //cpu.esp += data_size / 8;
    cpu.esp += 4;
}
make_instr_impl_1op(pop,r,v)

make_instr_func(popa)
{//出栈顺序依次是：EDI,ESI,EBP,EBX,EDX,ECX,EAX
    OPERAND rel;
    rel.data_size = 32;
    rel.type = OPR_MEM;
    rel.sreg = SREG_DS;
    
    //pop EDI
    rel.addr = cpu.esp;
    operand_read(&rel);
    cpu.edi = rel.val;
    cpu.esp += 4;
    
    //pop ESI
    rel.addr = cpu.esp;
    operand_read(&rel);
    cpu.esi = rel.val;
    cpu.esp += 4;
    
    //pop EBP
    rel.addr = cpu.esp;
    operand_read(&rel);
    cpu.ebp = rel.val;
    cpu.esp += 4;
    
    //increment ESP by 4
    cpu.esp += 4;
    
    //pop EBX
    rel.addr = cpu.esp;
    operand_read(&rel);
    cpu.ebx = rel.val;
    cpu.esp += 4;

    //pop EDX
    rel.addr = cpu.esp;
    operand_read(&rel);
    cpu.edx = rel.val;
    cpu.esp += 4;
    
    //pop ECX
    rel.addr = cpu.esp;
    operand_read(&rel);
    cpu.ecx = rel.val;
    cpu.esp += 4;
    
    //pop EAX
    rel.addr = cpu.esp;
    operand_read(&rel);
    cpu.eax = rel.val;
    cpu.esp += 4;
    
    print_asm_0("popa", " ", 1);
    return 1;
}