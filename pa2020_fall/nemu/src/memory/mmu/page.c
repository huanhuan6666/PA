#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	uint32_t dir_index = (laddr >> 22) & 0x3FF;//high 10 bits
	uint32_t pt_index = (laddr >> 12) & 0x3FF;//middle 10 bits
	uint32_t page_index = laddr & 0xFFF;//low 12 bits
	PDE* pdir = (PDE*)(hw_mem + (cpu.cr3.pdbr << 12) + (dir_index * 4));
	PTE* ptable = (PTE*)(hw_mem + (pdir->page_frame << 12) + (pt_index * 4));
	assert(pdir->present == 1);
	assert(ptable->present == 1);
	paddr_t paddr = (ptable->page_frame << 12) + page_index;
	return paddr;
	
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
