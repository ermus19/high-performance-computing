#include <mpi.h>
#include <time.h>
#include <stdio.h>

//#define A 1000000
#define A 10000
#define B 1000

int calculaSumaPrimos(int V[], int n);
int esPrimo(int n);

int main(int argc, char *argv[])
{

    int size, rank, tag1 = 1, tag2 = 2, VA[A], VB[B];
    int sumaMaestro = 0, sumaEsclavo = 0, sumasEsclavos = 0;

    MPI_Request reqs[2];
    MPI_Status stats[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));

    if (rank == 0)
    {
        //Inicializacion de vector maestro con valores aleatorios entre 1 y 1000000
        for (int i = 0; i < A; i++)
        {
            VA[i] = rand() % 1000000 + 1;
        }

        //Definicion de numero de paquetes a enviar y posicion inicial (se supone divisible)
        int paqs = A / B;
        int pos = 0, sentPaqs = 0, recvPaqs = 0;

        printf("Maestro envia %d paquetes de %d para un total de %d\n", paqs, B, A);

        //Proceso maestro envía los datos a todos los esclavos disponibles
        for(int i = 1; i < size; i++)
        {
            MPI_Isend(&VA[pos], B, MPI_INT, i, tag1, MPI_COMM_WORLD, &reqs[0]);
            printf("Proceso maestro enviando paquete %d, vector posicion inicial %d a proceso %d\n", sentPaqs, pos, i);
            pos+=B;
            sentPaqs++;
        }  

        //Proceso maestro calcula suma total de elementos
        printf("Maestro calcula suma de primos...\n");
        sumaMaestro = calculaSumaPrimos(VA, A);
        printf("Proceso maestro calcula suma de  primos: %d\n", sumaMaestro);

        //Proceso maestro espera a que se envíen los datos
        MPI_Wait(&reqs[0], &stats[0]);


        //Proceso maestro recibe calculo de cualquier esclavo mientras haya mensajes que recibir
        while(1)
        {
            int source = 0;
            printf("Maestro intenta recibir datos de cualquier esclavo\n");
            MPI_Recv(&sumaEsclavo, 1, MPI_INT, MPI_ANY_SOURCE, tag1, MPI_COMM_WORLD, &stats[0]);
            source = stats[0].MPI_SOURCE;
            recvPaqs++;
            sumasEsclavos+=sumaEsclavo;
            printf("Proceso maestro recibe suma de proceso %d : %d\n", source, sumaEsclavo);
            printf("Proceso maestro actualiza suma de esclavos %d\n", sumasEsclavos);

            if (sentPaqs < paqs)
            {
                printf("%d paquetes enviados de %d paquetes\n", sentPaqs, paqs);
                printf("Como hay mas trabajo por procesar, maestro envia mas datos\n");
                MPI_Send(&VA[pos], B, MPI_INT, source, tag1, MPI_COMM_WORLD);
                printf("Proceso maestro enviando paquete %d, vector posicion inicial %d a esclavo %d\n", sentPaqs, pos, source);
                pos+=B;
                sentPaqs++;

            }


            if (recvPaqs < paqs){

                printf("Proceso maestro no ha recibido todos los paquetes %d de %d\n", recvPaqs, paqs);

            } else {

                printf("Proceso maestro ya ha recibido todos los paquetes %d de %d, finalizando...\n", recvPaqs, paqs);
                break;

            }

        }

        //No hay mas trabajo, maestro envia mensaje de finalizacion por tag 2
        for(int i = 1; i < size; i++)
        {
            MPI_Isend(&VA[pos], B, MPI_INT, i, tag2, MPI_COMM_WORLD, &reqs[0]);
            printf("Proceso maestro enviando mensajed de finalizacion a proceso %d\n", i);
        }  
        printf("Proceso maestro ha enviado mensaje de finalizacion a todos los esclavos\n");
        printf("Paquetes enviados: %d\n", sentPaqs);
        printf("Paquetes recibidos: %d\n", recvPaqs);
        printf("Suma esclavos: %d\n", sumasEsclavos);
        printf("Suma maestro: %d\n", sumaMaestro);

    } else {

        int fin = 0, suma = 0;
        //Inicializacion de vector esclavo con 0
        for (int i = 0; i < B; i++)
        {
            VB[i] = 0;
        }

        //Mientras esclavo no recibe mensaje de fin, intenta recibir datos
        while(!fin)
        {
            suma = 0;
            //Proceso esclavo se prepara para recibir datos
            MPI_Recv(&VB[0], B, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stats[1]);
            printf("Esclavo %d recibe datos de maestro y comprueba si procesar datos o finalizar\n", rank);
            
            if(stats[1].MPI_TAG == 1){
                suma = calculaSumaPrimos(VB, B);
                printf("Esclavo %d calcula suma de primos %d\n", rank, suma);
                MPI_Send(&suma, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD);

            } else if (stats[1].MPI_TAG == 2) {
                printf("Esclavo recibe mensaje de fin\n");
                break;
            }

        }
        
    }

    MPI_Finalize();
    return 0;
}

//Funcion que recorre cada elemento y comprueba si es primo del un vector
int calculaSumaPrimos(int V[], int n)
{
    int sumaTotal = 0;

    for (int i = 0; i < n; i++)
    {
        if(esPrimo(V[i]))
            sumaTotal++;
    }

    return sumaTotal;
}

//Funcion que comprueba si la entrada es un numero primo
int esPrimo(int n) 
{

    if (n < 2) {
        return 0;
    }

    for(int i = 2; i < n ; ++i) {
        if (n % i == 0) {
            //divisor encontrado
            return 0;
        }
    }
    //No se han encontrado divisores
    return 1; 
}