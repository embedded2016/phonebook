#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phonebook_opt.h"


/* FILL YOUR OWN IMPLEMENTATION HERE! */
#if defined(USE_MEM_POOL)
static unsigned int buffer_count = 0;

entry *createMemoryPool(unsigned int size)
{
    buffer_count = 0;
    return (entry *)malloc(sizeof(entry) * size);
}

entry *findName(char lastName[], entry *pHead)
{
    unsigned int i = 0;

    i = 0;
    while (i < buffer_count) {
        if (strcasecmp(lastName, (pHead + i)->lastName) == 0) {
            return (pHead + i);
        }
        i++;
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    e++;
    strcpy(e->lastName, lastName);
    buffer_count++;
    assert(buffer_count <= MAX_MEM_POOL_SIZE);
    return e;
}
#else
entry *findName(char lastName[], entry *pHead)
{
    /* TODO: implement */
    while (pHead != NULL) {
        if (strcasecmp(lastName, pHead->lastName) == 0) {
            return pHead;
        }
        pHead = pHead->pNext;
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    /* allocate memory for the new entry and put lastName */
    e->pNext = (entry *) malloc(sizeof(entry));
    e = e->pNext;
    strcpy(e->lastName, lastName);
    e->pNext = NULL;
    return e;
}
#endif
