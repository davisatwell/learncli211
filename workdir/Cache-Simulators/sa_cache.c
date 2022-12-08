// PID: 730384155
// I pledge the COMP 211 honor code.
#include "sa_cache.h"

SACache *createSACache(MainMem *mem,
                     uint32_t set_index_bitcount,
                     uint32_t word_index_bitcount,
                     uint32_t cache_lines_per_set) {
    if (mem == NULL) {
        return NULL;
    }

    if (mem->address_width <= set_index_bitcount + word_index_bitcount +2) {
        return NULL;
    }

    SACache *cache = (SACache *) malloc(sizeof(SACache));
    if (cache == NULL) {
        return NULL;
    }

    SACacheSet *buffer = (SACacheSet *) calloc(1 << set_index_bitcount, sizeof(SACacheSet));
    if (buffer == NULL) {
        free(cache);
        return NULL;
    }

    for (uint32_t i = 0; i < (1 << set_index_bitcount); i++) {
       buffer[i].lines = (SACacheLine *) calloc(cache_lines_per_set, sizeof(SACacheLine));
       buffer[i].use_counter = 0;
       for (uint32_t j=0; j<cache_lines_per_set; j++) {
           buffer[i].lines[j].block = (uint32_t *) calloc((1<<word_index_bitcount), sizeof(uint32_t));
           buffer[i].lines[j].valid = 0;
           buffer[i].lines[j].updated = 0;
           buffer[i].lines[j].use_id = 0;
       }
    }

    cache->lines_per_set = cache_lines_per_set;
    cache->word_index_bitcount = word_index_bitcount;
    cache->set_index_bitcount = set_index_bitcount;
    cache->sets = buffer;
    cache->mem = mem;


    return cache;
}

void freeSACache(SACache *cache) {
    for (uint32_t i = 0; i<(1<<cache->set_index_bitcount); i++) {
        SACacheSet set = cache->sets[i];
        for (uint32_t j = 0; j<cache->lines_per_set; j++) {
            free(set.lines[j].block);
        }
        free(set.lines);
    }
    free(cache->sets);
    free(cache);
}

void writeBack(SACache *cache, uint32_t set_index, uint32_t line_index) {
    SACacheLine *line = &cache->sets[set_index].lines[line_index];
    for (uint32_t i = 0; i < (1<<cache->word_index_bitcount); i++) {
        uint32_t word_addr = (line->tag << (cache->set_index_bitcount+cache->word_index_bitcount+2)) + (set_index<<(cache->word_index_bitcount + 2)) + (i << 2);
        writeWord(cache->mem, word_addr, line->block[i]);
    }
}

uint32_t bit_select(uint32_t num, uint32_t startbit, uint32_t endbit) {
    num =  num << (32 - startbit - 1);
    num =  num >> (32 - startbit -1 + endbit);
    return num;
}

SACacheResult readByte(SACache *cache, uint32_t address, uint8_t *value) {
    if (cache == NULL) {
        return SA_INVALID_CACHE;
    }

    if (value == NULL) {
        return SA_INVALID_VALUE_PTR;
    }

    if (address > (1<<cache->mem->address_width)) {
        return SA_CACHE_ADDRESS_OUT_OF_RANGE;
    }

    uint32_t addr_tag = address >> (cache->set_index_bitcount + cache->word_index_bitcount + 2);

    uint32_t set_index_start = 1 + cache->set_index_bitcount + cache->word_index_bitcount;
    uint32_t set_index_end = 2 + cache->word_index_bitcount;
    uint32_t set_index = bit_select(address, set_index_start, set_index_end);

    SACacheSet *set = &(cache->sets[set_index]);

    SACacheLine *line = NULL;
    uint32_t least_recently_used = 0;
    for (uint32_t i = 0; i < cache->lines_per_set; i++) {
        if (!set->lines[i].valid) {
            line = &set->lines[i];
            break;
        }
        if (set->lines[least_recently_used].use_id > set->lines[i].use_id) {
            least_recently_used = i;
        }
        if (addr_tag == set->lines[i].tag) {
            line = &set->lines[i];
            break;
        }
    }
    if (line == NULL) {
        if (cache->sets[set_index].lines[least_recently_used].updated) {
            writeBack(cache, set_index, least_recently_used);
        }
        line = &set->lines[least_recently_used];
    }


    if ((!line->valid) || (line->tag != addr_tag)) {
        uint32_t block_addr_start = address & (0xffffffff << (cache->word_index_bitcount + 2));

        for (uint32_t i = 0; i < (1 << cache->word_index_bitcount); i++) {
                if(readWord(cache->mem, block_addr_start + (i*sizeof(uint32_t)), &(line->block[i])) != MM_SUCCESS) {

                   return SA_UNIT_FAIL;
                } 
        } 
        line->valid = 1;
        line->tag = addr_tag; 
        line->updated = 0;
    } 
    
    uint32_t word_index = bit_select(address, cache->word_index_bitcount+1, 2);
    uint32_t word = line->block[word_index];

    uint32_t byte_offset = address % sizeof(uint32_t);
    *value = ((word>>(8*byte_offset)) & 0x000000ff);

    line->use_id = set->use_counter++;

    return SA_CACHE_SUCCESS;
}

