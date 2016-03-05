#include "memorypool.h"
#include <stdlib.h>
#include <math.h>

#define MAX_MP_HANDLE 10

typedef struct _MEMORYPOOL_DESC {
    void *data;
    int capacity;
    int size;
} mpdesc;

static mpdesc mpdescArr[MAX_MP_HANDLE];

int mpInit(size_t minCapacity)
{
    /* Find 2^exp >= minCapacity */
    unsigned int exp = (unsigned int)log2(minCapacity)+1;
    minCapacity = 1<<exp;

    /* Find available memory pool */
    int i;
    for(i=0; i<MAX_MP_HANDLE; i++) {
        if(mpdescArr[i].data==NULL) {
            mpdescArr[i].data = malloc(minCapacity);
            mpdescArr[i].capacity = minCapacity;
            mpdescArr[i].size = 0;
            return i;
        }
    }
    return -1;
}

void *mpAlloc(int handle, size_t size)
{
    if(mpdescArr[handle].data) {
        mpdescArr[handle].size+=size;
        if(mpdescArr[handle].size>mpdescArr[handle].capacity) {
            unsigned int exp = (unsigned int)log2(mpdescArr[handle].size)+1;
            mpdescArr[handle].capacity = 1<<exp;
            mpdescArr[handle].data = realloc(mpdescArr[handle].data, mpdescArr[handle].capacity);
        }
        return (unsigned char *)mpdescArr[handle].data+mpdescArr[handle].size-size;
    }
    return NULL;
}

void mpFree(int handle)
{
    free(mpdescArr[handle].data);
    mpdescArr[handle].data = NULL;
}
