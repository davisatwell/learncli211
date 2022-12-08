// PID: 730384155
// I pledge the COMP 211 honor code.
#include "dm_cache.h"

DMCache *createDMCache(MainMem *mem,
                     uint32_t set_index_bitcount,
                     uint32_t word_index_bitcount) {
    if (mem == NULL) {
        return NULL;
    }

    if (mem->address_width <= (set_index_bitcount + word_index_bitcount + 2)){
        return NULL;
    }
    
    DMCache *cache = (DMCache *) malloc(sizeof(DMCache));

    if (cache == NULL){
        return NULL;
    }

    uint32_t num_lines = (1<<set_index_bitcount);
    cache->lines = (DMCacheLine *) calloc(num_lines, sizeof(DMCacheLine));
    
    if (cache->lines == NULL){
        free(cache);
    }

    for (uint32_t i=0; i<num_lines; i++){
        cache->lines[i].block = (uint32_t *) calloc((1<<word_index_bitcount), sizeof(uint32_t));

        if (cache->lines[i].block == NULL){
            for (uint32_t k=0; k<i;k++) {
                free(cache->lines[i].block);
            }
            free(cache->lines);
            free(cache);
            return NULL;
        }
        cache->lines[i].valid = 0;
    }
    cache->word_index_bitcount = word_index_bitcount;
    cache->set_index_bitcount = set_index_bitcount;
    cache->mem = mem;
    cache->lines = cache->lines;

    return cache;
}

void freeDMCache(DMCache *cache) {
    for (uint32_t i=0; i<(1<<cache->set_index_bitcount);i++){
        free(cache->lines[i].block);
    }
    free(cache->lines);
    free(cache);
}

uint32_t bit_select(uint32_t num, uint32_t startbit, uint32_t endbit) {
     uint32_t topmask = 0xffffffff;
    return (num >> endbit) & (~(topmask << (startbit-endbit+1)));
}

DMCacheResult readByte(DMCache *cache, uint32_t address, uint8_t *value) {
    if (cache == NULL) {
        return DM_INVALID_CACHE;
    }

    if (address >= (1 << cache->mem->address_width)) {
        return DM_CACHE_ADDRESS_OUT_OF_RANGE;
    }

    if (value == NULL) {
        return DM_INVALID_VALUE_PTR;
    }
    uint32_t line_index_start = 1+cache->word_index_bitcount+cache->set_index_bitcount;
    uint32_t line_index_end = 2+cache->word_index_bitcount;
    uint32_t line_index = bit_select(address, line_index_start, line_index_end);

    DMCacheLine *line = &cache->lines[line_index];
    
    uint32_t addr_tag = address >> (cache->set_index_bitcount + cache->word_index_bitcount + 2);

    if ((!line->valid) || (line->tag != addr_tag)) {
        // Line does not have the block we want. Go get it.
        
        uint32_t block_start_address = address & (0xffffffff << (cache->word_index_bitcount+2));
        uint32_t block_size = (1 << cache->word_index_bitcount);
        for (uint32_t i=0; i<block_size; i++) {
            if (!(readWord(cache->mem, block_start_address + (i*sizeof(uint32_t)), &(line->block[i])) == MM_SUCCESS)) {
                return DM_UNIT_FAIL;
            }
        }
        line->valid = 1;
        line->tag = addr_tag;
   }
    
   uint32_t word_index = bit_select(address, 2+cache->word_index_bitcount-1, 2);
   uint32_t word = line->block[word_index];

   uint32_t byte_offset = address & 0x3;

   *value = ((word >> (8*byte_offset)) & 0x000000ff);

   return DM_CACHE_SUCCESS;
}



