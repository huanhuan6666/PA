#include "cpu/instr.h"
#include "stdio.h"
/*
Put the implementations of `lgdt' instructions here.
*/
//complete in PA3-2
make_instr_func(lgdt) 
{
	/*OPERAND rel;
	rel.type = OPR_IMM;
	rel.data_size = 32;
	rel.addr = eip + 2;
	operand_read(&rel);
	//printf("%d\n", rel.val);
	
	uint32_t limit, base;
	memcpy(&limit, hw_mem + rel.val, 2);
	memcpy(&base, hw_mem + rel.val + 2, 4);
	//printf("%d %d\n", limit, base);
	
	cpu.gdtr.limit = limit;
	cpu.gdtr.base = base;
	
	return 6; */
	int len=1;
	OPERAND rel;
	len+=modrm_rm(eip+1,&rel);
	//printf("addr:%x",rel.addr);
	rel.data_size=16;
	operand_read(&rel);
	cpu.gdtr.limit=rel.val;
	//printf("limit:%x\n",rel.val);
	rel.addr+=2;
	rel.data_size=32;
	operand_read(&rel);
	//printf("base:%x\n",rel.val);
	cpu.gdtr.base=rel.val;
	//printf("len:%x\n",len);
	print_asm_1("lgdt", "", len, &rel);
 	return len;
}