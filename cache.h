#include <inttypes.h>
#include <stdbool.h>

#define u8 uint8_t
#define u32 uint32_t

typedef struct Line {
	u8 tag;
	bool valid;
} Line;

typedef struct Set {
	Line *lines;
	u32 l;
} Set;

typedef struct Cache {
	u32 s;  
	Set *sets;
} Cache;


