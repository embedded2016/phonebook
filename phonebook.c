#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phonebook.h"


#define GEN_DEF(prefix) \
static entry * prefix##FindName(char lastname[], entry *pHead);\
static int prefix##Append(char lastName[], entry **pHead, entry **e);\
static int prefix##Free(entry *pHead);
#define GEN_INIT_STRUCT(n, prefix) {.name=n, .findName=prefix##FindName, .append=prefix##Append, .free=prefix##Free}
#define ALLOC_ENTRY(IMPL) (entry *) malloc(sizeof(entry)+sizeof(IMPL))
#define GET_PRIV_PTR(IMPL, ptr) ((IMPL *)((ptr)->privData))

GEN_DEF(ll);
GEN_DEF(avl);
static impl implList[] = {
    GEN_INIT_STRUCT("linkedlist", ll),
    GEN_INIT_STRUCT("avltree", avl)
};
static void rightRotation(entry **parent,int *unbalanced);
static void leftRotation(entry **parent,int *unbalanced);
static int avlinsert(entry **parent,char lastname[],int * unbalanced);
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

/* Linked List */

typedef struct __LINKEDLIST_PRIV_DATA {
    struct __PHONE_BOOK_ENTRY *pNext;
} llpriv;

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
        *pE = *ppHead = ALLOC_ENTRY(llpriv);
        GET_PRIV_PTR(llpriv, *pE)->pNext = NULL;
        strcpy((*pE)->lastName, lastName);
    } else {
        /* allocate memory for the new entry and put lastName */
        GET_PRIV_PTR(llpriv, *pE)->pNext = ALLOC_ENTRY(llpriv);
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
    return 1;
}

/* AVL Tree */

typedef struct __AVLTREE_PRIV_DATA {
    struct __PHONE_BOOK_ENTRY *pLeft;
    struct __PHONE_BOOK_ENTRY *pRight;
    int bf;//hl-hr
} avlpriv;

static entry *avlFindName(char lastName[], entry *pHead)
{
    while (pHead != NULL) {
        int result=strcasecmp(lastName, pHead->lastName);
        if (result == 0)
            return pHead;
        else if(result > 0)
            pHead = GET_PRIV_PTR(avlpriv,pHead)->pRight;
        else
            pHead = GET_PRIV_PTR(avlpriv,pHead)->pLeft;
    }
    return NULL;
}

static int avlAppend(char lastName[], entry **ppHead, entry **pE)
{
    int a = 0;
    avlinsert(ppHead,lastName,&a);
    return 0;
}

