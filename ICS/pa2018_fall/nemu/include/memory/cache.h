#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"

#define line_num 1024

typedef struct 
{
    uint8_t valid :1; // valid bit
    uint32_t tag :19;
    //uint8_t grp_no :7;
    uint8_t data[64];
} CacheLine;

void init_cache();

uint32_t cache_read(paddr_t paddr, size_t len, CacheLine * Cache);

void cache_write(paddr_t paddr, size_t len, uint32_t data, CacheLine * Cache);

#endif