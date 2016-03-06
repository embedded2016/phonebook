#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#ifdef THREAD
#include <unistd.h>
#include <pthread.h>
#endif

#include IMPL

#define DICT_FILE "./dictionary/words.txt"

#ifdef RUN_TEST
void runTest(entry *pHead);
#endif

#ifdef THREAD
char buf[MAX_BUFFER_SIZE][MAX_LAST_NAME_SIZE];
void *processArray(void * args);
#endif

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec - t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i = 0;
#ifdef THREAD
    pthread_t threads[NUM_OF_THREADS];
    thread_data_t thread_data[NUM_OF_THREADS];
    unsigned int buf_line = 0;
    unsigned int  thd_index = 0;
    unsigned int  line_start = 0;
    void *tret;
    int j = 0;
#else
    char line[MAX_LAST_NAME_SIZE];
#endif
    struct timespec start, end;
    double cpu_time1, cpu_time2;
    struct timespec start1, end1;
    double cpu_time3;
#ifdef RUN_TEST
    struct timespec start2, end2;
    double cpu_time4;
#endif
    clock_gettime(CLOCK_REALTIME, &start1);

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

#if defined(HASH1) || defined(HASH2)
    initHashTable();
#endif

    /* build the entry */
    entry *pHead, *e;
    pHead = (entry *) malloc(sizeof(entry));
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
#ifdef THREAD
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets((char *) &(buf[buf_line]), MAX_LAST_NAME_SIZE, fp)) {
        while (buf[buf_line][i] != '\0')
            i++;
        buf[buf_line][i - 1] = '\0';
        i = 0;
        buf_line++;

        if((buf_line % LINE_H) == 0) {
            if(thd_index >= NUM_OF_THREADS) {
                thd_index = 0;
                for (j = 0; j < NUM_OF_THREADS; j++) {
                    pthread_join(threads[j], &tret);
                }
            }
            thread_data[thd_index].start = line_start * LINE_H;
            thread_data[thd_index].end   = buf_line - 1;
            thread_data[thd_index].thd = thd_index;
            pthread_create(&threads[thd_index], NULL, processArray, (void *)&thread_data[thd_index]);
            line_start++;
            thd_index++;
        }
    }

    if((buf_line % LINE_H) != 0) {
        if(thd_index >= NUM_OF_THREADS) {
            thd_index = 0;
            pthread_join(threads[thd_index], &tret);
        }
        thread_data[thd_index].start = line_start * LINE_H;
        thread_data[thd_index].end   = buf_line - 1;
        thread_data[thd_index].thd   = thd_index;
        pthread_create(&threads[thd_index], NULL, processArray, (void *)&thread_data[thd_index]);
    }

    for (j = 0; j < NUM_OF_THREADS; j++) {
        pthread_join(threads[j], &tret);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);
#else /* else of THREAD */
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        e = append(line, e);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);
#endif /* end of THREAD */

    /* close file as soon as possible */
    fclose(fp);

    e = pHead;

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
    e = pHead;

    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    findName(input, e);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output;
#if defined(OPT)
    output = fopen("opt.txt", "a");
#elif defined(THD2)
    output = fopen("opt_thread2.txt", "a");
#elif defined(THREAD)
    output = fopen("opt_thread1.txt", "a");
#elif defined(HASH1)
    output = fopen("opt_hash1.txt", "a");
#elif defined(HASH2)
    output = fopen("opt_hash2.txt", "a");
#else
    output = fopen("orig.txt", "a");
#endif
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

#ifdef RUN_TEST
    clock_gettime(CLOCK_REALTIME, &start2);
    runTest(pHead);
    clock_gettime(CLOCK_REALTIME, &end2);
    cpu_time4 = diff_in_second(start2, end2);
#endif

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    if (pHead->pNext) free(pHead->pNext);
    free(pHead);

#if defined(HASH1) || defined(HASH2)
    freeHashTable();
#endif

    clock_gettime(CLOCK_REALTIME, &end1);
    cpu_time3 = diff_in_second(start1, end1);
    printf("execution time of total : %lf sec\n", cpu_time3);
#ifdef RUN_TEST
    printf("execution time of test() : %lf sec\n", cpu_time4);
#endif

    return 0;
}

#ifdef RUN_TEST
void runTest(entry *pHead)
{
    int i = 0;
    char test[9][MAX_LAST_NAME_SIZE] = {
        "uninvolved",
        "zyxel",
        "whiteshank",
        "odontomous",
        "pungoteague",
        "reweighted",
        "xiphisternal",
        "aaah",
        "yakattalo"
    };
    for (i = 0; i < 9; i++) {
        assert(0 == strcmp(findName(test[i], pHead)->lastName, test[i]));
    }
}
#endif

#ifdef THREAD
void *processArray(void *args)
{
    thread_data_t *data = (thread_data_t *)args;
    int start = data->start;
    int end   = data->end;
    int thd = data->thd;
    int i = 0;
    entry *e = NULL;

    for (i = start; i < end; i++) {
        e = append((char *) & (buf[i]), e, thd);
    }

    pthread_exit(NULL);
}
#endif