SACacheResult writeByte(SACache *cache, uint32_t address, uint8_t value) {
    if (cache == NULL) {
        return SA_INVALID_CACHE;
    }

    if (address > (1<<cache->mem->address_width)) {
        return SA_CACHE_ADDRESS_OUT_OF_RANGE;
    }

    uint32_t addr_tag = address >> (cache->set_index_bitcount + cache->word_index_bitcount + 2);

    uint32_t set_index_start = 1 + cache->set_index_bitcount + cache->word_index_bitcount;
    uint32_t set_index_end = 2 + cache->word_index_bitcount;
    uint32_t set_index = bit_select(address, set_index_start, set_index_end);

    SACacheSet *set = &(cache->sets[set_index]);

    SACacheLine *line = NULL;
    uint32_t least_recently_used = 0;
    for (uint32_t i = 0; i < cache->lines_per_set; i++) {
        if (!set->lines[i].valid) {
            line = &set->lines[i];
            break;
        }
        if (set->lines[least_recently_used].use_id > set->lines[i].use_id) {
            least_recently_used = i;
        }
        if (addr_tag == set->lines[i].tag) {
            line = &set->lines[i];
            break;
        }
    }
    if (line == NULL) {
        if (cache->sets[set_index].lines[least_recently_used].updated) {
            writeBack(cache, set_index, least_recently_used);
        }
        line = &set->lines[least_recently_used];
    }


    if ((!line->valid) || (line->tag != addr_tag)) {
        uint32_t block_addr_start = address & (0xffffffff << (cache->word_index_bitcount + 2));

        for (uint32_t i = 0; i < (1 << cache->word_index_bitcount); i++) {
                if(readWord(cache->mem, block_addr_start + (i*sizeof(uint32_t)), &(line->block[i])) != MM_SUCCESS) {

                   return SA_UNIT_FAIL;
                } 
        } 
        line->valid = 1;
        line->tag = addr_tag; 
        line->updated = 0;
    } 

    uint32_t word_index = bit_select(address, cache->word_index_bitcount+1, 2);
    uint32_t *word = &line->block[word_index];

    uint32_t byte_offset = address % sizeof(uint32_t);
    uint32_t new_word = 0;
    for (uint32_t j = sizeof(uint32_t); j > 0; j--) {
        new_word = new_word<<8;
        if (j-1 == byte_offset) {
            new_word += value;
        } else {
            new_word += bit_select(*word, 8*j-1, 8*(j-1));
        }
    }
    *word = new_word;
    line->use_id = set->use_counter++;
    line->updated = 1;
    return SA_CACHE_SUCCESS;
}

void flushCache(SACache *cache) {
    for (uint32_t i = 0; i < (1<<cache->set_index_bitcount); i++) {
        SACacheSet *set = &cache->sets[i];
        for (uint32_t j = 0; j < cache->lines_per_set; j++) {
            if (set->lines[j].updated) {
                writeBack(cache, i, j);
            }
            set->lines[j].valid = 0;
            set->lines[j].updated = 0;
            set->lines[j].use_id = 0;
        }
        set->use_counter = 0;
    }
}

