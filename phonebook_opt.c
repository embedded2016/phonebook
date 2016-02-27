#include <stdlib.h>
#include <string.h>

#include "phonebook_opt.h"

void initHash()
{
    memset(hash_table, 0, sizeof(hash_table));
}

int nameToHash(char *lastName)
{
    unsigned int hash_key = 0;
    unsigned int length = 0;
    unsigned int i;

    if(lastName == NULL)
        return -1;

    length = strlen(lastName);

    for(i=0; i<length; i++) {
        hash_key += ((lastName[i] - 'a') << (CHAR_SIZE << (length-i)));
    }

    return hash_key % (HASH_SIZE);
}

/* FILL YOUR OWN IMPLEMENTATION HERE! */
entry *findName(char lastname[], entry *pHead)
{
    /* TODO: implement */\
    unsigned int key;

    key = nameToHash(lastname);
    memcpy(pHead->lastName, hash_table[key].phoneBook.lastName, sizeof(hash_table[key].phoneBook.lastName));
    return pHead;
}

entry *append(char lastName[], entry *e)
{
    memcpy(hash_table[nameToHash(lastName)].phoneBook.lastName, lastName, sizeof(lastName));
    return e;
}
