// PID: 730384155
// I pledge the COMP 211 honor code.
#ifndef FA_CACHE_H
#define FA_CACHE_H
#include <stdint.h>
#include "main_mem.h"

// FACache
// 
// Models a fully associative write through cache in front of a main memory model.
// Provides byte-level read interface. Replacement policy is least recently used.
typedef struct FACacheLine {
    uint32_t use_identification;
    uint32_t tag;
    uint32_t *block;
    uint32_t valid;
} FACacheLine;
typedef struct FACache {
    uint32_t word_index_bitcount;
    uint32_t num_cache_lines;
    uint32_t use_count;
    MainMem *mem;
    FACacheLine *lines;
} FACache;

// Enum for result codes returned by readByte

typedef enum {
    FA_CACHE_SUCCESS,
    FA_CACHE_ADDRESS_OUT_OF_RANGE,
    FA_INVALID_CACHE,
    FA_INVALID_VALUE_PTR,
    FA_UNIT_FAIL
} FACacheResult;

// createFACache
//
// Creates FACache for provided MainMem with specified configuration.
// 
// Overall address width specified in MainMem as mem->address_width.
// Word size is set at 4 bytes (i.e., bottom 2-bits of address is byte offset within word)
// Cache line block size is (1 << word_index_bitcount)
// Number of cache lines is specfiied as num_cache_lines.
// Feasability constraint which should be checked: 
//     mem->address_width > word_index_bit_count + 2 
//
// Returns pointer to allocated and initialized FACache structure or NULL on error.

FACache *createFACache(MainMem *mem,                // Underlying MainMem model. 
                   uint32_t word_index_bitcount,    // Number of word index bits
                   uint32_t num_cache_lines);       // Number of cache lines.

// freeFACache
// Frees the memory used by cache.
void freeFACache(FACache *cache);

// readByte
// Reads byte at address provided and returns result in value. 
// Returns one of the following FACacheResult symbols:
// FA_SUCCESS - returned when successful
// FA_INVALID_CACHE - returned if cache parameter is NULL
// FA_ADDRESS_OUT_OF_RANGE - returned if address not within range
// FA_INVALID_VALUE_PTR - returned if value parameter is NULL

FACacheResult readByte(FACache *cache, uint32_t address, uint8_t *value);

// writeByte
// Writes byte with value at address provided.
// Returns one of the following FACacheResult symbols:
// FA_SUCCESS - returned when successful
// FA_INVALID_CACHE - returned if cache parameter is NULL
// FA_ADDRESS_OUT_OF_RANGE - returned if address not within range

FACacheResult writeByte(FACache *cache, uint32_t address, uint8_t value);

#endif
