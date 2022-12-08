# Assignment 05

In this assignment, you'll develop three different versions of
a cache simulator:

* Direct Mapped Read-Only Cache

  This version only needs to support reading bytes from an address.
  The number of set index bits determines the number of cache lines as each
  set only has one line. No replacement policy is specified because there is
  no choice for which line to replace. 

* Fully Associative Write Through Cache

  This version supports both reading and writing bytes from an address.
  There is only one set of cache lines so the number of set index bits is known
  to be zero. The number of cache lines is specified when the cache is created.
  The replacement policy should be "least recently used". That means that if an
  empty cache line is not available, replace the cache line that was least recently
  used for either a read or write operation. HINT: use a global counter to keep track
  of how many read/write operations have occured and use that as a sort of "clock" for 
  keeping track of when a cache line was last used. This cache should implement 
  "write through" meaning that any write operations are immediately pushed to the main
  memory when they occur. Note, that only the word that contains the byte written should
  be updated in main memory (not the entire cache line).

* Set Associative Write Back Cache

  This version implements a general set-associative cache with reading and writing
  with "write back". Within a set of cache lines, least recently used is the replacement
  policy. Write back means that when a byte is written, it is only written to the cache line
  without immediately updating main memory. A "dirty" cache line is written back in its entirety
  to the main memory when it is replaced. This version also supports "flushing" the cache which
  means writing back any dirty cache lines and invalidating all cache lines to reset the cache
  to empty.

## Assignment Structure

The following files are used to implement the underlying main memory. Do not edit or change:
* main_mem.h
* main_mem.c
* main_mem_log.c
* main_mem_log.h

The main memory implementation logs all read/write operations. The contents of main memory can be dumped/loaded
into a file using the functions *writeMainMemToFile* and *loadMainMemFromFile*. The log can be written out to a file
using the function *writeLogToFile*.

The direct mapped cache should be implemented in dm_cache.h and dm_cache.c. You may (and in fact will need to)
define structures needed by your implementation and possibly add new functions that are part of your implementation.
You are free to change whatever you want as long as you don't change the declarations for *createDMCache*, *freeDMCache*, or *readByte*.

Similarly, the fully associative cache should be implemented in fa_cache.h and fa_cache.c. Again, feel free to
change / add things as necessary as long as you do not change the declarations of *createFACache*, *freeFACache*, *readByte*, or *writeByte*.

The general set associative cache should be implemented in sa_cache.h and sa_cache.c. The functions that should
remain as declared include *createSACache*, *freeSACache*, *readByte*, *writeByte*, and *flushCache*.

## Grading

There will be two different Gradescope assignments set up to turn in. One for the direct mapped and fully associative cache
implementations and one for the general set associative cache implementation. The general set associate cache 
implementation is a bonus assignment that can be done for extra credit and is not required. As with prior assignments,
20% of the points will go to code style and 80% of the points will go toward functionality.
 
