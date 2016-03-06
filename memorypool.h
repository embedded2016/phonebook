#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_

#include <stddef.h>

/* Return the handle to memory pool */
int mpInit(size_t minCapacity);
void *mpAlloc(int handle, size_t);
void mpFree(int);

#endif
