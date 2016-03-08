#include <stdlib.h>
#include <string.h>
#include <assert.h>


#ifdef THREAD
#include <pthread.h>
#endif

#include "phonebook_opt_hash.h"

/* FILL YOUR OWN IMPLEMENTATION HERE! */
hashTable_t hashTable;

#ifdef HASH1
unsigned int hash1(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal += *key++;
    }
    return hashVal % ht->tableSize;
}
#define hashFunc(k, h) hash1(k, h)

#elif HASH2
unsigned int hash2(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal = (hashVal << 5) + *key++;
    }
    return hashVal % ht->tableSize;
}
#define hashFunc(k, h) hash2(k, h)

#elif
#error
#endif

#ifdef THREAD
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void initHashTable(unsigned int bucket_size, unsigned int pool_size)
{
    hashTable.tableSize = bucket_size;

#ifndef THREAD
    hashTable.bucket = malloc(sizeof(hashEntry_t) * bucket_size);
#if defined(USE_MEM_POOL)
    hashTable.poolSize = pool_size;

#if 0 /* Evan: TEST */
    printf("\r\n(%s:%d) ---> bucket_size=%d, pool_size=%d, poolSize=%d, bucket=%p", __FUNCTION__, __LINE__,
           bucket_size,
           pool_size,
           hashTable.poolSize,
           hashTable.bucket);
#endif

    for (int i = 0; i < bucket_size; i++) {
        (hashTable.bucket + i)->pool = (entry *)malloc(sizeof(entry) * hashTable.poolSize);
        (hashTable.bucket + i)->pool_count = 0;
#if 0 /* Evan: TEST */
        printf("\r\n(%s:%d) ---> i=%d, bucket=%p, pool=%p", __FUNCTION__, __LINE__,
               i,
               (hashTable.bucket + i),
               (hashTable.bucket + i)->pool);
#endif
    }
#endif
#endif

#ifdef DEBUG
    hashTable.bucketSize = 0;
#endif
}

void freeHashTable()
{
#ifndef THREAD
#if defined(USE_MEM_POOL)
    int i = 0;

#if 0 /* Evan: TEST */
    printf("\r\n(%s:%d) ---> tableSize=%d, bucket=%p", __FUNCTION__, __LINE__,
           hashTable.tableSize,
           hashTable.bucket);
#endif

    for (i = 0; i < hashTable.tableSize; i++) {
#if 0 /* Evan: TEST */
        printf("\r\n(%s:%d) ---> i=%d, bucket=%p, pool=%p", __FUNCTION__, __LINE__,
               i,
               (hashTable.bucket + i),
               (hashTable.bucket + i)->pool);
#endif
        free((hashTable.bucket + i)->pool);
        (hashTable.bucket + i)->pool = NULL;
    }
#endif

    free(hashTable.bucket);
    hashTable.bucket = NULL;
#endif
}

#ifdef THREAD
entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;
    int i = 0;

    key = hashFunc(lastName, &hashTable);

    for (i = 0; i < NUM_OF_THREADS; i++) {
        hash = &hashTable.bucket[i][key];
        e = hash->pHead;
        while (e != NULL) {
            if (strcasecmp(lastName, e->lastName) == 0) {
                return e;
            }
            e = e->pNext;
        }
    }
    return NULL;
}

entry *append(char lastName[], entry *e, int thd)
{
    unsigned int key = 0;
    hashEntry_t *hash;

    key = hashFunc(lastName, &hashTable);
    hash = &hashTable.bucket[thd][key] ;

    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    strcpy(e->lastName, lastName);

    if (hash->pHead == NULL) {
        hash->pHead = e;
#if defined(__GNUC__)
        __builtin___clear_cache((char *) hash->pHead, (char *) hash->pHead + sizeof(entry));
#endif
#ifdef DEBUG
        pthread_mutex_lock(& mutex);
        hashTable.bucketSize++;
        pthread_mutex_unlock(& mutex);
#endif
    } else {
        hash->pTail->pNext = e;
    }
    hash->pTail = e;
#ifdef DEBUG
    hash->key = key;
    hash->slot++;
#endif

    return e;
}

#else /* else of THREAD */
entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;

    key = hashFunc(lastName, &hashTable);
    hash = (hashTable.bucket) + key;

#if defined(USE_MEM_POOL)
#if 1
    printf("\r\n(%s:%d) ---> lastName=(%s), key=%d, bucketSize=%d, pool_count=%d", __FUNCTION__, __LINE__,
           lastName,
           key,
           hashTable.bucketSize,
           hash->pool_count);
#endif

    unsigned int i = 0;
    while (i <= hash->pool_count) {
#if 0
        printf("\r\n(%s:%d) ---> i=%d, lastName=(%s)", __FUNCTION__, __LINE__,
               i,
               (hash->pool + i)->lastName);
#endif
        if (strcasecmp(lastName, (hash->pool + i)->lastName) == 0) {
            return (hash->pool + i);
        }
        i++;
    }
#else
    e = hash->pHead;
    while (e != NULL) {
        if (strcasecmp(lastName, e->lastName) == 0) {
            return e;
        }
        e = e->pNext;
    }
#endif
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;

    key = hashFunc(lastName, &hashTable);
    hash = ((hashTable.bucket) + key);

#if defined(USE_MEM_POOL)

#if 0
    printf("\r\n(%s:%d) ---> key=%d", __FUNCTION__, __LINE__,
           key);
#endif
#if 1
    if (hash->pool_count == 0) {
        /*
        printf("\r\n(%s:%d) ---> bucketSize=%d, key=%d", __FUNCTION__, __LINE__,
        hashTable.bucketSize,
        key);
        */
#ifdef DEBUG
        hashTable.bucketSize++;
        assert(hashTable.bucketSize <= HASH_TABLE_BUCKET);
#endif
    }
#endif
    strcpy((hash->pool + (hash->pool_count))->lastName, lastName);
#if 0
    printf("\r\n(%s:%d) ---> key=%d, pool_count=%d, lastName=(%s)", __FUNCTION__, __LINE__,
           key,
           hash->pool_count,
           (hash->pool + (hash->pool_count))->lastName);
#endif
    e = (hash->pool + (hash->pool_count));
    hash->pool_count++;
    assert(hash->pool_count < MAX_USE_MEM_POOL_SIZE);

    return e;
#else
    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    strcpy(e->lastName, lastName);

    if (hash->pHead == NULL) {
        hash->pHead = e;
#ifdef DEBUG
        hashTable.bucketSize++;
#endif
    } else {
        hash->pTail->pNext = e;
    }
    hash->pTail = e;

#ifdef DEBUG
    hash->key = key;
    hash->slot++;
#endif

    return e;
#endif
}
#endif /* end of THREAD */

