// PID: 730384155
// I pledge the COMP 211 honor code.
#include <stdio.h>
#include "fa_cache.h"

FACache *createFACache(MainMem *mem,
                     uint32_t word_index_bitcount,
                     uint32_t num_cache_lines) {
    if (mem == NULL){
        return NULL;
    }

    if (mem->address_width <= word_index_bitcount + 2){
        return NULL;
    }

    if (num_cache_lines == 0){
        return NULL;
    }

    FACache *cache = (FACache *) malloc(sizeof(FACache));
    if (cache == NULL) {
        return NULL;
    }

    FACacheLine *buff = (FACacheLine *) calloc(num_cache_lines, sizeof(FACacheLine));
    if (buff == NULL) {
        free(cache);
        return NULL;
    }

    for (uint32_t i=0; i<num_cache_lines; i++){
        buff[i].valid = 0;
        buff[i].use_identification = 0;
        buff[i].block = (uint32_t *) calloc((1<<word_index_bitcount), sizeof(uint32_t));
        if (buff[i].block == NULL){
            for (uint32_t k=0;k<i;k++) {
                free(buff[k].block);
            }
            free(buff);
            free(cache);
            return NULL;
        }
    }

    cache->word_index_bitcount = word_index_bitcount;
    cache->mem = mem;
    cache->num_cache_lines = num_cache_lines;
    cache->lines = buff;
    cache->use_count = 0;

    return cache;
    
}

void freeFACache(FACache *cache) {
    for (uint32_t i=0; i<cache->num_cache_lines; i++) {
        free(cache->lines[i].block);
    }
    free(cache->lines);
    free(cache);
}

uint32_t bit_select(uint32_t num, uint32_t startbit, uint32_t endbit) {
     uint32_t topmask = 0xffffffff;
    return (num >> endbit) & (~(topmask << (startbit-endbit+1)));
}

FACacheResult readByte(FACache *cache, uint32_t address, uint8_t *value) {
    if (cache == NULL) {
        return FA_INVALID_CACHE;
    }

    if (address > (1 << cache->mem->address_width)) {
        return FA_CACHE_ADDRESS_OUT_OF_RANGE;
    }

    if (value == NULL) {
        return FA_INVALID_VALUE_PTR;
    }

    FACacheLine *line = NULL;
    uint32_t least_used = 0;
    uint32_t addr_tag = address >> (cache->word_index_bitcount + 2);

    for (uint32_t i=0; i<cache->num_cache_lines;i++) {
        if (!cache->lines[i].valid){
            line = &cache->lines[i];
            break;
        }
        if (cache->lines[least_used].use_identification > cache->lines[i].use_identification){
            least_used = i;
        }
        if (addr_tag == cache->lines[i].tag){
            line = &cache->lines[i];
            break;
        }
    }
    if (line == NULL){
        line = &cache->lines[least_used];
    }
    if ((!line->valid) || (line->tag != addr_tag)) {
        // Line does not have the block we want. Go get it.
        
        uint32_t block_start_address = address & (0xffffffff << (cache->word_index_bitcount+2));
        uint32_t block_size = (1 << cache->word_index_bitcount);
        for (uint32_t i=0; i<block_size; i++) {
            if (readWord(cache->mem, block_start_address + (i*sizeof(uint32_t)), &(line->block[i])) != MM_SUCCESS) {
                return FA_UNIT_FAIL;
            }
        }
        line->valid = 1;
        line->tag = addr_tag;
   }

   uint32_t word_index = bit_select(address, 1+cache->word_index_bitcount, 2);
   uint32_t word = line->block[word_index];
   uint32_t byte_offset = address % sizeof(uint32_t);
   *value = ((word >> (8*byte_offset)) & 0x000000ff);
    line->use_identification = cache->use_count++;
    return FA_CACHE_SUCCESS;
}

FACacheResult writeByte(FACache *cache, uint32_t address, uint8_t value) {
    if (cache == NULL) {
        return FA_INVALID_CACHE;
    }

    if (address > (1 << cache->mem->address_width)) {
        return FA_CACHE_ADDRESS_OUT_OF_RANGE;
    }

    uint32_t addr_tag = address >> (2+cache->word_index_bitcount);
    uint32_t most_recently_used = 0;
    FACacheLine *line = NULL;
    for (uint32_t i = 0; i < cache->num_cache_lines; i++) {
        if (!cache->lines[i].valid) {
            line = &cache->lines[i];
            break;
        }
        if (cache->lines[most_recently_used].use_identification > cache->lines[i].use_identification) {
            most_recently_used = i;
        }
        if (addr_tag == cache->lines[i].tag) {
            line = &cache->lines[i];
            break;
        }
    }
    if (line == NULL) {
        line = &cache->lines[most_recently_used];
    }
    if (!line->valid || line->tag != addr_tag) {
        uint32_t block_start_address = address & (0xffffffff << (cache->word_index_bitcount + 2));
        for (uint32_t k = 0; k < (1 << cache->word_index_bitcount); k++) {
            if (readWord(cache->mem, block_start_address + (k*sizeof(uint32_t)), &(line->block[k])) != MM_SUCCESS) {
                return FA_UNIT_FAIL;
            }
        }
        line->valid = 1;
        line->tag = addr_tag;
    }
    uint32_t word_index = bit_select(address, 1+cache->word_index_bitcount, 2);
    uint32_t *word = &line->block[word_index];
    uint32_t byte_offset = address % sizeof(uint32_t);
    uint32_t newest_word = 0;

    for (uint32_t k = sizeof(uint32_t); k > 0; k--) {
        newest_word = (newest_word<<8);
        if (k-1 == byte_offset) {
            newest_word += value;
        } else {
            newest_word += bit_select(*word, 8*k-1, 8*(k-1));
        }
    }

    *word = newest_word;
    uint32_t word_addr = address & (0xffffffff << 2);
    if (writeWord(cache->mem, word_addr, newest_word) != MM_SUCCESS) {
        return FA_UNIT_FAIL;
    }

    line->use_identification = cache->use_count++;
    return FA_CACHE_SUCCESS;
}


