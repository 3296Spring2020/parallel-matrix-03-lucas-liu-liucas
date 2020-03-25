Mark Heimann, Stephen Lucas, Aaron Liu
Lab 6 Parallel Computing

1. We using a loop as the argument for main function that start with the matrix size of 100 to 2000, each loop will increase the size by 100. Also, we create a file to store the data for the time needs to do the multiplication for two matrixes and the size of the matrixes. For the master, we broadcast the second matrix and then store the rows of the first matrix to buffer, then send the buffer to the slave to do the calculation then send the answer back to the master. Then write the times to the file. For the slave, we broadcast the second matrix, then loop throught the receive method to get the buffer still the end. Then, use omp method to calculate the answer, then use the send method to send the answer back to the master. Then, we close the file and finalize the MPI.

2.
Contributions:
Mark completed the day one task of running matrix multiplication non-vectorized in C as well as creating a vectorized version.
Mark also completed the Matrix multiplication parallelization in Java.

Aaron
implement the mpi_omp method and the modify the omp_timing file, so it doesn't need to take any argument in the comman line, it will automatically loop from matrix size of 100 to 2000. Also, I made it to output the data to the text file for  the purpose of generating the graph.

Stephen

When wanting to review each other's work we would voice chat and screenshare, so we could see exactly what was happening with the changes being made.

3.
The typical cycle of :write, compile, run, test code was thrown off a bit with this project.  Requiring development to be on Wolfgand cluster through off our
typical coding environment, so we were forced to adapt.
