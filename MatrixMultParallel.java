package matrixmultparallel;

import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


/**
 *
 * @author Mark
 */
public class MatrixMultParallel {

    
    public final static int matrixSize = 250; //Random size of matrix

    public static void main(String[] args) throws InterruptedException {

        
        //create matrices A,B, to be multiplied
        int[][] A = new int[matrixSize][matrixSize];
        int[][] B = new int[matrixSize][matrixSize];
        //leave C empty for now (leave open for answer)
        //Ccalc is our reference answer, to ensure parallelization was correct
        int[][] Ccalc = new int[matrixSize][matrixSize];
        //calculate C using parallelization
        int[][] C = new int[matrixSize][matrixSize];
        
        Random r = new Random();
        
        //initialize Ccalc and C
        for (int i = 0; i < matrixSize; i++){
            for (int j = 0; j < matrixSize; j++){
                A[i][j] = r.nextInt(10);
                B[i][j] = r.nextInt(10);
                Ccalc[i][j] = 0;
                C[i][j] = 0;
            }
        }
        //start timer for calculating answer matrix w/o parallelization
        long startTime1 = System.currentTimeMillis();
        //calculate the answer matrix
        for (int i = 0; i < matrixSize; i++){
            for (int j = 0; j < matrixSize; j++){
                for(int k = 0; k < matrixSize; k++){
                    Ccalc[i][j] += A[i][k]* B[k][j];
                }
            }
        }
        //stop timer
        long endTime1 = System.currentTimeMillis();
        
        
        

        //create executorservice object
        ExecutorService calculations = Executors.newCachedThreadPool();
        
        //start timer for parallelized multiplication
        long startTime2 = System.currentTimeMillis();
            for(int i = 0; i < matrixSize; i++) {
                    int[] Bcol = getCol(B,i);
                    //for every column, submit a new thread to calculate
                    //a column of the answer matrix
                    calculations.submit(new Multi(matrixSize,A,Bcol,C, i));
            }
            //normal shutdown
            calculations.shutdown();     
            //end timer
            long endTime2 = System.currentTimeMillis();
            
            
            System.out.println("Calculation(w/o parallelization) completed in " +
                         (endTime1 - startTime1) + " milliseconds");
            
            System.out.println("Calculation(with parallelization) completed in " +
                         (endTime2 - startTime2) + " milliseconds");
            
        //printouts of matrices to ensure calculations are correct
        /*  
        System.out.println("A:");
        printMatrix(A);
        System.out.println("\nB:");
        printMatrix(B);
        System.out.println("Ccalc:");
        printMatrix(Ccalc);
        System.out.println("\nCpar:");
        printMatrix(C);
        */
    }
    static class Multi implements Runnable {
        final int matrixSize;
        final int [][] A;
        final int [] Bcol;
        final int [][] C;
        final int col;

            public Multi(int matrixSize,  int[][] A, int[] Bcol, int[][] C, int col){           
                this.matrixSize=matrixSize;
                this.A=A;
                this.Bcol=Bcol;
                this.C=C;
                this.col = col;
            }


        //run method, calculates on column of answer matrix
        @Override
        public void run() {
            //for each column, we multiply by every row to determine the answer matrix 
            for (int i = 0; i < matrixSize; i++){
                for(int k = 0; k < matrixSize; k++){
                    C[i][col] += A[i][k]* Bcol[k];
                }               
            }
        }
    }
    
    //simple method to pull a column out of a matrix
    public static int[] getCol(int[][] matrix, int index){
        int[] col = new int[matrix[0].length];
        for(int i = 0; i < col.length; i++){
            col[i] = matrix[i][index];
        }
        return col;
    }
    
    //simple method to print out a matrix
    public static void printMatrix(int mat[][]) 
    { 
        for (int[] row : mat) 
  
            // converting each row as string 
            // and then printing in a separate line 
            System.out.println(Arrays.toString(row)); 
    } 
    
}
