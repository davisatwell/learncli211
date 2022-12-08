// PID: 730384155
// I pledge the COMP 211 honor code.
#ifndef DM_CACHE_H
#define DM_CACHE_H
#include <stdint.h>
#include "main_mem.h"

// DMCache
// 
// Models a direct mapped read-only cache in front of a main memory model.
// Provides byte-level read interface.
typedef struct DMCacheLine {
    uint32_t valid;
    uint32_t tag;
    uint32_t *block;
} DMCacheLine;

typedef struct DMCache {
    uint32_t word_index_bitcount;
    uint32_t set_index_bitcount;
    MainMem *mem;
    DMCacheLine *lines;
} DMCache;

// Enum for result codes returned by readByte

typedef enum {
    DM_CACHE_SUCCESS,
    DM_CACHE_ADDRESS_OUT_OF_RANGE,
    DM_INVALID_CACHE,
    DM_INVALID_VALUE_PTR,
    DM_UNIT_FAIL
} DMCacheResult;

// createDMCache
//
// Creates DMCache for provided MainMem with specified configuration.
// 
// Overall address width specified in MainMem as mem->address_width.
// Word size is set at 4 bytes (i.e., bottom 2-bits of address is byte offset within word)
// Cache line block size is (1 << word_index_bitcount)
// Number of cache lines (same as number of sets because this is a direct mapped cache)
// is (1 << set_index_bitbount)
// Feasability constraint which should be checked: 
//     mem->address_width > set_index_bitcount + word_index_bit_count + 2 
//
// Returns pointer to allocated and initialized DMCache structure or NULL on error.

DMCache *createDMCache(MainMem *mem,                // Underlying MainMem model. 
                   uint32_t set_index_bitcount,     // Number of set index bits 
                   uint32_t word_index_bitcount);   // Number of word index bits

// freeDMCache
// Frees the memory used by cache.
void freeDMCache(DMCache *cache);
// readByte
// Reads byte at address provided and returns result in value. 
// Returns one of the following DMCacheResult symbols:
// DM_SUCCESS - returned when successful
// DM_INVALID_CACHE - returned if cache parameter is NULL
// DM_ADDRESS_OUT_OF_RANGE - returned if address not within range
// DM_INVALID_VALUE_PTR - returned if value parameter is NULL

DMCacheResult readByte(DMCache *cache, uint32_t address, uint8_t *value);

#endif
