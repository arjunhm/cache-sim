## Cache Simulator
Configurable cache simulator that employs LRU eviction policy to output the number of hits, misses, and evictions.  
### Files
```
csim.c       Your cache simulator  
Makefile     Builds the simulator and tools  
README       This file  
cachelab.c   Required helper functions  
cachelab.h   Required header file  
csim-ref*    The executable reference cache simulator  
test-csim*   Tests your cache simulator  
traces/      Trace files used by test-csim.c  
```
### Usage

Compile your code:  
    `make csim`  

Check the correctness of your simulator:  
    `./test-csim`  

Usage:
    `./csim [-hv] -s <num> -E <num> -b <num> -t <file>`
    
Options:
```
  -h         Print this help message.
  -v         Optional verbose flag.
  -s <num>   Number of set index bits.
  -E <num>   Number of lines per set.
  -b <num>   Number of block offset bits.
  -t <file>  Trace file.
```

Examples:  
  `./csim -s 4 -E 1 -b 4 -t traces/yi.trace`  
  `./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace`  

### Tracefiles

Valgrind memory traces have the following form:
```
I 0400d7d4,8
 M 0421c7f0,4
 L 04f6b868,8
 S 7ff0005c8,8
```

Each line denotes one or two memory accesses. The format of each line is  
`[space]operation address,size`

**Operations**  
`L` a data load  
`S` a data store  
`M` a data modify (data load + data store)  

**address** specifies a 64-bit hexadecimal memory address.   
**size** specifies the number of bytes accessed by the operation  

