/**
 * This file compares the time to multiply matrices
 * both unoptimized and with OMP. 
 */

#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>

#include "mat.h"

int main(int argc, char* argv[]) {
    clock_t start;
    clock_t end;
    clock_t res;
    double *a, *b, *c1, *c2, *c3;
    int n;
    double times[3];
    if (argc > 1) {       
        FILE * fp;
        fp = fopen ("Data.txt","w");
        if(fp == NULL)
        {
            /* File not created hence exit */
            printf("Unable to create file.\n");
            exit(1);
        }
        n = atoi(argv[1]);
        do{
		a = gen_matrix(n, n);
        b = gen_matrix(n, n);
        c1 = malloc(sizeof(double) * n * n);
        c2 = malloc(sizeof(double) * n * n);
		c3 = malloc(sizeof(double) * n * n);
        start = clock();
        mmult(c1, a, n, n, b, n, n);
        end = clock();
        // time_spent = end - start
        times[0] = (double)(end - start)/CLOCKS_PER_SEC;
        printf("%d", n);
        printf(" ");
        printf("%f", times[0]);
        start = clock();
        mmult_omp(c2, a, n, n, b, n, n);
        end = clock();
        times[1] = (double)(end - start)/CLOCKS_PER_SEC;
        printf(" %f", times[1]);
        start = clock();
        mmult_simd(c3, a, n, n, b, n, n);
        end = clock();
        times[2] = (double)(end - start)/CLOCKS_PER_SEC;
        printf("%d %f", n, times[2]);       
        printf("\n");
        compare_matrices(c1, c2, n, n);
		compare_matrices(c1, c3, n, n);
        fprintf(fp, "%d", n);
        fprintf(fp, " ");
        fprintf(fp, "%f", times[0]);
        fprintf(fp, "%f", times[1]);
        fprintf(fp, "%f\n", times[2]);
		n += 100;
		}while(n < 2000);
        fclose (fp);
    } else {
        fprintf(stderr, "Usage %s <n>\n", argv[0]);
    }
}
