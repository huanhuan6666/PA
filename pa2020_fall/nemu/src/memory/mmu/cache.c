#include "memory/mmu/cache.h"
#include<time.h>
#include<stdlib.h>
#include "cpu/cpu.h"
#include "memory/memory.h"
#ifdef CACHE_ENABLED
//Cacheline Cache[1024];//the 64K's size of cache
/*
the cache have 1024(2^10) lines and one line have 64B(2^6),so the offset_inline is the lowest 6 bits
8(2^3) lines is one group, so the group have 2^7, the group_count is middle 7 bits
the sign is left highest 32-6-7=19 bits

*/
// init the cache
void init_cache()
{
	// implement me in PA 3-1
	for(int i = 0; i< 1024; i++)
	    Cache[i].valid = 0;
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	// implement me in PA 3-1
	//vaddr_write(paddr, SREG_CS, len, data);//write it in memory at first
	memcpy(hw_mem+paddr,&data,len);
	
	uint32_t offset_inline = paddr & 0x3f;//0x3f is 0x11_1111 get the lowest 6 bits
	uint32_t group_count = (paddr >> 6) & 0x7f;//0x7f is 0x111_1111 get the middle 7 bits
	uint32_t sign = (paddr >> 13) & 0x7ffff;//0x7ffff is 0x111_1111_1111_1111 get the highest 19 bits 
	for(int i = 0; i < 8; i++)//each group have 8 lines
	{//compare one by one in corresponding group
	    if(Cache[group_count*8 + i].sign == sign && Cache[group_count*8 + i].valid)//find it in cache so need to rewrite
	    {
	        if(offset_inline + len >= 64)//over the line
	        {
	            memcpy(&Cache[group_count*8 + i].line[offset_inline], &data, 64 - offset_inline);
	            memcpy(&Cache[group_count*8 + i + 1].line[0], &data + (64 - offset_inline), len - 64 + offset_inline);
	        }
	        else
	        {
	            memcpy(&Cache[group_count*8 + i].line[offset_inline], &data, len);
	        }
	        break;
	    }
	}
	//if can't find line in cache we needn't to write it in cache
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	uint32_t data = 0;//we need read
	uint32_t offset_inline = paddr & 0x3f;//0x3f is 0x11_1111 get the lowest 6 bits
	uint32_t group_count = (paddr >> 6) & 0x7f;//0x7f is 0x111_1111 get the middle 7 bits
	uint32_t sign = (paddr >> 13) & 0x7ffff;//0x7ffff is 0x111_1111_1111_1111 get the highest 19 bits 
	int i =0;
	for(i = 0; i < 8; i++)//each group have 8 lines
	{//compare one by one in corresponding group
	    if(Cache[group_count*8 + i].sign == sign && Cache[group_count*8 + i].valid)//get the line we want
	    {
	        if(offset_inline + len >= 64)//over one line
	        {
	            uint32_t before = 0;
	            memcpy(&before, &Cache[group_count*8 + i].line[offset_inline], 64 - offset_inline);//get the before paddr_t
	            uint32_t after = 0;
	            memcpy(&after, &Cache[group_count*8 + i + 1].line[0], len - 64 + offset_inline);//get the left part in next line
	            data = before | after;
	        }
	        else//normal condition
	        {
	            memcpy(&data, &Cache[group_count*8 + i].line[offset_inline], len);
	        }
	    }
	}
	if(i == 8)//can't find the line in Cache
	{
	    memcpy(&data ,hw_mem+paddr, len);
	    int i = 0;
	    for(i = 0; i < 8; i++)
	    {
	        if(Cache[group_count*8 + i].valid == 0)//can be changed
	        {
	            Cache[group_count*8 + i].valid = 1;
	            Cache[group_count*8 + i].sign = sign;
	            memcpy(Cache[group_count*8 + i].line, hw_mem + paddr - offset_inline, 64);//rewrite this line
	            break;
	        }
	    }
	    if(i == 8)//no line can be changed select one randly
	    {
	        srand((unsigned)time(0));
	        int s = rand() % 8;
	        Cache[group_count*8 + s].valid = 1;
	        Cache[group_count*8 + s].sign = sign;
	        memcpy(Cache[group_count*8 + s].line, hw_mem + paddr - offset_inline, 64);//rewrite this line
	    }
	}
	return data;
}
#endif
