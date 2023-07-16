#include "memory/cache.h"
#include "memory/memory.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

CacheLine Cache[line_num];

void init_cache()
{
    for (int i = 0; i < line_num; i++)
        Cache[i].valid = 0;
    return;
}

uint32_t cache_read(paddr_t paddr, size_t len, CacheLine * Cache)
{
    uint32_t tag; // 19 bits
    uint8_t grp_no, baddr; // 7bits and 6bits
    tag = (paddr >> 13) & 0x0007FFFF;
    grp_no = (paddr >> 6) & 0x7F;
    baddr = paddr & 0x3F;
    uint16_t start_line = ((uint16_t) grp_no) * 8;
    for (uint16_t i = start_line; i < start_line + 8; i++)
    {
        
        if (Cache[i].valid && Cache[i].tag == tag) // hit cache
        {
            //printf("read hit cache\n");
            // not cross line
            if (baddr + len <= 64)
            {
                //printf("read not cross line\n");
                uint32_t result = 0;
                memcpy(&result, &Cache[i].data[baddr], len);
                return result;
            }
            // cross line
            else
            {
                //printf("read cross line\n");
                paddr_t rec_addr = paddr + 64 - baddr;
                size_t rec_len = baddr + len - 64;
                uint32_t result = 0;
                uint32_t base = 8 * (64 - baddr);
                memcpy(&result, &Cache[i].data[baddr], 64 - baddr);
                return result | (cache_read(rec_addr, rec_len, Cache) << base);
            }
        }

        else if (i == start_line + 7) // not hit cache
        {
            //printf("read not hit cache\n");
            for (uint16_t j = start_line; j < start_line + 8; j++)
            {
                // have an empty line
                if (!Cache[j].valid)
                {
                    //printf("have empty line\n");
                    Cache[j].valid = 1;
                    Cache[j].tag = tag;
                    //printf("Cache[j].data = %x, paddr - baddr = %x\n", (uint32_t) Cache[j].data, (uint32_t) paddr - baddr);
                    memcpy(Cache[j].data, (void*) (hw_mem + paddr - baddr), 64);
                    //printf("reach here");
                    break;                   
                }
                // full
                else if (j == start_line + 7)
                {
                    //printf("full\n");
                    uint32_t random = rand() % 8;
                    Cache[start_line + random].tag = tag;
                    memcpy(Cache[start_line + random].data, (void*) (hw_mem + paddr - baddr), 64);
                    break; 
                } 
            }
            // use recursion to cope with line crossing
            return cache_read(paddr, len, Cache);
        }
    }
    return 0;
}

void cache_write(paddr_t paddr, size_t len, uint32_t data, CacheLine * Cache)
{
    uint32_t tag; // 19 bits
    uint8_t grp_no, baddr; // 7bits and 6bits
    tag = (paddr >> 13) & 0x0007FFFF;
    grp_no = (paddr >> 6) & 0x7F;
    baddr = paddr & 0x3F;
    uint16_t start_line = ((uint16_t) grp_no) * 8;
    for (uint16_t i = start_line; i < start_line + 8; i++)
    {
        if (Cache[i].valid && Cache[i].tag == tag) // hit cache, write cache and memory
        {
            //printf("write hit cache\n");
            if (baddr + len <= 64) // not cross line
            {
                memcpy(&Cache[i].data[baddr], &data, len); // write cache
                memcpy(hw_mem + paddr, &data, len); // write memory
                return;
            }

            else // cross line
            {
                memcpy(&Cache[i].data[baddr], &data, 64 - baddr); // write cache
                memcpy(hw_mem + paddr, &data, 64 - baddr); // write memory
                cache_write(paddr + 64 - baddr, len + baddr - 64, (data >> ((64 - baddr) << 3) ), Cache); // recursion
                return;
            }
        }

        else if (i == start_line + 7) // not hit cache, only write memory
        {
            //printf("write not hit cache\n");
            memcpy(hw_mem + paddr, &data, len); // write memory
            return;
        }
    }
}