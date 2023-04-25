#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CACHE_SIZE 32
#define BLOCK_SIZE 4
//block of code is including standard libraries and defining some constants
typedef struct cache {
    int cache_size;   
    int cache_assoc;  
    int cachenum_sets;   
    int block_size; 
    int Replacement;     
    int cache_hit;   
    int cache_miss;   
    int **TagArr;  
    int **valid_Arr;    
    char **data;    
    int *lru;   
} Cache;

// Helper function to convert hexadecimal strings to integers
int hex_to_int(char *hex) {
    return (int) strtol(hex, NULL, 16);
}

// a helper function that converts a hexadecimal string to an integer.
void init_cache(Cache *cache, int cache_assoc, int Replacement) {
    cache->cache_assoc = cache_assoc;
    cache->cachenum_sets = cache->cache_size / (cache_assoc * cache->block_size);
    cache->Replacement = Replacement;
    cache->cache_hit = 0;
    cache->cache_miss = 0;
    
    cache->TagArr = calloc(cache->cachenum_sets, sizeof(int *));
    cache->valid_Arr = calloc(cache->cachenum_sets, sizeof(int *));
    cache->data = calloc(cache->cachenum_sets, sizeof(char *));
    
    if (Replacement == 0) {
        cache->lru = calloc(cache->cachenum_sets, sizeof(int));
    }
    
    for (int i = 0; i < cache->cachenum_sets; i++) {
        cache->TagArr[i] = calloc(cache_assoc, sizeof(int));
        cache->valid_Arr[i] = calloc(cache_assoc, sizeof(int));
        cache->data[i] = calloc(cache_assoc * cache->block_size, sizeof(char));
        
        memset(cache->TagArr[i], -1, cache_assoc * sizeof(int));
        memset(cache->valid_Arr[i], 0, cache_assoc * sizeof(int));
        memset(cache->data[i], 0, cache_assoc * cache->block_size * sizeof(char));
    }
}

// Helper function to access the cache
void access_cache(Cache *cache, int addr) {
    int set_index = (addr / cache->block_size) % cache->cachenum_sets;
    int tag = (addr / cache->block_size) / cache->cachenum_sets;
    int i, j, lru_index, random_index, empty_index;

    // Check if the address is already in the cache
    for (i = 0; i < cache->cache_assoc; i++) {
        if (cache->valid_Arr[set_index][i] && cache->TagArr[set_index][i] == tag) {
            cache->cache_hit++;
            return;
        }
    }

    // If the address is not in the cache, find an empty block or replace one using the selected policy
    cache->cache_miss++;
    empty_index = -1;
    lru_index = -1;
    random_index = -1;

    for (i = 0; i < cache->cache_assoc; i++) {
        if (!cache->valid_Arr[set_index][i]) {
            empty_index = i;
            break;
        }
    }

    if (cache->Replacement == 0) {  // LRU policy
        lru_index = 0;
        for (i = 1; i < cache->cache_assoc; i++) {
            if (cache->lru[set_index * cache->cache_assoc + i] < cache->lru[set_index * cache->cache_assoc + lru_index]) {
                lru_index = i;
            }
        }
        cache->lru[set_index * cache->cache_assoc + lru_index] = cache->cache_assoc;
    } else {  // Random policy
        random_index = rand() % cache->cache_assoc;
    }

    // Update the cache block metadata and data with the new address
    if (empty_index >= 0) {
        cache->TagArr[set_index][empty_index] = tag;
        cache->valid_Arr[set_index][empty_index] = 1;
        memcpy(&cache->data[set_index][empty_index * cache->block_size], &addr, cache->block_size * sizeof(char));
    } else if (lru_index >= 0) {
        cache->TagArr[set_index][lru_index] = tag;
        cache->valid_Arr[set_index][lru_index] = 1;
        memcpy(&cache->data[set_index][lru_index * cache->block_size], &addr, cache->block_size * sizeof(char));
    } else if (random_index >= 0) {
        cache->TagArr[set_index][random_index] = tag;
        cache->valid_Arr[set_index][random_index] = 1;
        memcpy(&cache->data[set_index][random_index * cache->block_size], &addr, cache->block_size * sizeof(char));
    }
}

int main(int argc, char *argv[]) {
  
Cache cache;
char hex[9];
int addr;
int i;
// Initialize cache with size 32, associativity 1, block size 4, and replacement policy LRU
cache.cache_size = CACHE_SIZE;
cache.block_size = BLOCK_SIZE;
  //set the cache_size and block_size members of the cache variable to the values
init_cache(&cache, 1, 0);
// This calls the init_cache() function with three arguments
for (i = 0; i < 16; i++) {
    // Read hexadecimal address from input
    scanf("%s", hex);
    // Convert hexadecimal address to integer
    addr = hex_to_int(hex);
    // the hex_to_int() function to convert the hexadecimal address stored in the hex array to an integer representation
    access_cache(&cache, addr);
  // a pointer to the cache variable, and the integer representation of the address to access
}

// Print cache statistics
printf("Hits: %d\n", cache.cache_hit);
printf("Misses: %d\n", cache.cache_miss);
  //the cache hit and miss statistics to the console

return 0;
}