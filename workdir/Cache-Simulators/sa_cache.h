// PID: 730384155
// I pledge the COMP 211 honor code.
#ifndef SA_CACHE_H
#define SA_CACHE_H
#include <stdint.h>
#include "main_mem.h"

// SACache
// 
// Models a set associative write back cache in front of a main memory model.
// Provides byte-level read/write interface. Replacement policy is least recently used.

typedef struct {
    uint32_t tag;
    uint32_t use_id;
    uint8_t valid;
    uint8_t updated;
    uint32_t *block;
} SACacheLine;

typedef struct {
    uint32_t use_counter;
    SACacheLine *lines;
} SACacheSet;

typedef struct SACache {
    uint32_t word_index_bitcount;
    uint32_t set_index_bitcount;
    uint32_t lines_per_set;
    MainMem *mem;
    SACacheSet *sets;
} SACache;

// Enum for result codes returned by readByte

typedef enum {
    SA_CACHE_SUCCESS,
    SA_CACHE_ADDRESS_OUT_OF_RANGE,
    SA_INVALID_CACHE,
    SA_INVALID_VALUE_PTR,
    SA_UNIT_FAIL
} SACacheResult;

// createSACache
//
// Creates SACache for provided MainMem with specified configuration.
// 
// Overall address width specified in MainMem as mem->address_width.
// Word size is set at 4 bytes (i.e., bottom 2-bits of address is byte offset within word)
// Cache line block size is (1 << word_index_bitcount)
// Number of sets is (1 << set_index_bitcount)
// Number of cache lines per set is specfiied as cache_lines_per_set.
// Feasability constraint which should be checked: 
//     mem->address_width > set_index_bitcount + word_index_bitcount + 2 
//
// Returns pointer to allocated and initialized FACache structure or NULL on error.

SACache *createSACache(MainMem *mem,                // Underlying MainMem model. 
                   uint32_t set_index_bitcount,     // Number of set index bits 
                   uint32_t word_index_bitcount,    // Number of word index bits
                   uint32_t cache_lines_per_set);   // Number of cache lines per set

// freeSACache
// Frees the memory used by cache.
void freeSACache(SACache *cache);

// readByte
// Reads byte at address provided and returns result in value. 
// Returns one of the following SACacheResult symbols:
// SA_SUCCESS - returned when successful
// SA_INVALID_CACHE - returned if cache parameter is NULL
// SA_ADDRESS_OUT_OF_RANGE - returned if address not within range
// SA_INVALID_VALUE_PTR - returned if value parameter is NULL

SACacheResult readByte(SACache *cache, uint32_t address, uint8_t *value);

// writeByte
// Writes byte as provided value at address. 
// Returns one of the following SACacheResult symbols:
// SA_SUCCESS - returned when successful
// SA_INVALID_CACHE - returned if cache parameter is NULL
// SA_ADDRESS_OUT_OF_RANGE - returned if address not within range

SACacheResult writeByte(SACache *cache, uint32_t address, uint8_t value);

// flushCache
// Writes back any cache lines with pending changes to main memory and 
// invalidates all cache lines.

void flushCache(SACache *cache);

void writeBack(SACache *cache, uint32_t set_index, uint32_t line_index);

#endif