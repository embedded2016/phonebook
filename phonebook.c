#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phonebook.h"

#define GET_PRIV_PTR(IMPL, ptr) ((IMPL *)((ptr)->privData))

typedef struct __LINKEDLIST_PRIV_DATA {
    struct __PHONE_BOOK_ENTRY *pNext;
} llpriv;

static entry *llFindName(char lastname[], entry *pHead);
static int llAppend(char lastName[], entry **pHead, entry **e);
static int llFree(entry *pHead);


static impl implList[] = {{.name="linkedlist", .findName=llFindName, .append=llAppend, .free=llFree}};

int initImpl(impl *opt, const char *implName)
{
    int i;
    for(i=0; i<sizeof(implList)/sizeof(impl); i++) {
        if(strcmp(implList[i].name, implName)==0) {
            memcpy(opt, implList+i, sizeof(impl));
            return 1;
        }
    }
    return 0;
}

static entry *llFindName(char lastname[], entry *pHead)
{
    while (pHead != NULL) {
        if (strcasecmp(lastname, pHead->lastName) == 0)
            return pHead;
        pHead = GET_PRIV_PTR(llpriv, pHead)->pNext;
    }
    return NULL;
}

static int llAppend(char lastName[], entry **ppHead, entry **pE)
{
    if(*ppHead == NULL) {
        *pE = *ppHead = (entry *) malloc(sizeof(entry)+sizeof(llpriv));
        GET_PRIV_PTR(llpriv, *pE)->pNext = NULL;
        strcpy((*pE)->lastName, lastName);
    } else {
        /* allocate memory for the new entry and put lastName */
        GET_PRIV_PTR(llpriv, *pE)->pNext = (entry *) malloc(sizeof(entry)+sizeof(llpriv));
        (*pE) = GET_PRIV_PTR(llpriv, *pE)->pNext;
        strcpy((*pE)->lastName, lastName);
        GET_PRIV_PTR(llpriv, *pE)->pNext = NULL;
    }
    return 1;
}

static int llFree(entry *pHead)
{
    while(pHead != NULL) {
        entry *next = GET_PRIV_PTR(llpriv, pHead)->pNext;
        free(pHead);
        pHead = next;
    }
}
