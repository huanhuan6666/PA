#include "cpu/instr.h"
/*
Put the implementations of `iret' instructions here.
*/
make_instr_func(iret)
{
    OPERAND rel;
    rel.type = OPR_MEM;
    rel.sreg = SREG_SS;
    
    //eip
    rel.data_size = 32;
    rel.addr = cpu.esp;
    operand_read(&rel);
    //printf("eip:%x\n",rel.val);
    cpu.eip = rel.val;
    cpu.esp += 4;
    
    //cs
    rel.data_size = 16;
    rel.addr = cpu.esp;
    operand_read(&rel);
    //printf("cs:%x\n",rel.val);
    cpu.cs.val = rel.val;
    cpu.esp += 2;
    
    //eflags
    rel.data_size = 32;
    rel.addr = cpu.esp;
    operand_read(&rel);
    //printf("eflags:%x\n",rel.val);
    cpu.eflags.val = rel.val;
    cpu.esp += 4;
    
    print_asm_0("iret", " ", 1);
    return 0;
    /*print_asm_0("iret", "", 1);
	cpu.eip=vaddr_read(cpu.esp,SREG_SS,4);
	cpu.esp+=4;
	cpu.cs.val=vaddr_read(cpu.esp,SREG_SS,2);
	cpu.esp+=2;
	cpu.eflags.val=vaddr_read(cpu.esp,SREG_SS,4);
	cpu.esp+=4;
	return 0;*/
}