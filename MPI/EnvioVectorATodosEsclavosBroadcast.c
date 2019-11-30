#include <stdio.h>
#include <mpi.h>

#define N 10

// Ruben Rodriguez

int main(int argc, char* argv[]){

	int size, rank, i, j, from, to, ndat, part, tag, VA[N];
	MPI_Status info;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	//Inicializo el vector
	for (i=0; i<N; i++) {
		VA[i] = 0;
	}
	
	
	//Si soy maestro
	if (rank == 0){
		for (i=1; i<N; i++) {
			VA[i] = i;
		}
		
	}


	printf("Proceso %d: VA antes de recibir datos: \n", rank);
		for (i=0; i<N; i++) {
			printf("%d  ",VA[i]);
		}
		printf("\n\n");
	

	MPI_Bcast(&VA, N, MPI_INT, 0, MPI_COMM_WORLD);
	
							
	printf("Proceso %d despues de recibir datos:  \n", rank);
		
	for (i=0; i<N; i++) {
		printf("%d  ",VA[i]);
	}

	printf("\n\n");

	MPI_Finalize();
	return 0;
}
