#ifndef MAIN_MEM_H
#define MAIN_MEM_H
#include <stdint.h>
#include "main_mem_log.h"

// MainMem
// 
// Models a main memory organized as an addressable sequence
// of 4 byte words. Read and write operations must be word aligned.
// Operations are recorded in a log (see MainMemLog)

typedef struct MainMem {
    uint32_t address_width;  // Address width in bits
    uint32_t *memory;        // Memory as an array of 32-bit words
    MainMemOpLog *op_log;    // Operation log tracking read/write operations
} MainMem;

// Symbols used by functions that return MainMemResult type.
typedef enum {MM_SUCCESS, 
              MM_ADDRESS_OUT_OF_RANGE, 
              MM_INVALID_VALUE, 
              MM_INVALID_MAIN_MEM, 
              MM_INVALID_FILE_NAME, 
              MM_LOAD_READ_ERROR, 
              MM_ADDRESS_MISALIGNED
} MainMemResult;

// Allocates and returns new MainMem structure for provided address width
MainMem *createMainMem(uint32_t address_width);

// Frees MainMem struct
void freeMainMem(MainMem *mem);

// Returns number of words in address space (i.e., 1 << address_width / sizeof(uint32_t)
uint32_t wordCount(MainMem *mem);

// Reads word from specified address, returned in value
MainMemResult readWord(MainMem *mem, uint32_t address, uint32_t *value);

// Writes word provided as value at specified address
MainMemResult writeWord(MainMem *mem, uint32_t address, uint32_t value);

// Updates contents of MainMem from data in specified file. Resets log.
MainMemResult loadMainMemFromFile(MainMem *mem, char *file_name);

// Writes contents of MainMem to specified file in format read by loadMainMemFromFile.
MainMemResult writeMainMemToFile(MainMem *mem, char *file_name);


#endif
