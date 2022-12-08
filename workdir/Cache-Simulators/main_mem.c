#include <stdlib.h>
#include <stdio.h>
#include "main_mem.h"

//----------------------
// createMainMem
//
// Arguments: address_width - width in bits of main memory address
//
// Results: If successful, returns a pointer to initialized 
//          MainMem structure with 2^address_width bytes 
//          stored as 4-byte words.
//
//          NULL on error.
//

MainMem *createMainMem(uint32_t address_width) {
    if (address_width < 2 || address_width > 32) {
        return NULL;
    }

    MainMem *main_mem = (MainMem *) malloc(sizeof(MainMem));
    if (main_mem == NULL) {
        return NULL;
    }
    
    uint32_t num_words = (1 << address_width) / sizeof(uint32_t);
    uint32_t *buffer = (uint32_t *) calloc(num_words, sizeof(uint32_t));
    if (buffer == NULL) {
        free(main_mem);
        return NULL;
    }

    MainMemOpLog *log = createMainMemOpLog(num_words);
    if (log == NULL) {
        free(buffer);
        free(main_mem);
        return NULL;
    }

    main_mem->address_width = address_width;
    main_mem->memory = buffer;
    main_mem->op_log = log;
   return main_mem;
} 
    
//----------------------
// freeMainMem
//
// Arguments: mem - Pointer to MainMem structure to free
//
// Results: None. Memory associated with structure and its
//          components are free'd.
//
void freeMainMem(MainMem *mem) {
    if (mem != NULL) {
        if (mem->memory != NULL) {
            free(mem->memory);
            mem->memory = NULL;
        }
        if (mem->op_log != NULL) {
            freeMainMemOpLog(mem->op_log);
            mem->op_log = NULL;
        }
        free(mem);
    }
}

//----------------------
// wordCount
//
// Arguments: mem - pointer to valid, initialized MainMem structure 
//
// Results: number of words of main memory
//          
uint32_t wordCount(MainMem *mem) {
    return (1 << mem->address_width) / sizeof(uint32_t);
}

//----------------------
// loadMainMemFromFile
//
// Arguments: mem - pointer to MainMem structure
//            file_name - name of file to load
//
// Results: One of the following MainMemResult symbols,
//          SUCCESS - contents of file read as hexadecimal
//                    words loaded into MainMem structure
//                    and operation log is reset.
//          INVALID_MAIN_MEM - mem passed in was NULL
//          INVALID_FILE_NAME - file_name is NULL or 
//                              error opening specified file
//          LOAD_READ_ERROR - error encountered in file format
//
MainMemResult loadMainMemFromFile(MainMem *mem, char *file_name) {
    if (mem == NULL) {
        return MM_INVALID_MAIN_MEM;
    }

    if (file_name == NULL) {
        return MM_INVALID_FILE_NAME;
    }

    FILE *fhnd;

    fhnd = fopen(file_name, "r");

    if (fhnd == NULL) {
        return MM_INVALID_FILE_NAME;
    }

    uint32_t num_words = (1 << mem->address_width) / sizeof(uint32_t);

    for (uint32_t i=0; i<num_words; i++) {
        if (fscanf(fhnd, "%x", &(mem->memory[i])) != 1) {
            fclose(fhnd);
            return MM_LOAD_READ_ERROR;
        }
    }

    fclose(fhnd);

    clearLog(mem->op_log);
    return MM_SUCCESS;
}

// ----------------------------
// writeMainMemToFile
//
// Arguments: mem - reference to valid MainMem structure
//            file_name - name of main memory file to create
//
// Result:    SUCCESS (main memory structure written to file)
//            INVALID_MAIN_MEM (mem is NULL)
//            INVALID_FILE_NAME (file_name is null or file cannot be created)

MainMemResult writeMainMemToFile(MainMem *mem, char *file_name) {
    if (mem == NULL) {
        return MM_INVALID_MAIN_MEM;
    }

    if (file_name == NULL) {
        return MM_INVALID_FILE_NAME;
    }
    
    FILE *fhnd = fopen(file_name, "w");
    if (fhnd == NULL) {
        return MM_INVALID_FILE_NAME;
    }

    uint32_t num_words = (1 << mem->address_width) / sizeof(uint32_t);

    for (uint32_t i=0; i<num_words; i++) {
        fprintf(fhnd, "%x\n", mem->memory[i]);
    }

    fclose(fhnd);

    return MM_SUCCESS;
}


// ----------------------------
// readWord
//
// Arguments: mem - reference to a valid MainMem structure
//            address - address of word to read
//            value - pointer to value updated with read value
//            
// Result:    SUCCESS (value updated with word from main memory,
//                     and operation logged)
//            INVALID_MAIN_MEM (mem reference is  NULL)
//            ADDRESS_OUT_OF_RANGE (address is < 0 or >= 2^address_width
//            INVALID_VALUE (value reference is NULL)
//            ADDRESS_MISALIGNED (address not a multiple of word size)
//
MainMemResult readWord(MainMem *mem, uint32_t address, uint32_t *value) {

    if (mem == NULL) {
        return MM_INVALID_MAIN_MEM;
    }

    if (address >= (1 << mem->address_width)) {
        return MM_ADDRESS_OUT_OF_RANGE;
    }

    if (address % sizeof(uint32_t) != 0) {
        return MM_ADDRESS_MISALIGNED;
    }

    if (value == NULL) {
        return MM_INVALID_VALUE;
    }

    uint32_t word_index = address / sizeof(uint32_t);
    *value =  mem->memory[word_index];
    logOperation(mem->op_log, READ_OP, word_index, *value);

    return MM_SUCCESS;
}

// ----------------------------
// writeWord
//
// Arguments: mem - reference to a valid MainMem structure
//            address - address of word to write
//            value - value to write at the specified address
//
// Result:    SUCCESS (value in main memory updated, and
//                     operation logged)
//            INVALID_MAIN_MEM (mem reference is NULL)
//            ADDRESS_OUT_OF_RANGE (address is < 0 or >= 2^address_width
//            ADDRESS_MISALIGNED (address not a multiple of word size)
//
MainMemResult writeWord(MainMem *mem, uint32_t address, uint32_t value) {

    if (mem == NULL) {
        return MM_INVALID_MAIN_MEM;
    }

    if (address < 0 || address >= (1 << mem->address_width)) {
        return MM_ADDRESS_OUT_OF_RANGE;
    }

    if (address % sizeof(uint32_t) != 0) {
        return MM_ADDRESS_MISALIGNED;
    }

    uint32_t word_index = address / sizeof(uint32_t);
    mem->memory[word_index] = value;
    logOperation(mem->op_log, WRITE_OP, word_index, value);

    return MM_SUCCESS;
}

