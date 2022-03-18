#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
	//complete in PA4-1
	uint32_t addr = (uint32_t)hw_mem + page_translate(segment_translate(cpu.idtr.base + 8*intr_no, SREG_DS));
	GateDesc* gate = (void*)addr;
	
	//push EFLAGS CS EIP
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eflags.val);
	cpu.esp -= 2;
	vaddr_write(cpu.esp, SREG_SS, 2, cpu.cs.val);
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eip);
	
	if(gate->type == 0XF)//if it's an interrupt
	    cpu.eflags.IF = 0;//clear IF
	else if(gate->type == 0XE)
	    cpu.eflags.IF = 1;
	
	cpu.eip = (gate->offset_31_16 << 16) + gate->offset_15_0;//the entry of the interrupt handle
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
