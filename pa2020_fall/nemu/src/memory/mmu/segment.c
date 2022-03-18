#include "cpu/cpu.h"
#include "cpu/reg.h"
#include "memory/memory.h"
// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	uint32_t base = cpu.segReg[sreg].base;//get the segment's base addr
	return base + offset;//segment's base addr add the offset is the laddr
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	 uint32_t addr = (uint32_t)hw_mem + cpu.gdtr.base + 8 * cpu.segReg[sreg].index;//1 descriptor in gdt is 8 byte
	 SegDesc* sreg_des_addr = (void*) addr;
	 uint32_t base = (sreg_des_addr->base_31_24 << 24) + (sreg_des_addr->base_23_16 << 16) + sreg_des_addr->base_15_0;
	 uint32_t limit = (sreg_des_addr->limit_19_16 << 16) + sreg_des_addr->limit_15_0;
	 uint32_t granularity = sreg_des_addr->granularity;
	 uint32_t pl = sreg_des_addr->privilege_level;
	 assert(base == 0);
	 assert(limit == 0xFFFFF);
	 assert(granularity == 1);
	 cpu.segReg[sreg].base = base;
	 cpu.segReg[sreg].limit = limit;
	 cpu.segReg[sreg].privilege_level = pl;
}