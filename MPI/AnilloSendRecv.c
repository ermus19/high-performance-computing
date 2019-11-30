#include <stdio.h>
#include <mpi.h>

#define N 10

int main(int argc, char* argv[]){

	int size, rank, i, from, to, ndat, part, tag, VA[N], VR[N];
	MPI_Status info;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len = MPI_MAX_PROCESSOR_NAME;
    
    //printf("Tamaño antes: %d", name_len);

	//Si no soy maestro
    if (rank != 0){
        //Recibo el mensaje
        MPI_Get_processor_name(processor_name, &name_len);
        MPI_Recv(&processor_name, name_len + 1, MPI_CHAR, (rank - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proceso %d recibe mensaje %s del proceso %d\n", rank, processor_name, rank - 1);
		
    } else {
        //Inicializo mensaje
	    MPI_Get_processor_name(processor_name, &name_len);
        //printf("Longitud del mensaje: %d\n", name_len);
    }

    MPI_Send(&processor_name, name_len, MPI_CHAR, ((rank + 1) % size), 0, MPI_COMM_WORLD);

    if (rank == 0) {
        MPI_Recv(&processor_name, name_len + 1, MPI_CHAR, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proceso %d recibe mensaje %s del proceso %d\n", rank, processor_name, size - 1);
    }

	MPI_Finalize();
	return 0;
}