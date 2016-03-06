#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#define MAX_LAST_NAME_SIZE 16

/* original version */
typedef struct __PHONE_BOOK_ENTRY {
    char lastName[MAX_LAST_NAME_SIZE];
    char firstName[16];
    char email[16];
    char phone[10];
    char cell[10];
    char addr1[16];
    char addr2[16];
    char city[16];
    char state[2];
    char zip[5];
#ifdef OPT_ALIGN
    char alignByte[OPT_ALIGN];
#endif
    unsigned char privData[];
} entry;

typedef struct __IMPL {
    const char name[12];
    /* if return NULL, nothing found */
    entry *(*findName)(char lastname[], entry *pHead);
    // append an entry to data structure, if false, then this function is not yet implemented
    // allow implementation to change the value of e for some optimization
    int (*append)(char lastName[], entry **ppHead, entry **pE);
    int (*free)(entry *pHead);
} impl;

/* find implementation and provide memory function */
int initImpl(impl *opt, const char *implName);
void setMemoryFunc(void *(*allocator)(size_t), void (*deallocator)(void *));

#endif