static int avlFree(entry *pHead)
{
    if(pHead!=NULL) {
        avlFree(GET_PRIV_PTR(avlpriv, pHead)->pLeft);
        avlFree(GET_PRIV_PTR(avlpriv, pHead)->pRight);
        free(pHead);
    }
    return 0;
}
static int avlinsert(entry **parent,char lastname[],int * unbalanced)
{
    if(*parent==NULL) {
        *unbalanced=1;
        *parent = ALLOC_ENTRY(avlpriv);
        GET_PRIV_PTR(avlpriv, *parent)->pLeft = NULL;
        GET_PRIV_PTR(avlpriv, *parent)->pRight = NULL;
        strcpy((*parent)->lastName, lastname);
        GET_PRIV_PTR(avlpriv, *parent)->bf=0;
    } else if(strcasecmp((*parent)->lastName,lastname)>0) {

        avlinsert(&(GET_PRIV_PTR(avlpriv, *parent)->pLeft),lastname,unbalanced);
        if(*unbalanced)
            switch(GET_PRIV_PTR(avlpriv, *parent)->bf) {
                case -1:
                    GET_PRIV_PTR(avlpriv, *parent)->bf=0;
                    *unbalanced=0;
                    break;
                case 0:
                    GET_PRIV_PTR(avlpriv, *parent)->bf=1;
                    break;
                case 1:
                    leftRotation(parent,unbalanced);
            }              //need to do Left r
    } else if(strcasecmp((*parent)->lastName,lastname)<=0) {
        avlinsert(&(GET_PRIV_PTR(avlpriv,*parent)->pRight),lastname,unbalanced);
        if(*unbalanced)
            switch(GET_PRIV_PTR(avlpriv, *parent)->bf) {
                case 1:
                    GET_PRIV_PTR(avlpriv, *parent)->bf=0;
                    *unbalanced=0;
                    break;
                case 0:
                    GET_PRIV_PTR(avlpriv, *parent)->bf=-1;
                    break;
                case -1:
                    rightRotation(parent,unbalanced);//   do right
            }

    }
    return 1;

}
static void leftRotation(entry **parent,int *unbalanced)
{
    entry *grandchild;
    entry *child;
    child =GET_PRIV_PTR(avlpriv, *parent)->pLeft;
    if(GET_PRIV_PTR(avlpriv, child)->bf==1) { // this is LL
        GET_PRIV_PTR(avlpriv, *parent)->pLeft=GET_PRIV_PTR(avlpriv, child)->pRight;
        GET_PRIV_PTR(avlpriv, child)->pRight=*parent;
        GET_PRIV_PTR(avlpriv, *parent)->bf=0;
        *parent=child;
    } else {
        //do lr rotation
        grandchild=GET_PRIV_PTR(avlpriv, child)->pRight;
        GET_PRIV_PTR(avlpriv, child)->pRight=GET_PRIV_PTR(avlpriv, grandchild)->pLeft;
        GET_PRIV_PTR(avlpriv, grandchild)->pLeft=child;
        GET_PRIV_PTR(avlpriv, *parent)->pLeft=GET_PRIV_PTR(avlpriv, grandchild)->pRight;
        GET_PRIV_PTR(avlpriv, grandchild)->pRight=*parent;
        switch(GET_PRIV_PTR(avlpriv, grandchild)->bf) {
            case 1:
                GET_PRIV_PTR(avlpriv, *parent)->bf=-1;
                GET_PRIV_PTR(avlpriv, child)->bf=0;
                break;
            case 0:
                GET_PRIV_PTR(avlpriv, *parent)->bf=GET_PRIV_PTR(avlpriv, child)->bf=0;
                break;
            case -1:
                GET_PRIV_PTR(avlpriv, *parent)->bf=0;
                GET_PRIV_PTR(avlpriv, child)->bf=1;
        }
        *parent=grandchild;

    }
    GET_PRIV_PTR(avlpriv, *parent)->bf=0;
    *unbalanced=0;
}
static void rightRotation(entry **parent,int *unbalanced)
{
    entry *grandchild;
    entry *child;
    child= GET_PRIV_PTR(avlpriv, *parent)->pRight;
    if(GET_PRIV_PTR(avlpriv, child)->bf==-1) {
        //rr
        grandchild=GET_PRIV_PTR(avlpriv, child)->pRight;
        GET_PRIV_PTR(avlpriv, *parent)->pRight=GET_PRIV_PTR(avlpriv, child)->pLeft;
        GET_PRIV_PTR(avlpriv, child)->pLeft=*parent;
        GET_PRIV_PTR(avlpriv, *parent)->bf=0;
        *parent=child;
    } else {
        //rl
        grandchild=GET_PRIV_PTR(avlpriv, child)->pLeft;
        GET_PRIV_PTR(avlpriv, child)->pLeft=GET_PRIV_PTR(avlpriv, grandchild)->pRight;
        GET_PRIV_PTR(avlpriv, grandchild)->pRight=child;
        GET_PRIV_PTR(avlpriv, *parent)->pRight=GET_PRIV_PTR(avlpriv, grandchild)->pLeft;
        GET_PRIV_PTR(avlpriv, grandchild)->pLeft = *parent;
        switch(GET_PRIV_PTR(avlpriv, grandchild)->bf) {
            case 1:
                GET_PRIV_PTR(avlpriv, *parent)->bf=-1;
                GET_PRIV_PTR(avlpriv, child)->bf=0;
                break;
            case 0:
                GET_PRIV_PTR(avlpriv, *parent)->bf=GET_PRIV_PTR(avlpriv, child)->bf=0;
                break;
            case -1:
                GET_PRIV_PTR(avlpriv, *parent)->bf=0;
                GET_PRIV_PTR(avlpriv, child)->bf=1;
        }
        *parent=grandchild;
    }
    GET_PRIV_PTR(avlpriv, *parent)->bf=0;
    *unbalanced=0;
}
