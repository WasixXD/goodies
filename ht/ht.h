/*
 * HT.H - Header-Only Hash Table Library
 * ====================================
 * 
 * A simple and efficient hash table (map/dictionary) implementation in C
 * that uses linear probing for collision resolution and the FNV-1a hash algorithm.
 * 
 * FEATURES:
 * - String keys (const char*)
 * - Generic values (void*)
 * - Automatic resizing when load factor exceeds 50%
 * - Linear probing collision resolution
 * - FNV-1a hash function for good distribution
 * 
 * HOW TO USE:
 * Define HT_IMPLEMENTATION before including to get the implementation:
 * 
 * #define HT_IMPLEMENTATION
 * #include "ht.h"
 * 
 * BASIC USAGE EXAMPLE:
 * 
 *   // Create a new map with initial capacity of 16
 *   Map *map = ht_new_map(16);
 *   
 *   // Insert data
 *   int *age = malloc(sizeof(int));
 *   *age = 25;
 *   ht_set(map, "john", age);
 *   
 *   char *city = malloc(strlen("New York") + 1);
 *   strcpy(city, "New York");
 *   ht_set(map, "city", city);
 *   
 *   // Retrieve data
 *   int *retrieved_age = (int*)ht_get(map, "john");
 *   char *retrieved_city = (char*)ht_get(map, "city");
 *   
 *   printf("John is %d years old\n", *retrieved_age);  // John is 25 years old
 *   printf("City: %s\n", retrieved_city);              // City: New York
 *   
 *   // Check size
 *   printf("Items in map: %zu\n", ht_length(map));     // Items in map: 2
 *   
 *   // Clean up memory
 *   ht_destroy(map);
 * 
 * AVAILABLE FUNCTIONS:
 * 
 * Map *ht_new_map(size_t capacity)
 *   - Creates a new map with the specified capacity
 *   - Parameters: capacity - initial capacity (must be > 0)
 *   - Returns: pointer to the new map or NULL on error
 *   - Example: Map *m = ht_new_map(32);
 * 
 * const char *ht_set(Map *m, const char *key, void *value)
 *   - Inserts or updates a key-value pair in the map
 *   - Parameters: m - map pointer, key - string key, value - value pointer
 *   - Returns: pointer to the inserted key or NULL on error
 *   - Automatically resizes if needed
 *   - Example: ht_set(map, "name", "Alice");
 * 
 * void *ht_get(Map *m, const char *key)
 *   - Retrieves a value by its key
 *   - Parameters: m - map pointer, key - string key to search for
 *   - Returns: pointer to the value or NULL if key not found
 *   - Example: char *name = (char*)ht_get(map, "name");
 * 
 * size_t ht_length(Map *m)
 *   - Returns the number of key-value pairs in the map
 *   - Parameters: m - map pointer
 *   - Returns: number of items currently stored
 *   - Example: size_t count = ht_length(map);
 * 
 * void ht_destroy(Map *m)
 *   - Frees all memory associated with the map
 *   - Parameters: m - map pointer
 *   - WARNING: This frees both keys and values, assuming they were malloc'd
 *   - Example: ht_destroy(map);
 * 
 * ADVANCED EXAMPLE:
 * 
 *   typedef struct {
 *       char name[50];
 *       int score;
 *   } Player;
 *   
 *   Map *players = ht_new_map(64);
 *   
 *   // Add players
 *   Player *p1 = malloc(sizeof(Player));
 *   strcpy(p1->name, "Alice");
 *   p1->score = 1500;
 *   ht_set(players, "player1", p1);
 *   
 *   Player *p2 = malloc(sizeof(Player));
 *   strcpy(p2->name, "Bob");
 *   p2->score = 1200;
 *   ht_set(players, "player2", p2);
 *   
 *   // Retrieve and use
 *   Player *alice = (Player*)ht_get(players, "player1");
 *   if (alice) {
 *       printf("%s has score: %d\n", alice->name, alice->score);
 *   }
 *   
 *   ht_destroy(players);
 * 
 * NOTES:
 * - Keys are stored as provided (shallow copy), ensure they remain valid
 * - Values are stored as void pointers, cast appropriately when retrieving
 * - The library assumes ownership of keys and values for ht_destroy()
 * - Load factor is maintained at 50% for optimal performance
 * - Uses FNV-1a hash function for good distribution and speed
 * - Linear probing handles collisions efficiently
 */

#ifndef HT_H
#define HT_H

#ifdef HT_IMPLEMENTATION
#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>

#define FNV_OFFSET_BASIS 14695981039346656037UL
#define FNV_PRIME 1099511628211UL 


typedef struct {
    const char *key;
    void* value;
} ht_entry;


typedef struct {
    ht_entry *ht;
    size_t capacity;
    size_t items;
} Map;


size_t ht_length(Map *m) {
    return m->items;
}

Map *ht_new_map(size_t capacity) {

    Map *m = (Map *)malloc(sizeof(Map));

    m->ht = (ht_entry *)calloc(capacity, sizeof(ht_entry));
    m->items = 0;
    m->capacity = capacity;

    return m;
}


static uint64_t ht__hash(const char *key) {
    uint64_t hash = FNV_OFFSET_BASIS;
    for(size_t i = 0; key[i] != '\0'; i++) {
        hash ^= (uint64_t)(unsigned char)key[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

const char *ht_entry_set(Map *m, size_t index, const char *key, void *value) {
    while(m->ht[index].value != NULL) {
        if(strcmp(m->ht[index].key, key) == 0) {
            m->ht[index].value = value;
            return m->ht[index].key;
        }
        index ++;
        if(index >= m->capacity) {
            index = 0;
        }
    }

    m->ht[index].key = key;
    m->ht[index].value = value;
    m->items++;

    return m->ht[index].key;
}

int ht_expand(Map *m) {
    size_t new_cap = m->capacity * 2;

    if(new_cap < m->capacity) {
        return -1;
    }

    ht_entry *new_entries = (ht_entry*)calloc(new_cap, sizeof(ht_entry));

    for(size_t i = 0; i < ht_length(m); i++) {
        if(m->ht[i].key != NULL)
            new_entries[i] = m->ht[i];
    }

    m->capacity = new_cap;
    m->ht = new_entries;
    return 0;
}

const char *ht_set(Map *m, const char *key, void* value) {
    if(key == NULL) {
        return NULL;
    }

    if(ht_length(m) >= m->capacity / 2) {
        if(ht_expand(m) < 0) {
            return NULL;
        }
    }
    uint64_t hash = ht__hash(key);
    size_t index = (size_t)(hash % m->capacity);
    return ht_entry_set(m, index, key, value);
}


void *ht_get(Map *m, const char *key) {
    uint64_t hash = ht__hash(key);
    size_t index = (size_t)(hash % m->capacity);
    while(m->ht[index].value != NULL) {
        if(strcmp(m->ht[index].key, key) == 0) {
            return m->ht[index].value;
        }
        index++;

        if(index >= m->capacity) {
            index = 0;
        }
    }
    return NULL;
}


void ht_destroy(Map *m) {
    for(size_t i = 0; i < ht_length(m); i++) {

        free(m->ht[i].value);
        free((void *)m->ht[i].key);
    }
    free(m->ht);
    free(m);
}


#endif // HT_IMPLEMENTATION
#endif // HT_H