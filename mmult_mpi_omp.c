/**
 * This program multiplies a matrix aa times a vector b to produce a vector c using MPI.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include<string.h>

#include "mat.h"

#define min(x, y) ((x)<(y)?(x):(y))

int main(int argc, char* argv[])
{
	FILE *fp;

	int i, d;

	fp = fopen("MPI_OMP.txt", "w");

	if (fp == NULL)

	{

		printf("File cannot open! ");

		exit(0);

	}
    int nrows, ncols;

    double *aa;
	double *bb;
	double *cc1;	/* A x B computed using the omp-mpi code you write */
	double *cc2;	/* A x B computed using the conventional algorithm */

    double *buffer, *ans;
	char buffer2[1024];
    double *times;
    double total_times;

    int run_index;
    int nruns;
    int myid, master, numprocs;
    
   struct timeval start,end;
    MPI_Status status;
    int j, numsent, sender;
    int anstype, row;
	double durTime[1];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    for (int Msize = 100; Msize <=2000; Msize +=100) {
        nrows = Msize;
        ncols = nrows;
		
        aa = (double*)malloc(sizeof(double) * nrows * ncols);
		bb = gen_matrix(nrows, ncols);
       
        buffer = (double*)malloc(sizeof(double) * ncols);
		ans = (double*)malloc(sizeof(double) * ncols);
		cc1 = (double*)malloc(sizeof(double) *nrows * nrows);
		cc2 = (double*)malloc(sizeof(double) *nrows * nrows);
		
        master = 0;
        if (myid == master) {
            // Master Code goes here
            aa = gen_matrix(nrows, ncols);
            
			
		printf("Matrix Size \n");
		
		
            gettimeofday(&start,NULL);
            numsent = 0;
			MPI_Bcast(bb, ncols*ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);

            for (i = 0; i < min(numprocs-1, nrows); i++) {
                for (j = 0; j < ncols; j++) {
                    buffer[j] = aa[i * ncols + j];
                }  
                MPI_Send(buffer, ncols, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
                numsent++;
            }
            for (i = 0; i < nrows; i++) {
                MPI_Recv(ans, ncols, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, 
                    MPI_COMM_WORLD, &status);
                sender = status.MPI_SOURCE;
                anstype = status.MPI_TAG;
				
				
				for(int m=0;m<ncols;m++){
					
					cc1[(anstype-1)*ncols + m] = ans[m];
					
				}

				
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
            gettimeofday(&end,NULL);

			durTime[0] = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;


			gettimeofday(&start,NULL);
			mmult(cc2, aa, nrows, ncols, bb, nrows, nrows);
			gettimeofday(&end,NULL);
			durTime[1] = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
			char temp[50];
			
			sprintf(temp, "%d", Msize);
			strcpy(buffer2, temp);
			strcat(buffer2, ",");
			sprintf(temp, "%.2f", durTime[0]);
			strcat(buffer2, temp);
			strcat(buffer2, ",");
			sprintf(temp, "%.2f", durTime[1]);
			strcat(buffer2, temp);	
			fprintf(fp, "%s\n", buffer2);
			
			
			if( compare_matrices(cc1, cc2, nrows, nrows)) {
				printf("Matrix Size %d:\n",nrows);
				printf("No MPI:%f\n",durTime[1]);
				printf("With MPI:%f\n",durTime[0]);
			}
			
            
        } else {
            // Slave Code goes here
            MPI_Bcast(bb, ncols*ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
            if (myid <= nrows) {
				
                while(1) {
				//	printf("\n%d\n",myid);
                    MPI_Recv(buffer, ncols, MPI_DOUBLE, master, MPI_ANY_TAG, 
                        MPI_COMM_WORLD, &status);
                    if (status.MPI_TAG == 0){
                        break;
                    }
                    row = status.MPI_TAG;
                    //ans = 0.0;
					
					for(int k = 0; k < ncols; k++){
						ans[k]=0.0;
						
					}
#pragma omp for
					for (int k = 0; k < ncols; k++) {
						for (j = 0; j < ncols; j++) {
							//c[i*bCols + j] += a[i*aCols + k] * b[k*bCols + j];
							ans[j] += buffer[k] * bb[k*ncols + j];
							
						}
					}
				
					
                    MPI_Send(ans, ncols, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
                }
            }
        }
 

    } 
	fclose(fp);
    MPI_Finalize();
    return 0;
}