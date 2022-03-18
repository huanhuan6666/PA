#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"

#ifdef CACHE_ENABLED
typedef struct
{
   bool valid;//valid bit
   uint32_t sign;//sign bit 
   uint8_t line[64];//the data each line, the size is 64B
}Cacheline;

Cacheline Cache[1024];//the 64K's size of cache
// init the cache
void init_cache();

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data);

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len);

#endif

#endif
