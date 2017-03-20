#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *fp = fopen("orig.txt", "r");
    FILE *output = fopen("output.txt", "w");
    if (!fp) {
        printf("ERROR opening input file orig.txt\n");
        exit(0);
    }
    int i = 0;
    char append[50], find[50];
    double orig_sum_a = 0.0, orig_sum_f = 0.0, orig_a, orig_f;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %s %lf %lf\n", append, find, &orig_a, &orig_f);
        orig_sum_a += orig_a;
        orig_sum_f += orig_f;
    }
    fclose(fp);

    fp = fopen("opt.txt", "r");
    if (!fp) {
        fp = fopen("orig.txt", "r");
        if (!fp) {
            printf("ERROR opening input file opt.txt\n");
            exit(0);
        }
    }
    double opt_sum_a = 0.0, opt_sum_f = 0.0, opt_a, opt_f;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %s %lf %lf\n", append, find, &opt_a, &opt_f);
        opt_sum_a += opt_a;
        opt_sum_f += opt_f;
    }
    fclose(fp);

    fp = fopen("opt_hash1.txt", "r");
    if (!fp) {
        fp = fopen("orig.txt", "r");
        if (!fp) {
            printf("ERROR opening input file opt.txt\n");
            exit(0);
        }
    }
    double opt_hash1_sum_a = 0.0, opt_hash1_sum_f = 0.0, opt_hash1_a, opt_hash1_f;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %s %lf %lf\n", append, find, &opt_hash1_a, &opt_hash1_f);
        opt_hash1_sum_a += opt_hash1_a;
        opt_hash1_sum_f += opt_hash1_f;
    }
    fclose(fp);

    fp = fopen("opt_hash2.txt", "r");
    if (!fp) {
        fp = fopen("orig.txt", "r");
        if (!fp) {
            printf("ERROR opening input file opt.txt\n");
            exit(0);
        }
    }
    double opt_hash2_sum_a = 0.0, opt_hash2_sum_f = 0.0, opt_hash2_a, opt_hash2_f;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %s %lf %lf\n", append, find, &opt_hash2_a, &opt_hash2_f);
        opt_hash2_sum_a += opt_hash2_a;
        opt_hash2_sum_f += opt_hash2_f;
    }
    fclose(fp);

    fp = fopen("opt_thread1.txt", "r");
    if (!fp) {
        fp = fopen("orig.txt", "r");
        if (!fp) {
            printf("ERROR opening input file opt.txt\n");
            exit(0);
        }
    }
    double opt_thd_sum_a = 0.0, opt_thd_sum_f = 0.0, opt_thd_a, opt_thd_f;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %s %lf %lf\n", append, find, &opt_thd_a, &opt_thd_f);
        opt_thd_sum_a += opt_thd_a;
        opt_thd_sum_f += opt_thd_f;
    }
    fclose(fp);

    fp = fopen("opt_thread2.txt", "r");
    if (!fp) {
        fp = fopen("orig.txt", "r");
        if (!fp) {
            printf("ERROR opening input file opt.txt\n");
            exit(0);
        }
    }
    double opt_thd2_sum_a = 0.0, opt_thd2_sum_f = 0.0, opt_thd2_a, opt_thd2_f;
    for (i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %s %lf %lf\n", append, find, &opt_thd2_a, &opt_thd2_f);
        opt_thd2_sum_a += opt_thd2_a;
        opt_thd2_sum_f += opt_thd2_f;
    }
    fclose(fp);

    fprintf(output, "append() %lf %lf %lf %lf %lf %lf\n", \
            orig_sum_a / 100.0, opt_sum_a / 100.0, opt_hash1_sum_a / 100.0, opt_hash2_sum_a / 100.0, \
            opt_thd_sum_a / 100.0, opt_thd2_sum_a / 100.0);
    fprintf(output, "findName() %lf %lf %lf %lf %lf %lf\n", \
            orig_sum_f / 100.0, opt_sum_f / 100.0, opt_hash1_sum_f / 100.0, opt_hash2_sum_f / 100.0, \
            opt_thd_sum_f / 100.0, opt_thd2_sum_f / 100.0);
    fprintf(output, "total %lf %lf %lf %lf %lf %lf", (orig_sum_a + orig_sum_f) / 100.0, (opt_sum_a + opt_sum_f) / 100.0,
            (opt_hash1_sum_a + opt_hash1_sum_f) / 100.0, (opt_hash2_sum_a + opt_hash2_sum_f) / 100.0,
            (opt_thd_sum_a + opt_thd_sum_f) / 100.0, (opt_thd2_sum_a + opt_thd2_sum_f) / 100.0);
    fclose(output);
    return 0;
}
