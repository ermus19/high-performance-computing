#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define N 8

int calcularMaximo(int A[], int tam);

int main(int argc, char *argv[])
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 12)
    {
        printf("Please run with 12 processes.\n");fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    //int mR[N][N], mG[N][N], mB[N][N];
    int mRGB[N][N][3];

    MPI_Comm cart;
    int dim[3], period[3], reorder, procs_dim;
    int coord[3], id;
    double t0, total;

    srand(time(NULL));

    dim[0] = 2; dim[1] = 2; dim[2] = 3;
    period[0] = 0; period[1] = 0; period[2] = 0;
    reorder = 1;

    MPI_Cart_create(MPI_COMM_WORLD, 3, dim, period, reorder, &cart);

    //El maestro inicializa matrices
    if (rank == 0)
    {

        printf("Proceso maestro, inicializa matrices\n");

        for(int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                //mR[i][j] = rand() % 255;
                //mG[i][j] = rand() % 255;
                //mB[i][j] = rand() % 255;
                for (int k = 0; k < 3; k++){
                    mRGB[i][j][k] = rand() % 255;
                }
            }
        }
    }

    //Tipo de datos que representa cuadrante
    MPI_Datatype cuadrante;
    MPI_Type_vector(N/2, N/2, N, MPI_INT, &cuadrante);
    MPI_Type_commit(&cuadrante);

    //Esperamos sincronizacion de todos los procesos
    MPI_Barrier(MPI_COMM_WORLD);

    //Se toma tiempo inicial
    t0 = MPI_Wtime();

    //Maestro reparte cuadrantes de matriz a la topologia y calcula su cuadrante
    if (rank == 0)
    {

        printf("Proceso maestro: distribuye matrices\n");
        int cart_rank;
        

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 3; k++){

                    coord[0] = i; 
                    coord[1] = j;
                    coord[2] = k;
                    
                    //Calculo del rank correspondiente a las coordenadas
                    MPI_Cart_rank(cart, coord, &cart_rank);

                    if(cart_rank != 0)
                    {

                        printf("Maestro envia cuadrante %d %d %d de matriz RGB a proceso en coordenadas %d %d %d\n", coord[0] * N/2, coord[1] * N/2, coord[2], coord[0], coord[1], coord[2]);
                        MPI_Send(&mRGB[coord[0] * N/2][coord[1] * N/2][coord[2]], 1, cuadrante, cart_rank, 0, MPI_COMM_WORLD);

                    } else {

                        
                        int V[2 * N], maxMaestro;
                        int p = 0;

                        for (int z = 0; z < N/2; z++)
                        {
                            for(int x = 0; x < N/2; x++)
                            {
                                V[p] = mRGB[z][x][0];
                                p++;
                            }

                        }

                        maxMaestro = calcularMaximo(V, 2 * N);
                        printf("Soy maestro y calculo mi maximo %d\n", maxMaestro);

                    }

                }
               
            }
        }

        MPI_Type_free(&cuadrante);

    //Esclavo 
    } else {

        int V[2 * N], max;

        //Procesos diferentes al maetro reciben los bloques en vector y calculan maximo de su cuadrante
        MPI_Recv(&V, 2 * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        max = calcularMaximo(V, 2 * N);
        printf("Proceso: %d, maximo %d\n", rank, max);
        for(int i = 0; i < 2 * N; i++)
        {
            printf("%d ", V[i]);
        }
        printf("\n");

    }


    MPI_Finalize();
    return 0;
}

int calcularMaximo(int A[], int tam)
{
    int i, max;
    max = A[0];

    for (i = 1; i < tam; i++)
    {
        if (A[i] > max) {
            max = A[i];
        }
    }

    return max;
}