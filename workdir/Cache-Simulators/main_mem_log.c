#include <stdio.h>
#include "main_mem_log.h"

//----------------------------
// createMainMemOpLog
//
// Arguments: none.
//
// Result: If successful, returns pointer to allocated and 
//         initialized MainMemOpLog structure
//
//         NULL on error.
// 
MainMemOpLog *createMainMemOpLog(uint32_t word_count) {
    MainMemOpLog *op_log = (MainMemOpLog *) malloc(sizeof(MainMemOpLog));
    if (op_log == NULL) {
        return NULL;
    }

    op_log->logSize = INIT_LOG_SIZE;
    op_log->nextIdx = 0;
    op_log->entries = (MainMemOpLogEntry *) calloc(op_log->logSize, sizeof(MainMemOpLogEntry));

    if (op_log->entries == NULL) {
        free(op_log);
        return NULL;
    }

    op_log->wordCount = word_count;
    op_log->readCounts = (uint32_t *) calloc(word_count, sizeof(uint32_t));

    if (op_log->readCounts == NULL) {
        free(op_log->entries);
        free(op_log);
        return NULL;
    }

    op_log->writeCounts = (uint32_t *) calloc(word_count, sizeof(uint32_t));

    if (op_log->writeCounts == NULL) {
        free(op_log->readCounts);
        free(op_log->entries);
        free(op_log);
        return NULL;
    }
    return op_log;
}

//------------------------
// freeMainMemOpLog
//
// Arguments: op_log - pointer to MainMemOpLog structure to free
//
// Result: None. Structure and its components are free'd.
//
void freeMainMemOpLog(MainMemOpLog *op_log) {
    if (op_log != NULL) {
        if (op_log->entries != NULL) {
            free(op_log->entries);
        }
        if (op_log->readCounts != NULL) {
            free(op_log->readCounts);
        }
        if (op_log->writeCounts != NULL) {
            free(op_log->writeCounts);
        }
        free(op_log);
    }
}

//------------------------
// logOperation
//
// Arguments: op_log - pointer to MainMemOpLog structure
//            op_type - operation to log (i.e., READ_OP or WRITE_OP)
//            word_index - word affected by operation
//            value - value read/written from/to word
//
// Result: None. Operation is logged. Size of log is automatically
//         increased if necessary. Read or write count updated.
//            
void logOperation(MainMemOpLog *op_log, MemOp op_type, uint32_t word_index, uint32_t value) {
    if (op_log->nextIdx == op_log->logSize) {
        op_log->logSize += LOG_INCREMENT_SIZE;
        op_log->entries = (MainMemOpLogEntry *) realloc(op_log->entries, 
                op_log->logSize * sizeof(MainMemOpLogEntry)); 
    } 

    op_log->entries[op_log->nextIdx].op = op_type;
    op_log->entries[op_log->nextIdx].wordIndex = word_index;
    op_log->entries[op_log->nextIdx].value = value;
    op_log->nextIdx++;

    if (op_type == READ_OP) {
        op_log->readCounts[word_index]++;
    } else {
        op_log->writeCounts[word_index]++;
    }
}

//--------------------------
// clearLog
//
// Arguments: op_log - pointer to MainMemOpLog structure to clear
//
// Results: None. Structure is cleared by resetting index of next
//          operation to log to zero and clearing read/write counts.
//
void clearLog(MainMemOpLog *op_log) {
    if (op_log != NULL) {
        op_log->nextIdx = 0;
    }
    for (uint32_t i=0; i<op_log->wordCount; i++) {
        op_log->readCounts[i] = 0;
        op_log->writeCounts[i] = 0;
    }
}

//-----------------------
// writeLogToFile
//
// Arguments: op_log - pointer ot MainMemOpLog structure
//            file_name - file name to write logged operations to
//
// Results: None. Description of logged operations written to
//          specified file.
void writeLogToFile(MainMemOpLog *op_log, char *file_name) {
    if (op_log == NULL) {
        return;
    }

    if (file_name == NULL) {
        return;
    }

    FILE *file = fopen(file_name, "w");

    fprintf(file, "Operation Log: \n");
    for (uint32_t i=0; i<op_log->nextIdx; i++) {
        fprintf(file, "%s word %d as value 0x%x\n", 
                op_log->entries[i].op == READ_OP ? "READ" : "WRITE",
                op_log->entries[i].wordIndex,
                op_log->entries[i].value);
    }

    uint32_t readTotal = 0;
    uint32_t writeTotal = 0;

    fprintf(file, "\n");
    fprintf(file, "Operation Counts: READ, WRITE\n");
    for (uint32_t i=0; i < op_log->wordCount; i++) {
        fprintf(file, "%d, %d\n", op_log->readCounts[i], op_log->writeCounts[i]);
        readTotal += op_log->readCounts[i];
        writeTotal += op_log->writeCounts[i];
    }

    fprintf(file, "\n");
    fprintf(file, "Total reads %d\n", readTotal);
    fprintf(file, "Total writes %d\n", writeTotal);

    fclose(file);
}

