#define PICKY_IMPLEMENTATION
#define HT_IMPLEMENTATION
#include <ht.h>
#include <picky.h>

void map_creation(T *t) {

    size_t capacity = 5;
    Map *map = ht_new_map(capacity);

    picky_test(t, "ht_new_map() not null");
    picky_assertNotNull(t, map);

    picky_test(t, "ht_new_map() items are 0");
    picky_int_toBe(t, map->items, 0);

    picky_test(t, "ht_new_map() capacity");
    picky_int_toBe(t, map->capacity, capacity);

    ht_destroy(map);
}


void map_insertion(T *t) {
    size_t capacity = 5;
    Map *map = ht_new_map(capacity);

    picky_test(t, "ht_set() returns the key");
    int num = 42;
    int *item = &num;

    const char *r = ht_set(map, "foo", item);
    picky_assertNotNull(t, r);

    picky_test(t, "ht_get() equal pointer");
    int *get = (int *)ht_get(map, "foo");
    picky_assert(t, item == get);

    picky_test(t, "ht_get() point to correct value");
    picky_int_toBe(t, *get,  num);

    picky_test(t, "ht_get() returns NULL to non existent value");
    void *non_existent = ht_get(map, "asdf");
    picky_assert(t, non_existent == NULL);

    ht_destroy(map);
}

void map_expand(T *t) {
    size_t capacity = 4;

    Map *m = ht_new_map(capacity);
    picky_test(t, "length is 0");
    picky_int_toBe(t, m->capacity, capacity);

    ht_set(m, "foo", (void *)2);
    ht_set(m, "bar", (void *)1);
    ht_set(m, "bar", (void *)3);

    picky_test(t, "length is double");
    picky_int_toBe(t, m->capacity, capacity * 2);
}

int main(int argc, char **argv) {

    picky_describe("Map creation", map_creation);
    picky_describe("Map set/get", map_insertion);
    picky_describe("Map expand", map_expand);
}