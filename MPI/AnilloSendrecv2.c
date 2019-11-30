#include <stdio.h>
#include <mpi.h>

#define N 10

int main(int argc, char* argv[]){

	int size, rank, i, from, to, ndat, part, tag, VA[N], VR[N];
	MPI_Status info;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    MPI_Request reqs[2];
    MPI_Status stats[2];
	
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    char local_name[MPI_MAX_PROCESSOR_NAME];
    int name_len = MPI_MAX_PROCESSOR_NAME;
    
    //printf("Tamaño antes: %d", name_len);
    
	//Si soy maestro
    if (rank == 0){
        //Recibo el mensaje
        MPI_Get_processor_name(processor_name, &name_len);

        MPI_Sendrecv(&processor_name, name_len + 1, MPI_CHAR, (rank + 1), 0, &local_name, name_len + 1, MPI_CHAR, (size - 1), 0, MPI_COMM_WORLD, &stats[0]);

        printf("Proceso %d recibe mensaje %s del proceso %d\n", rank, local_name, size - 1);	

    } else if (rank == size - 1){
	    MPI_Get_processor_name(processor_name, &name_len);

        MPI_Sendrecv(&processor_name, name_len + 1, MPI_CHAR, 0, 0, &local_name, name_len + 1, MPI_CHAR, (rank - 1), 0, MPI_COMM_WORLD, &stats[0]);
        
        printf("Proceso %d recibe mensaje %s del proceso %d\n", rank, local_name, rank - 1); 
                 
    } else {
        MPI_Get_processor_name(processor_name, &name_len);

        MPI_Sendrecv(&processor_name, name_len + 1, MPI_CHAR, (rank + 1), 0, &local_name, name_len + 1, MPI_CHAR, (rank - 1), 0, MPI_COMM_WORLD, &stats[0]);
        
        printf("Proceso %d recibe mensaje %s del proceso %d\n", rank, local_name, rank - 1);  
    }

	MPI_Finalize();
	return 0;
}