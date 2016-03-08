#include <stdlib.h>
#include <string.h>

#include "phonebook_opt.h"


/* FILL YOUR OWN IMPLEMENTATION HERE! */
#if defined(USE_MEM_POOL)
extern unsigned int line_count;

entry *createMemoryPool(unsigned int size)
{
    return (entry *)malloc(sizeof(entry) * size);
}

entry *findName(char lastName[], entry *pHead)
{
    unsigned int i = 0;

    if (line_count >= MAX_USE_MEM_POOL_SIZE)
        return NULL;

    while (i < line_count) {
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
