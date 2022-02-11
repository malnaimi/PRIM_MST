/************************************************************************
 * ECE7610 Final Project
 * Developer: Mostafa AlNaimi
 * Project: Prim's MST - Parallel.
 *
 *************************************************************************/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

/* Files to read and to write to */
FILE *file_matrix;
FILE *f_time;
FILE *file_result;

int *send_counts, *matrix_per_proc, *displs, *MST; 
int size, rank, rand_matrix, mst_size, min_weight;
    
/* Global Structure */
typedef struct { 
    int v1; 
    int v2; 
} edge_ver;

int main(int argc,char *argv[])
{
    int i,j,k;
    int *matrix;
    /* Initialize MPI functions */
    MPI_Init ( &argc, &argv );
    MPI_Comm_rank ( MPI_COMM_WORLD, &rank);
    MPI_Comm_size ( MPI_COMM_WORLD, &size );

    srand(time(NULL) + rank);
    
    /* First processor read the MST matrix */
    if (rank==0)
    {
        file_matrix = fopen("mst_matrix.txt", "r");
        if (file_matrix != NULL){
            fscanf(file_matrix, "%d\n", &mst_size);
        }
        /* If only one line, generate random Matrix */
        if (feof(file_matrix))
            rand_matrix = 1 ; 
        else 
            rand_matrix = 0 ;
    }

    /* Broadcast the size of the MST Matrix */
    MPI_Bcast(&mst_size, 1, MPI_INT, 0, MPI_COMM_WORLD);       

    /* Allocate temporary arrays */
    displs = (int*)malloc(sizeof(int) * size);   
    send_counts = (int*)malloc(sizeof(int) * size);

    displs[0] = 0;
    send_counts[0] = mst_size / size;
    
    /* Check the number of vertices if it's not a multiply of number of processors */
    int remainder_ver = size - (mst_size % size);
    for (i = 1; i < remainder_ver; ++i) 
    {
        send_counts[i] = send_counts[0];
        displs[i] = displs[i - 1] + send_counts[i - 1];
    }
    for (i = remainder_ver; i < size; ++i)
    {
        send_counts[i] = send_counts[0] + 1;
        displs[i] = displs[i - 1] + send_counts[i - 1];
    }

   /* Processor 0 reads infromation about the MST Matrix */
    if (rank==0) 
    {
        matrix = (int*)malloc(mst_size*mst_size*sizeof(int));
        for (i = 0; i < mst_size; ++i )
        {
            matrix[mst_size*i+j] = 0 ;
            for(j = i + 1; j < mst_size; ++j )
            {
                int buf;
                
                if (!rand_matrix) {
                    fscanf(file_matrix, "%d\n", &buf);
                    matrix[mst_size*i+j] = matrix[mst_size*i+j] = buf ;
                }
                
                else {
                    matrix[mst_size*i+j] = matrix[mst_size*i+j] = rand() % 20;
                }
            }
        }
        fclose(file_matrix);
    }
  
    /* Distribute Matrix */
    matrix_per_proc = (int*)malloc(send_counts[rank]*mst_size*sizeof(int));

    /* Create a datatype of the Matrix */
    MPI_Datatype matrixString; 
    MPI_Type_contiguous(mst_size,MPI_INT, &matrixString);
    MPI_Type_commit(&matrixString);
  
    /* Distribute to each processor */
    MPI_Scatterv(matrix,send_counts,displs,matrixString,matrix_per_proc,send_counts[rank],matrixString,0,MPI_COMM_WORLD);
  
    MST = (int*)malloc(sizeof(int)*mst_size);
    for ( i = 0; i < mst_size; ++i)
    {
        MST[i] = -1;
    }
  
    double time_start; 

    /* Start captring time */
    time_start = MPI_Wtime();
      
    MST[0] = 0;
    min_weight = 0;

    int min;
    int v1 = 0;
    int v2 = 0;

    struct { 
        int min; 
        int rank; 
    } min_row, row;
  
    edge_ver edge;
  
    for ( k = 0; k < mst_size - 1; ++k)
    {
        min = INT_MAX;
        for ( i = 0; i < send_counts[rank]; ++i)
        {
            if (MST[i + displs[rank]] != -1) 
            {
                for ( j = 0; j < mst_size; ++j)
                {
                    if (MST[j] == -1) 
                    {

                        if ( matrix_per_proc[mst_size*i+j] < min && matrix_per_proc[mst_size*i+j] != 0)
                        {
                          min = matrix_per_proc[mst_size*i+j];
                          v2 = j;
                          v1 = i;   
                        }
                    }
                }
            }
        }
        
        row.min = min;
        row.rank = rank;
        
        /* Preform All reduction operation */
        MPI_Allreduce(&row, &min_row, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD); 
        edge.v1 = v1 + displs[rank];
        edge.v2 = v2;
        MPI_Bcast(&edge, 1, MPI_2INT, min_row.rank, MPI_COMM_WORLD);

        MST[edge.v2] = edge.v1;
        min_weight += min_row.min;
    }
  
  
    double time_end, time_diff; 
  
    time_end = MPI_Wtime();
    time_diff = time_end-time_start;

    /* Write results to file */
    if (rank == 0) 
    {
        file_result = fopen("result.txt", "a+");
        fprintf(file_result, "\n Number of processors: %d\n Number of vertices: %d\n Time of execution: %f\n Total Weight: %d\n\n", size, mst_size ,time_diff, min_weight);
        fprintf(file_result,"The min_weight is %d\n", min_weight);
        
        for ( i=0; i< mst_size; ++i)
        {
            fprintf(file_result,"edge_ver %d %d\n",i, MST[i]);
        }
        fclose(file_result);
    }

    MPI_Finalize();
    
    return 0;

}
  