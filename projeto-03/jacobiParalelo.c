//
//  jabociParalelo.c
//  tr3conc
//
//  Created by Leticia Silva on 5/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>
//#include <omp.h>

#define TRUE 1
#define FALSE 0

typedef float **MATRIZ;
typedef float cast;
int main (int argc,char**argv){
    FILE *file;
    char processorName[MPI_MAX_PROCESSOR_NAME];
    int ordem, maxIter, filaAval, coef;
    int i, j;
    cast erro;
    MATRIZ matriz;
    cast *t_indep, *x_old, *x_new, *diagonal;
    struct timeval iniTempo;
    struct timeval fimTempo;
    MPI_Status status;
    int nroProc, myRank, nameLen;           
    
    MPI_Init ( &argc, &argv );
    
    MPI_Comm_size ( MPI_COMM_WORLD, &nroProc );
    MPI_Comm_rank ( MPI_COMM_WORLD, &myRank );
    MPI_Get_processor_name( processorName, &nameLen );
    
    /* Inicia o cálculo do tempo */
    gettimeofday( &iniTempo, NULL );
    double tS = iniTempo.tv_sec + ( iniTempo.tv_usec / 1000000.0 );
    if( ( file = fopen( argv[1], "r" ) ) == NULL ) {
        
        printf( "ERRO: na abertura do arquivo\n" );
        MPI_Finalize();
        exit( 1 );
        
    } else {
        if ( myRank == ROOT ) {
            fscanf( file, "%d", &ordem );
            //printf( "Ordem: %d\n", ordem );
            fscanf( file, "%d", &filaAval );
            //printf( "Fila avaliacao: %d\n", filaAval );
            fscanf( file, "%f", &erro );
            //printf( "Ordem: %f\n", erro );
            fscanf( file, "%d", &maxIter );
            //printf( "MaxIteracao: %d\n", maxIter );
            
            /* Alocação de memória para os valores da diagonal principal */
            diagonal = ( cast* ) malloc ( ( ordem ) * sizeof( cast ) );
            
            /* Alocação de memória das linhas da matriz */
            matriz = ( cast** ) malloc ( ( ordem ) * sizeof( cast* ) ); 
            
            /* Alocação de memória das colunas e o preechimento da matriz */
            for( i = 0; i < ordem; i++ ) {
                
                matriz[i] = ( cast* ) malloc( ( ordem + 1 ) * sizeof( cast ) );
                
                for( j = 0; j < ordem; j++ ) { 
                    
                    fscanf( file, "%f ", &matriz[i][j] );
                    
                    if( i == j ) 
                        diagonal[i] = matriz[i][j];
                }
            }
            x_old = ( cast* ) malloc ( ( ordem ) * sizeof( cast ) );
            x_new = ( cast* ) malloc ( ( ordem ) * sizeof( cast ) );
            
            /* Alocação de memória para os termos independentes da equação */
            t_indep = ( cast* ) malloc ( ( ordem ) * sizeof( cast ) );
            
            /* Preechimento do termo independente */
            for( i = 0; i < ordem; i++ ) {
                
                fscanf( file, "%f", &t_indep[i] );
                x_old[i] = 0.0;
                
            }
            
            /* Conferencia da matriz */
            printf("\nCOEFICIENTES DA MATRIZ\n");
            for( i = 0; i < ordem; i++ ) {
                
                for( j = 0; j < ordem; j ++ )
                    
                    printf("%f   ", matriz[i][j] );
                
                printf("\n");
            }
            
            /* Conferencia do vetor */
            printf("\nVALORES DO VETOR\n");
            for( i = 0; i < ordem; i++ )
                printf( "%f ", t_indep[i] );
            
            printf("\n\n");
        }
        else {}
    }
    /* Termina o cálculo do tempo */
    gettimeofday( &fimTempo, NULL );
    double tE = fimTempo.tv_sec + ( fimTempo.tv_usec / 1000000.0 );
    MPI_Finalize();
    printf( "Tempo de execução do processo: %d eh de %.3lf segundos\n\n", myRank, tE - tS );
    
    /* Desalocação de memória */
	for( i = 0; i < ordem; i++ )
		free( matriz[i] );
    
	free( matriz );
	free( diagonal );
	free( x_old );
	free( x_new );
    
	fclose( file );
    return EXIT_SUCCESS;
}