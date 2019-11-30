#include <stdio.h>
#include <mpi.h>

#define N 10

int main(int argc, char* argv[]){

	int size, rank, i, from, to, ndat, part, tag, VA[N], VR[N];
	MPI_Status info;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	//Inicializo vector
	for (i=0; i<N; i++) {
		VA[i] = 0;
        VR[i] = 0;
	}
    
	//Si soy maestro
    if (rank == 0){
		for (i=1; i<N; i++) {
			VA[i] = i;
		}
    }

    printf("Proceso %d: VA antes de enviar datos: \n", rank);
	for (i=0; i<N; i++) {
		printf("%d  ", VA[i]);
	}
	printf("\n\n");

    ndat = N/size;
    
    MPI_Scatter(&VA, ndat, MPI_INT, &VA, ndat, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Proceso %d: VA despues de recibir datos: \n", rank);
	for (i=0; i<N/size; i++) {
        VA[i] = VA[i]*2;
		printf("%d  ", VA[i]);
	}

    printf("\n\n");

    MPI_Gather(&VA, ndat, MPI_INT, &VR, ndat, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0){
        for (i=0; i<N; i++) {
		    printf("%d  ", VR[i]);
	    }

       printf("\n\n");

    }
	
	MPI_Finalize();
	return 0;
}