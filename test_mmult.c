#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#include "mat.h"

#define MAT_SIZE 5
struct timespec start;
struct timespec end;
double count_time[2];

int test_unoptimized(double *a, int arows, int acols,
                     double *b, int brows, int bcols,
                     double *c_actual) {
    double *c_calc = malloc(MAT_SIZE * MAT_SIZE * sizeof(double));
       
    	clock_gettime(CLOCK_REALTIME, &start);
    mmult(c_calc, a, MAT_SIZE, MAT_SIZE, b, MAT_SIZE, MAT_SIZE);
        clock_gettime(CLOCK_REALTIME, &end);
    count_time[0] = deltaTime(&start, &end);
    printf("The matrix size is %d, and the time is  %f ", MAT_SIZE, count_time[0]);
        
    int are_same = compare_matrices(c_actual, c_calc, MAT_SIZE, MAT_SIZE);

    free(c_calc);

    return are_same;
}

int main(void) {
    double *a = read_matrix_from_file("a.txt");
    double *b = read_matrix_from_file("b.txt");
    double *c_actual = read_matrix_from_file("c.txt");
    double *c_calc = malloc(MAT_SIZE * MAT_SIZE * sizeof(double));
        
    if(!test_unoptimized(a, MAT_SIZE, MAT_SIZE, b, MAT_SIZE, MAT_SIZE, c_actual)) {
        exit(1);
    }
         clock_gettime(CLOCK_REALTIME, &start);
         mmult_simd(c_calc, a, MAT_SIZE, MAT_SIZE, b, MAT_SIZE, MAT_SIZE);
         clock_gettime(CLOCK_REALTIME, &end);
         count_time[1] = deltaTime(&start, &end);
         printf("The SIMD matrix size is %d, and the time is %f ", MAT_SIZE, count_time[1]);
    puts("All tests pass.");

    free(a);
    free(b);
    free(c_actual);
}
