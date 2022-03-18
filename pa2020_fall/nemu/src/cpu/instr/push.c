#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
    cpu.esp -= 4;
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.data_size = 32;
    mem.addr = cpu.esp;
    mem.val = opr_src.val;
    operand_write(&mem);
}

make_instr_impl_1op(push,r,v)
make_instr_impl_1op(push,rm,v)
make_instr_impl_1op(push,i,v)

make_instr_func(push_i_b)
{
    OPERAND imm;
    imm.data_size = 8;
    imm.addr = eip + 1;
    imm.type = OPR_IMM;
    imm.sreg = SREG_CS;
    operand_read(&imm);//get the 8 bit's imm
    
    print_asm_1("push","b" ,2 ,&imm);
    
    cpu.esp -= 4;
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.sreg = SREG_DS;
    mem.data_size = 32;
    mem.addr = cpu.esp;
    mem.val = sign_ext(imm.val,8);
    operand_write(&mem);//write the imm in stack
    
    return 2;//the lenth is 2
}

//complete in PA 4-1
make_instr_func(pusha)
{//push EAX、ECX、EDX、EBX、ESP（原始值）、EBP、ESI and EDI in order
    uint32_t before = cpu.esp;
    
    OPERAND rel;
    rel.type  = OPR_MEM;
    rel.data_size = 32;
    rel.sreg = SREG_DS;
    
    //push EAX
    cpu.esp -= 4;
    rel.addr = cpu.esp;
    rel.val = cpu.eax;
    operand_write(&rel);
    
    //push ECX
    cpu.esp -= 4;
    rel.addr = cpu.esp;
    rel.val = cpu.ecx;
    operand_write(&rel);
    
    //push EDX
    cpu.esp -= 4;
    rel.addr = cpu.esp;
    rel.val = cpu.edx;
    operand_write(&rel);
    
    //push EBX
    cpu.esp -= 4;
    rel.addr = cpu.esp;
    rel.val = cpu.ebx;
    operand_write(&rel);
    
    //push ESP 原始值
    cpu.esp -= 4;
    rel.addr = cpu.esp;
    rel.val = before;
    operand_write(&rel);
    
    //push EBP
    cpu.esp -= 4;
    rel.addr = cpu.esp;
    rel.val = cpu.ebp;
    operand_write(&rel);
    
    //push ESI
    cpu.esp -= 4;
    rel.addr = cpu.esp;
    rel.val = cpu.esi;
    operand_write(&rel);
    
    //push edi
    cpu.esp -= 4;
    rel.addr = cpu.esp;
    rel.val = cpu.edi;
    operand_write(&rel);
    
    print_asm_0("pusha", " ", 1);
    return 1;
}
