#include <stdlib.h>
#include <string.h>

#include <stdio.h>
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
    return hashVal % ht->bucketSize;
}
#define hashFunc(k, h) hash1(k, h)

#elif HASH2
unsigned int hash2(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal = (hashVal << 5) + *key++;
    }
    return hashVal % ht->bucketSize;
}
#define hashFunc(k, h) hash2(k, h)

#elif
#error
#endif

#ifdef THREAD
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void initHashTable(unsigned int bucket_size, unsigned int pool_size)
{
    int i;
#if defined(USE_MEM_POOL)
    int j;
#endif

    memset(&hashTable, 0,sizeof(hashTable));

    hashTable.bucketSize = bucket_size;
#if defined(USE_MEM_POOL)
    hashTable.poolSize = pool_size;
#endif

    for (i = 0; i < NUM_OF_THREADS; i++) {
        hashTable.bucket[i] = malloc(sizeof(hashEntry_t) * bucket_size);
#if defined(USE_MEM_POOL)
        for (j = 0; j < bucket_size; j++) {
            (hashTable.bucket[i] + j)->pool = (entry *)malloc(sizeof(entry) * hashTable.poolSize);
            (hashTable.bucket[i] + j)->pool_count = 0;
        }
#endif
    }
}

void freeHashTable()
{
    int i;
#if defined(USE_MEM_POOL)
    int j;
#endif

    for (i = 0; i < NUM_OF_THREADS; i++) {
#if defined(USE_MEM_POOL)
        for (j = 0; j < hashTable.bucketSize; j++) {
            free((hashTable.bucket[i] + j)->pool);
            (hashTable.bucket[i] + j)->pool = NULL;
        }
#endif
        free(hashTable.bucket[i]);
    }
}

entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;
    int i;
#if defined(USE_MEM_POOL)
    int j;
#endif

    key = hashFunc(lastName, &hashTable);


#if defined(USE_MEM_POOL)
    for (i = 0; i < NUM_OF_THREADS; i++) {
        hash = hashTable.bucket[i] + key;
        j = 0;
        while (j <= hash->pool_count) {
            if (strcasecmp(lastName, (hash->pool + j)->lastName) == 0) {
                return (hash->pool + j);
            }
            j++;
        }
    }
#else
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
#endif
    return NULL;
}

entry *append(char lastName[], entry *e, int thd)
{
    unsigned int key = 0;
    hashEntry_t *hash;

    key = hashFunc(lastName, &hashTable);
    hash = hashTable.bucket[thd] + key ;

#if defined(USE_MEM_POOL)
#ifdef DEBUG
    if (hash->pool_count == 0) {
//        pthread_mutex_lock(& mutex);
        hashTable.activeBuckets[thd]++;
        if (hashTable.activeBuckets[thd] > HASH_TABLE_BUCKET)
            printf("\r\nthd=%d, key=%d, hashTable.activeBuckets=%d\n",
                   thd,
                   key,
                   hashTable.activeBuckets[thd]);
        assert(hashTable.activeBuckets[thd] <= HASH_TABLE_BUCKET);
//        pthread_mutex_unlock(& mutex);
    }
#endif
    strcpy((hash->pool + (hash->pool_count))->lastName, lastName);
    e = (hash->pool + (hash->pool_count));
    hash->pool_count++;
    assert(hash->pool_count <= MAX_USE_MEM_POOL_SIZE);
    return e;
#else
    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    strcpy(e->lastName, lastName);

    if (hash->pHead == NULL) {
        hash->pHead = e;
#if defined(__GNUC__)
        __builtin___clear_cache((char *) hash->pHead, (char *) hash->pHead + sizeof(entry));
#endif
# ifdef DEBUG
//        pthread_mutex_lock(& mutex);
        hashTable.activeBuckets[thd]++;
//        pthread_mutex_unlock(& mutex);
#endif
    } else {
        hash->pTail->pNext = e;
    }
    hash->pTail = e;

#ifdef DEBUG
    hash->slot++;
#endif

    return e;
#endif
}

#else /* else of THREAD */
void initHashTable(unsigned int bucket_size, unsigned int pool_size)
{
#if defined(USE_MEM_POOL)
    int i;
#endif

    memset(&hashTable, 0,sizeof(hashTable));

    hashTable.bucketSize = bucket_size;
    hashTable.slotSize = 0;
#if defined(USE_MEM_POOL)
    hashTable.poolSize = pool_size;
#endif

    hashTable.bucket = malloc(sizeof(hashEntry_t) * bucket_size);
#if defined(USE_MEM_POOL)
    for (i = 0; i < bucket_size; i++) {
        (hashTable.bucket + i)->pool = (entry *)malloc(sizeof(entry) * hashTable.poolSize);
        (hashTable.bucket + i)->pool_count = 0;
    }
#endif

#ifdef DEBUG
    hashTable.activeBuckets = 0;
#endif
}

void freeHashTable()
{
#if defined(USE_MEM_POOL)
    int i;
#endif

#if defined(USE_MEM_POOL)
    for (i = 0; i < hashTable.bucketSize; i++) {
        free((hashTable.bucket + i)->pool);
        (hashTable.bucket + i)->pool = NULL;
    }
#endif
    free(hashTable.bucket);
    hashTable.bucket = NULL;
}

entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;

    key = hashFunc(lastName, &hashTable);
    hash = (hashTable.bucket) + key;

#if defined(USE_MEM_POOL)
    unsigned int i = 0;
    while (i <= hash->pool_count) {
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
#ifdef DEBUG
    if (hash->pool_count == 0) {
        hashTable.activeBuckets++;
        assert(hashTable.activeBuckets <= HASH_TABLE_BUCKET);
    }
#endif
    strcpy((hash->pool + (hash->pool_count))->lastName, lastName);
    e = (hash->pool + (hash->pool_count));
    hash->pool_count++;
    assert(hash->pool_count <= MAX_USE_MEM_POOL_SIZE);
    return e;
#else
    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    strcpy(e->lastName, lastName);

    if (hash->pHead == NULL) {
        hash->pHead = e;
#ifdef DEBUG
        hashTable.activeBuckets++;
#endif
    } else {
        hash->pTail->pNext = e;
    }
    hash->pTail = e;

#ifdef DEBUG
    hash->slot++;
#endif
    return e;
#endif
}
#endif /* end of THREAD */

