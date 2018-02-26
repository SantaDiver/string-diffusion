#include <stdio.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int rank;
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(comm, &rank);

#pragma omp parallel
    printf("(%04d, %d): Hello, world!\n", rank, omp_get_thread_num());

    MPI_Finalize();
    return 0;
}
