#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "phonebook.h"
#include "memorypool.h"

#define DICT_FILE "./dictionary/words.txt"

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

static int mpHandle = -1;

static void *myAlloc(size_t size)
{
    return mpAlloc(mpHandle, size);
}

static void myFree(void *addr)
{
}

int main(int argc, char *argv[])
{
    if(argc != 3) {
        printf("Usage: phonebook <IMPL> <output>\n");
        printf("IMPL can be either linkedlist or avltree.\n");
        exit(1);
    }

    impl myImpl;
    assert(initImpl(&myImpl, argv[1]));

    FILE *fp;
    int i = 0;
    char line[MAX_LAST_NAME_SIZE];
    struct timespec start, end;
    double cpu_time1, cpu_time2;

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

    mpHandle = mpInit(sizeof(entry) * 300000);
    setMemoryFunc(myAlloc, myFree);

    /* build the entry */
    entry *pHead=NULL, *e=NULL;
    /*
    #if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
    #endif*/
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        myImpl.append(line, &pHead, &e);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    clock_gettime(CLOCK_REALTIME, &end);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    /* close file as soon as possible */
    fclose(fp);

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";

    assert(myImpl.findName(input, pHead) &&
           "Did you implement findName() ?");
    assert(0 == strcmp(myImpl.findName(input, pHead)->lastName, "zyxel"));

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i=0; i<100; i++)
        myImpl.findName(input, pHead);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output;
    output = fopen(argv[2], "a");
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    myImpl.free(pHead);

    mpFree(mpHandle);

    return 0;
}
