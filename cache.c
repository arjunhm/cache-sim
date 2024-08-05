#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Cache *cache_new(uint32_t s, uint32_t l) {
	Cache *c = malloc(sizeof(Cache));
	if (c == NULL) {
		printf("malloc of cache failed\n");
		return NULL;
	}

	c->s = s;
	c->sets = (Set*)malloc(s * sizeof(Set));
	if (c->sets == NULL) {
		printf("malloc of sets failed\n");
		free(c);
		return NULL;
	}

	for(uint32_t i=0; i<s; i++) {
		c->sets[i].l = l;
		c->sets[i].lines = (Line*)malloc(sizeof(Line));

		if (c->sets[i].lines == NULL) {
			printf("malloc of line %d failed\n", i);
			free(c->sets);
			free(c);
		}

		for(uint32_t j=0; j<l; j++) {
			c->sets[i].lines[j].tag=0;
			c->sets[i].lines[j].valid=false;
		}
	}

	return c;
}

void cache_free(Cache *c) {
	
	if (c == NULL)
		return;

	if (c->sets != NULL){
		for(uint32_t i = 0; i < c->s; i++) {
			if (c->sets[i].lines != NULL)
				free(c->sets[i].lines);
		}
	}

	free(c->sets);
	free(c);
}


int main() {
	return 0;
}
