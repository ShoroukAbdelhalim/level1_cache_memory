# level1_cache_memory
	Level 1 cache memory is a simulator of a cache system of one level. It simulates different types of caches: 
Dirrect mapping, Set-associative and full associative caches. It also implements different types of replacements 
policies: LRU, FIFO and Random replacement policies. In addition, It considers the different writing moods: write 
through with write-allocation, write through with non-write-allocation and write back with write allocation.


# Usage
	Level 1 cache memory reads the instruction traces from a file, and simulates a simplified model of a cache 
that generates memory requests to the first level cache. Each line in the trace file represents a memory request, and 
can have the following format:

<read/write>:<addr>


# Building and Running MCsim - Standalone
	Level 1 cache memory requires a C++11 compiler. To build an optimized MCsim simply follow:

 $ cd level1_cache_memory/src
 $ g++ cache_memory.cpp funtions.cpp functions.h -o main
 $ ./main.exe -b <# bytes in cache line> -a <# associativity> -c <cache size> -r <l,f,r replacement policies> -w <t,T,b writing policies>
