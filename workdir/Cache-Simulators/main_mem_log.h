#ifndef MAIN_MEM_LOG_H
#define MAIN_MEM_LOG_H
#include <stdint.h>
#include <stdlib.h>

// Enum symbols for operation to log
typedef enum {READ_OP, WRITE_OP} MemOp;

// Log entry structure
typedef struct MainMemOpLogEntry {
    MemOp op;
    uint32_t wordIndex;
    uint32_t value;
} MainMemOpLogEntry;

// Log structure
typedef struct MainMemOpLog {
    uint32_t logSize;
    uint32_t nextIdx;
    MainMemOpLogEntry *entries;
    uint32_t wordCount;
    uint32_t *readCounts;
    uint32_t *writeCounts;
} MainMemOpLog;

// Initial size of log entries array
#define INIT_LOG_SIZE 100

// Increment size when extending log entries array
#define LOG_INCREMENT_SIZE 250

// Allocates and returns a pointer to MainMemOpLog structure
MainMemOpLog *createMainMemOpLog(uint32_t word_count);

// Frees memory associatd with MainMemOpLog structure
void freeMainMemOpLog(MainMemOpLog *op_log);

// Logs operation given operation type, word index, and value read/written
void logOperation(MainMemOpLog *op_log, MemOp op_type, uint32_t word_index, uint32_t value);

// Resets log, clears read/write counts
void clearLog(MainMemOpLog *op_log);

// Writes logged information to specified file.
void writeLogToFile(MainMemOpLog *op_log, char *file_name);


#endif
