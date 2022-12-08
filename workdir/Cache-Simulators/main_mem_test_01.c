#include <stdio.h>
#include <stdint.h>
#include "main_mem.h"

int main() {

    uint32_t test_data[16] = {
        0x7cd27462, 0x68aeb834, 0x87517e9e, 0xd50448ee,
        0xe88f531f, 0x97c9bf51, 0x2b1469d5, 0x7721faef,
        0x84a66278, 0x132d8e54, 0xd5af965e, 0x0298ee88,
        0x27ee1026, 0x9ed0f871, 0xb8891f1f, 0xa671515f
    };
    
    MainMem *main_mem = createMainMem(6);
    if (main_mem == NULL) {
        printf("createMainMem failed\n");
        exit(-1);
    } 

    if (wordCount(main_mem) != 16) {
        printf("Unexpected word count.\n");
        exit(-1);
    }

    uint32_t value;
    for (uint32_t i=0; i<16; i++) {
        value = 0x1;

        if (readWord(main_mem, i*4, &value) != MM_SUCCESS) {
            printf("readWord error\n");
            exit(-1);
        }
        if (value != 0) {
            printf("Main memory contents not initialized to zero\n");
            exit(-1);
        }
    }

    if (readWord(main_mem, -1, &value) != MM_ADDRESS_OUT_OF_RANGE) {
        printf("Expected ADDRESS_OUT_OF_RANGE_ERROR\n");
        exit(-1);
    }

    if (readWord(main_mem, 64, &value) != MM_ADDRESS_OUT_OF_RANGE) {
        printf("Expected ADDRESS_OUT_OF_RANGE_ERROR\n");
        exit(-1);
    }

    if (readWord(main_mem, 3, &value) != MM_ADDRESS_MISALIGNED) {
        printf("Expected ADDRESS_MISALIGNED\n");
        exit(-1);
    }

    for (uint32_t i=0; i<16; i++) {
        value = test_data[i];

        if (writeWord(main_mem, i*4, value) != MM_SUCCESS) {
            printf("writeWord error\n");
            exit(-1);
        }
    }

    if (writeWord(main_mem, -1, value) != MM_ADDRESS_OUT_OF_RANGE) {
        printf("Expected ADDRESS_OUT_OF_RANGE_ERROR\n");
        exit(-1);
    }

    if (writeWord(main_mem, 64, value) != MM_ADDRESS_OUT_OF_RANGE) {
        printf("Expected ADDRESS_OUT_OF_RANGE_ERROR\n");
        exit(-1);
    }

    if (writeWord(main_mem, 3, value) != MM_ADDRESS_MISALIGNED) {
        printf("Expected ADDRESS_MISALIGNED\n");
        exit(-1);
    }

    if (writeMainMemToFile(main_mem, "main_mem_test_01-dump.txt") != MM_SUCCESS) {
        printf("Error writing contents of main memory to file");
    }

    writeLogToFile(main_mem->op_log, "main_mem_test_01-log.txt");

    freeMainMem(main_mem);

    main_mem = createMainMem(6);
    if (loadMainMemFromFile(main_mem, "main_mem_test_01-dump.txt") != MM_SUCCESS) {
        printf("loadMainMemFromFile failed\n");
        exit(-1);
    }

    for (uint32_t i=0; i<16; i++) {
        if (readWord(main_mem, i*4, &value) != MM_SUCCESS) {
            printf("readWord error\n");
            exit(-1);
        }
        if (value != test_data[i]) {
            printf("Main memory contents loaded from file does not match expected value\n");
            exit(-1);
        }
    }

    printf("Test 01 Finished\n");
}
