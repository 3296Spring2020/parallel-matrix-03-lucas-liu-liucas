/** 
 * Incomplete program to multiply a matrix times a matrix using both
 * MPI to distribute the computation among nodes and OMP
 * to distribute the computation among threads.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>xi
#define min(x, y) ((x)<(y)?(x):(y))

#include "mat.h"

int main(int argc, char* argv[])
{
    int nrows, ncols, i, j;
    double *aa;	/* the A matrix */
    double *bb;	/* the B matrix */
    double *cc1;	/* A x B computed using the omp-mpi code you write */
    double *cc2;	/* A x B computed using the conventional algorithm */
    int myid, numprocs, numsent, sender, anstype, row, master;
    double starttime, endtime，ans, *buffer;

    MPI_Status status;

    /* insert other global variables here */

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    if (argc > 1) {
        nrows = atoi(argv[1]);
        ncols = nrows;
        master = 0;
        numsent = 0;
        if (myid == master) {
            // Master Code goes here
            aa = gen_matrix(nrows, ncols);
            bb = gen_matrix(ncols, nrows);
            cc1 = malloc(sizeof(double) * nrows * nrows); 
            buffer = malloc(sizeof(double) * nrows * ncols);
            starttime = MPI_Wtime();
            /* Insert your master code here to store the product into cc1 */
	        MPI_Bcast(bb, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
            for(i = 0; i < min(numprocs-1, nrows); i++){
                for(j = 0; j < nrows; j++){
                    buffer[j] = aa[i * ncols + j];
                }
                MPI_Send(buffer, nrows, MPI_DOUBLE, myid + 1, myid + 1, MPI_COMM_WORLD);
                numsent++;
            }

            for(i = 0; i < nrows; i++){
                MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, 
                    MPI_COMM_WORLD, &status);
                sender = status.MPI_SOURCE;
                anstype = status.MPI_TAG;
                c[anstype-1] = ans;
                if (numsent < nrows) {
                    for (j = 0; j < ncols; j++) {
                        buffer[j] = aa[numsent*ncols + j];
                    }  
                    MPI_Send(buffer, ncols, MPI_DOUBLE, sender, numsent+1, 
                        MPI_COMM_WORLD);
                    numsent++;
                } else {
                    MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
                }
            }
            endtime = MPI_Wtime();
            printf("%f\n",(endtime - starttime));
            cc2  = malloc(sizeof(double) * nrows * nrows);
            mmult(cc2, aa, nrows, ncols, bb, ncols, nrows);
            compare_matrices(cc2, cc1, nrows, nrows);
        } else {
            // Slave Code goes here
            MPI_Bcast(bb, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
            if (myid <= nrows) {
	            while(1) {
	                MPI_Recv(buffer, ncols, MPI_DOUBLE, master, MPI_ANY_TAG, 
		                MPI_COMM_WORLD, &status);
	                if (status.MPI_TAG == 0){
	                    break;
                    }
	                row = status.MPI_TAG;
	                ans = 0.0;
#pragma omp parallel
#pragma omp shared(ans) for reduction(+:ans)
	                for (j = 0; j < ncols; j++) {
	                    ans += buffer[j] * b[j];
	                }
	                MPI_Send(&ans, 1, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
	            }
            }
        }
    } else {
        fprintf(stderr, "Usage matrix_times_vector <size>\n");
    }
    MPI_Finalize();
    return 0;
}
