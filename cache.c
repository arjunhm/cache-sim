#include "cache.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool is_power_2(int n) {
  int count = 0;

  for (int i = 0; i < sizeof(int) * 8; i++) {
    count += (n & 1);
    n = n >> 1;
  }
  return (count == 1);
}

int logtwo(int n) {
  if (!is_power_2(n))
    return -1;

  for (int i = 0; i < sizeof(int) * 8; i++) {
    if ((n & 1) == 1)
      return i;
    n = n >> 1;
  }
  return -1;
}

void *cache_new(u32 set_count, u32 line_count, u32 block_count) {

  Cache *c = malloc(sizeof(Cache));
  if (c == NULL)
    return NULL;

  c->set_count = set_count;
  c->set_bits = logtwo(set_count);
  c->block_count = block_count;
  c->block_bits = logtwo(block_count);
  c->tag_bits = ADDRESS_WIDTH - (c->set_bits + c->block_bits);
  c->stats = malloc(sizeof(Stats));
  if (c->stats == NULL) {
    free(c);
    return NULL;
  }
  c->stats->hits = 0;
  c->stats->misses = 0;
  c->stats->evictions = 0;

  c->sets = (Set *)malloc(set_count * sizeof(Set));
  if (c->sets == NULL) {
    free(c);
    free(c->stats);
    return NULL;
  }

  for (u32 i = 0; i < set_count; i++) {
    c->sets[i].line_count = line_count;
    c->sets[i].lines = (Line *)malloc(line_count * sizeof(Line));

    if (c->sets[i].lines == NULL) {
      free(c->sets);
      free(c->stats);
      free(c);
      return NULL;
    }

    for (u32 j = 0; j < line_count; j++) {
      Line l = c->sets[i].lines[j];
      l.valid = false;
      l.tag = 0;
      l.timestamp = 0;
      // TODO alloc blocks
    }
  }
  return c;
}

void cache_free(Cache *c) {
  if (c == NULL)
    return;

  if (c->sets == NULL) {
    free(c);
    return;
  }

  int lc = c->sets[0].line_count;
  for (u32 i = 0; i < c->set_count; i++) {
    for (u32 j = 0; j < lc; j++) {
      // TODO dealloc blocks
    }
    free(c->sets[i].lines);
  }
  free(c->sets);
  free(c);
}

void display_stats(Stats *s) {
  printf("---Stats---\n");
  printf("Hits=%d\tMisses=%d\tEvictions=%d\n", s->hits, s->misses,
         s->evictions);
  printf("-----\n");
}

void display(Cache *c) {
  if (c == NULL)
    return;

  printf("---Configuration---\n");
  printf("set_count=%d (%d)\n", c->set_count, c->set_bits);
  printf("block_count=%d (%d)\n", c->block_count, c->block_bits);
  printf("tag_count=(%d)\n", c->tag_bits);
  display_stats(c->stats);

  for (u32 i = 0; i < c->set_count; i++) {
    printf("Set %d\n", i);
    for (u32 j = 0; j < c->sets[i].line_count; j++) {
      printf("Line %d\n", j);
      Line line = c->sets[i].lines[j];
      printf("\tvalid=%d\ttag=%ld\n", line.valid, line.tag);
    }
    printf("-----\n");
  }
}

u64 get_block_offset(Cache *c, u64 address) {
  int64_t mask = ~((0x1L << 63) >> (63 - c->block_bits));
  return (address & mask);
}

u64 get_set_index(Cache *c, u64 address) {
  int64_t mask = (0x1L << 63) >> (63 - (c->set_bits + c->block_bits));
  int64_t block_mask = ~((0x1L << 63) >> (63 - c->block_bits));
  mask += block_mask;
  mask = ~mask;
  return (address & mask) >> c->block_bits;
}

u64 get_tag(Cache *c, u64 address) {
  int64_t mask = (0x1L << 63) >> (c->tag_bits - 1);
  return (address & mask);
}

void hexprint(u64 addr, u64 result, char *c) {
  printf("a=%lx\n", addr);
  printf("%s=%lx\n\n", c, result);
}

void test_mask(Cache *c) {

  u8 nums[6] = {1, 2, 3, 4, 5, 6};

  u64 a;
  u64 r;

  for (int i = 0; i < 6; i++) {
    a = (u64)&nums[i];

    r = (u64)get_block_offset(c, a);
    hexprint(a, r, "block offset");

    r = (u64)get_set_index(c, a);
    hexprint(a, r, "set index");

    r = (u64)get_tag(c, a);
    hexprint(a, r, "tag");

    printf("-----\n");
  }
}

u32 evict(Cache *c, u32 set_index) {
  printf("evicting....\n");
  u32 line_index = 0;
  u32 min_timestamp = time(NULL);

  for (int i = 0; i < c->sets[set_index].line_count; i++) {
    Line l = c->sets[set_index].lines[i];
    if (l.timestamp < min_timestamp) {
      min_timestamp = l.timestamp;
      line_index = i;
    }
  }
  printf("li=%d\n", line_index);

  if (c->sets[set_index].lines[line_index].tag != 0)
    c->stats->evictions++;

  return line_index;
}

void store(Cache *c, u32 set_index, u32 line_index, u64 tag) {
  c->sets[set_index].lines[line_index].tag = tag;
  c->sets[set_index].lines[line_index].valid = 1;
  c->sets[set_index].lines[line_index].timestamp = time(NULL);
}

void read(Cache *c, u64 address) {

  u32 set_index = get_set_index(c, address);
  u64 tag = get_tag(c, address);
  printf("addr=%lx\tsi=%d\ttag=%lx\n", address, set_index, tag);

  for (u32 i = 0; i < c->sets[set_index].line_count; i++) {
    Line l = c->sets[set_index].lines[i];

    // hit
    if (l.valid && l.tag == tag) {
      c->stats->hits++;
      return;
    }
  }

  // miss. evict and store
  c->stats->misses++;
  u32 line_index = evict(c, set_index);
  store(c, set_index, line_index, tag);
}

void test_cache(Cache *c) {
  int N = 20;
  u64 a;
  u8 nums[20] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  for (int i = 0; i < N; i++) {
    a = (u64)&nums[i];
    read(c, a);
    display_stats(c->stats);
    printf("\n\n");
  }
}

int main() {

  Cache *c = cache_new(2, 2, 4);
  if (c == NULL)
    return 0;
  // display(c);

  // test_mask(c);
  test_cache(c);

  free(c);
  return 0;
}
