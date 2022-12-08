CC=gcc
CFLAGS=-c -Wall -Werror -g

all: tests

tests: main_mem_test_01
	./main_mem_test_01

main_mem_test_01: main_mem_test_01.o main_mem.o main_mem_log.o
	$(CC) -o main_mem_test_01 main_mem_test_01.o main_mem.o main_mem_log.o

main_mem_test_01.o: main_mem_test_01.c main_mem.h main_mem_log.h
	$(CC) $(CFLAGS) main_mem_test_01.c

main_mem.o: main_mem.c main_mem.h
	$(CC) $(CFLAGS) main_mem.c

main_mem_log.o: main_mem_log.c main_mem_log.h
	$(CC) $(CFLAGS) main_mem_log.c

dm_cache.o: dm_cache.c dm_cache.h main_mem.h
	$(CC) $(CFLAGS) dm_cache.c

fa_cache.o: fa_cache.c fa_cache.h main_mem.h
	$(CC) $(CFLAGS) fa_cache.c

sa_cache.o: sa_cache.c sa_cache.h main_mem.h
	$(CC) $(CFLAGS) sa_cache.c

clean:
	rm -f *.o main_mem_test_01 *.txt

