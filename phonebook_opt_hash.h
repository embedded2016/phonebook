#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#define MAX_LAST_NAME_SIZE 16

/* TODO: After modifying the original version, uncomment the following
 * line to set OPT properly */
#ifdef HASH_1
#define HASH1 1
#endif

#if HASH_2
#define HASH2 1
#endif

#if 0
#define BUCKET_UNIT 42737
#else
#define BUCKET_UNIT 7919
#endif

#ifdef THREAD

#define NUM_OF_THREADS (MAX_BUFFER_SIZE/LINE_H + 1)
#define HASH_TABLE_BUCKET ((BUCKET_UNIT / NUM_OF_THREADS) + 1)
#define MAX_BUFFER_SIZE 400000
#define LINE_H 10000

#else /* else of THREAD */

#define HASH_TABLE_BUCKET BUCKET_UNIT

#endif /* end of THREAD */

#if defined(USE_MEM_POOL)
#define MAX_MEM_POOL_SIZE 2000
#endif

typedef struct phoneBook_s {
    char firstName[16];
    char email[16];
    char phone[10];
    char cell[10];
    char addr1[16];
    char addr2[16];
    char city[16];
    char state[2];
    char zip[5];
} phoneBook_t;

typedef struct __PHONE_BOOK_ENTRY {
    char lastName[MAX_LAST_NAME_SIZE];
    phoneBook_t *phoneBook;
    struct __PHONE_BOOK_ENTRY *pNext;
} entry;

typedef struct hashEntry_s {
#if defined(USE_MEM_POOL)
    entry *pool;
    unsigned int pool_count;
#else
    entry *pHead;
    entry *pTail;

#ifdef DEBUG
    unsigned int slot;
#endif
#endif
} hashEntry_t;

typedef struct hashTable_s {
    unsigned int bucketSize;
    unsigned int slotSize;
#if defined(USE_MEM_POOL)
    unsigned int poolSize;
#endif

#ifdef THREAD
    hashEntry_t *bucket[NUM_OF_THREADS];
#ifdef DEBUG
    unsigned int activeBuckets[NUM_OF_THREADS];
#endif
#else
    hashEntry_t *bucket;
#ifdef DEBUG
    unsigned int activeBuckets;
#endif
#endif
} hashTable_t;

#ifdef THREAD
typedef struct thread_data_s {
    int start;
    int end;
    int thd;
} thread_data_t;
#endif

entry *findName(char lastName[], entry *pHead);
#ifdef THREAD
entry *append(char lastName[], entry *e, int thd);
#else
entry *append(char lastName[], entry *e);
#endif

#if defined(HASH1) || defined(HASH2)
void initHashTable(unsigned int bucket_size, unsigned int pool_size);
void freeHashTable();
#endif

#endif

