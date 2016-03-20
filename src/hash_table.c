#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
#include "hashlib.h"

typedef struct Bucket_s* Bucket;
typedef struct BucketNode_s* BucketNode;
typedef void (*DestroyFunc)(void *);

struct BucketNode_s {
    char* key;
    void* value;
    BucketNode next;
};

struct Bucket_s {
    int size;
    BucketNode first;
};

struct HashTable_s {
    int size;
    int n_buckets;
    DestroyFunc destroy_value;
    Bucket* buckets; 
};

/**
 * BucketNode
 */

static BucketNode bnode_new(char* key, void* value, BucketNode next) {
    BucketNode bnode = (BucketNode)malloc(sizeof(struct BucketNode_s));

    bnode->key = strdup(key);
    bnode->value = value;
    bnode->next = next;

    return bnode;
}

static void bnode_destroy(BucketNode bnode, DestroyFunc destroy_value) {
    destroy_value(bnode->value);
    free(bnode->key);
    free(bnode);
}

static int bnode_match(BucketNode bnode, char* key) {
    return !strcmp(bnode->key, key);
}

static void bnode_replace(
        BucketNode bnode,
        void* value,
        DestroyFunc destroy_value) {
    
    destroy_value(bnode->value);
    bnode->value = value;
}

/**
 * Bucket
 */

static Bucket bucket_new() {
    Bucket bucket = (Bucket)malloc(sizeof(struct Bucket_s));

    bucket->size = 0;
    bucket->first = NULL;

    return bucket;
}

static void bucket_destroy(Bucket bucket, DestroyFunc destroy_value) {
    BucketNode curr, next;

    if (!bucket)
        return;

    curr = bucket->first;

    while (curr) {
        next = curr->next;
        
        bnode_destroy(curr, destroy_value);

        curr = next;
    }

    free(bucket);
}

static BucketNode* bucket_find(Bucket bucket, char* key) {
    BucketNode* curr;

    curr = &(bucket->first);

    while (*curr) {
        if (bnode_match(*curr, key))
            return curr;

        curr = &((*curr)->next);
    }

    return NULL;
}

/**
 * HashTable
 */

static Bucket* htable_get_bucket(HashTable htable, char* key) {
    Bucket* bucket;
    unsigned int hash_value;

    hash_value = djb2(key) % htable->n_buckets;

    bucket = &(htable->buckets[hash_value]);

    return bucket;
}

HashTable htable_new(int n_buckets, DestroyFunc destroy_value) {
    HashTable htable = (HashTable)malloc(sizeof(struct HashTable_s));
    int i;

    htable->size = 0;
    htable->n_buckets = n_buckets;
    htable->buckets = (Bucket*)malloc(sizeof(Bucket) * n_buckets);
    htable->destroy_value = destroy_value;

    for (i = 0; i < n_buckets; i++) {
        htable->buckets[i] = NULL;
    }

    return htable;
}

void htable_destroy(HashTable htable) {
    int i;
    
    for (i = 0; i < htable->n_buckets; i++) {
        bucket_destroy(htable->buckets[i], htable->destroy_value);
    }

    free(htable->buckets);
    free(htable);
}

void htable_put(HashTable htable, char* key, void* value) {
    BucketNode* bnode_ptr;
    BucketNode bnode;
    Bucket* bucket_ptr;
    Bucket bucket;

    bucket_ptr = htable_get_bucket(htable, key);
    bucket = *bucket_ptr;

    if (!bucket) {
        *bucket_ptr = bucket_new();
        bucket = *bucket_ptr;
    }
    
    bnode_ptr = bucket_find(bucket, key);

    if (!bnode_ptr) {
        bnode = bnode_new(key, value, bucket->first);
        bucket->first = bnode;
        htable->size++;
    }
    else {
        bnode = *bnode_ptr;
        bnode_replace(bnode, value, htable->destroy_value); 
    }
}

void htable_remove(HashTable htable, char* key) {
    BucketNode* bnode_ptr;
    BucketNode bnode, next;
    Bucket* bucket_ptr;
    Bucket bucket;

    bucket_ptr = htable_get_bucket(htable, key);
    bucket = *bucket_ptr;

    if (!bucket)
        return;

    bnode_ptr = bucket_find(bucket, key);

    if (bnode_ptr) {
        bnode = *bnode_ptr;
        next = bnode->next;

        bnode_destroy(bnode, htable->destroy_value);
        *bnode_ptr = next;
        htable->size--;
    }
}

void* htable_get(HashTable htable, char* key) {
    BucketNode* bnode_ptr;
    Bucket* bucket_ptr;
    Bucket bucket;

    bucket_ptr = htable_get_bucket(htable, key);
    bucket = *bucket_ptr;

    if (!bucket)
        return NULL;

    bnode_ptr = bucket_find(bucket, key);

    if (bnode_ptr) {
        return (*bnode_ptr)->value;
    }

    return NULL;
}

int htable_size(HashTable htable) {
    return htable->size;
}

int htable_has_key(HashTable htable, char* key) {
    BucketNode* bnode_ptr;
    Bucket* bucket_ptr;
    Bucket bucket;

    bucket_ptr = htable_get_bucket(htable, key);
    
    bucket = *bucket_ptr;

    if (!bucket)
        return 0;

    bnode_ptr = bucket_find(bucket, key);

    if (bnode_ptr)
        return 1;

    return 0;
}
