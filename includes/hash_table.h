#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct HashTable_s* HashTable;

HashTable htable_new(int n_buckets, void (*destroy_value)(void*));
void htable_destroy(HashTable htable);

void htable_put(HashTable htable, char* key, void* value);
void htable_remove(HashTable htable, char* key);
void* htable_get(HashTable htable, char* key);

int htable_size(HashTable htable);
int htable_has_key(HashTable htable, char* key);

#endif
