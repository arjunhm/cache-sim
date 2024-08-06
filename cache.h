#include <stdbool.h>
#include <stdint.h>

#define u8 uint8_t
#define u32 uint32_t
#define u64 uint64_t
#define ADDRESS_WIDTH 64 // bits

typedef struct Block {
  u8 data;
} Block;

typedef struct Line {
  bool valid;
  u64 tag;
  u64 timestamp;
  // Block *blocks;
} Line;

typedef struct Set {
  Line *lines;
  u32 line_count; // number of lines
} Set;

typedef struct Stats {
  u32 hits;
  u32 misses;
  u32 evictions;
} Stats;

typedef struct Cache {
  Set *sets;

  // config
  u32 set_count;
  u32 set_bits;
  u32 block_count;
  u32 block_bits;
  u32 tag_bits;

  Stats *stats;
} Cache;

// init
void *cache_new(uint32_t set_count, uint32_t line_count, uint32_t block_count);
void cache_free(Cache *c);
// display
void display(Cache *c);
void display_stats(Stats *s);
// helper
u64 get_block_offset(Cache *c, u64 address);
u64 get_set_index(Cache *c, u64 address);
u64 get_tag(Cache *c, u64 address);
// operations
u32 evict(Cache *c, u32 set_index);
void store(Cache *c, u32 set_index, u32 line_index, u64 tag);
void read(Cache *c, u64 address);
