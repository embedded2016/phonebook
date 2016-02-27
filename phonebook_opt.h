#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#define MAX_LAST_NAME_SIZE 16

#define HASH_SIZE (349900+1)
#define CHAR_SIZE 5

/* TODO: After modifying the original version, uncomment the following
 * line to set OPT properly */
#define OPT 1

typedef struct __PHONE_BOOK_DATA {
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
    struct __PHONE_BOOK_ENTRY *pNext;
} phone_book_t;

typedef struct __PHONE_BOOK_ENTRY {
    char lastName[MAX_LAST_NAME_SIZE];
    struct __PHONE_BOOK_ENTRY *pNext;
} entry;

typedef struct hash_table_s {
    phone_book_t phoneBook;
} hash_table_t;

hash_table_t hash_table[HASH_SIZE];

entry *findName(char lastname[], entry *pHead);
entry *append(char lastName[], entry *e);

void initHash();

#endif
