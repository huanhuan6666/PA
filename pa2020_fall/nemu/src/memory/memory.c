#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}


uint32_t paddr_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
#ifdef CACHE_ENABLED
    ret = cache_read(paddr, len);
#else
	ret = hw_mem_read(paddr, len);
#endif
	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
#ifdef CACHE_ENABLED
    cache_write(paddr, len, data);
#else
	hw_mem_write(paddr, len, data);
#endif
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{
    assert(len == 1 || len == 2 || len == 4);
    if(cpu.cr0.pe == 1 && cpu.cr0.pg == 1)
    {
        if((laddr & 0xFFF) + len >= 0x1000)//over 4KB (laddr >> 12)!=((laddr + len - 1) >> 12)
        {
            uint32_t x = 0x1000 - (laddr & 0xFFF);
            uint32_t ret = paddr_read(page_translate(laddr), x);
            ret += (paddr_read(page_translate(laddr + x), len - x) << (x << 3));
            return ret;
        }
        else
        {
            uint32_t paddr = page_translate(laddr);
            return paddr_read(paddr, len);
        }
    }
    else
	    return paddr_read(laddr, len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
    assert(len == 1 || len == 2 || len == 4);
    if(cpu.cr0.pe == 1 && cpu.cr0.pg == 1)
    {
        uint32_t paddr = page_translate(laddr);
        paddr_write(paddr, len, data);
    }
    else
	    paddr_write(laddr, len, data);
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	uint32_t laddr = 0;
#ifdef IA32_SEG
	if(cpu.cr0.pe)
	{
	    laddr = segment_translate(vaddr, sreg);// & 0x07);
	    return laddr_read(laddr, len);
	}
	else return laddr_read(vaddr, len);
#else
	return laddr_read(vaddr, len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
	uint32_t laddr = 0;
#ifdef IA32_SEG 
	if(cpu.cr0.pe)
	{
	    laddr = segment_translate(vaddr, sreg);// & 0x07);
	    laddr_write(laddr, len, data);
	}
	else laddr_write(vaddr, len, data);
#else
	laddr_write(vaddr, len, data);
#endif
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
#ifdef CACHE_ENABLED
    init_cache();//init the cache
#endif

#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
