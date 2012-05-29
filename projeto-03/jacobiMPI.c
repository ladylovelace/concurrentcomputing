#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define ROOT 0

typedef struct {

	int qdeLinha;
	int fromLinha;
	int flagAval;

} Bloco;

typedef float cast;

/* Variáveis globais */
cast erro;
int ordem, maxIter, filaAval;

void tratamentoMatrizVetor( FILE *file, cast **A, cast *b, cast *x, int *qdeProc ) {

	int i, j;
	/* Número de elementos por processos */
	int divisaoTam = ordem / *qdeProc;

	/* corrigindo caso tenha sido arredondado para baixo */
	if( divisaoTam * ( *qdeProc ) < ordem )
		divisaoTam++;

	for( i = 0; i < ordem; i++ ) {

		for( j = 0; j < ordem; j++ ) {

			fscanf( file, "%f", &A[i][j] );
			//printf("%f   ", A[i][j]);
		}
		//printf("\n");
	}

	for( j = 0; j < ordem; j++ ) {

		fscanf( file, "%f", &b[j] );
		//printf("%f   ", b[j]);
	}

	/* Divindo coeficientes da matriz pela diagonal principal */
	for( i = 0; i < ordem; i++ ) {

		for( j = 0; j < ordem; j++ ) {

			if( i != j ) 
				A[i][j] /= A[i][i];
		
			//printf("%f   ", A[i][j]);
		}	
		//printf("\n");
	}

	/* Dividindo o termo independente pela correspondente diagonal principal */
	for( i = 0; i < ordem; i++ ) {

		b[i] /= A[i][i];
		//printf("%f   ", b[i]);
	}
	//printf("\n");

	for( i = 0; i < ordem; i++ ) 
		x[i] = 0.0;
	
}

/*
 * Aloca todos os valores da matriz em um vetor e também faz a divisão do vetor de acordo com a
 * quantidade de máquinas alocadas. Lembrando que a divisão do vetor é dada da forma mais uniforme possível
 */
void particionamentoVetor( int *qdeProc, Bloco *bloco ) {

	int i, j;
	int quociente, resto, denominador, qdeLinhaAux, qdeLinhaProx, qdeLinha;

	qdeLinhaAux = 0;
	denominador = *qdeProc;
	qdeLinhaProx = 0;

	for( i = 0; i < *qdeProc; i ++ ) {

		quociente =  ( ordem - qdeLinhaAux ) / ( denominador );
		//printf("Quociente: %d\n", quociente);
		resto = ( ordem - qdeLinhaAux ) % ( denominador );
		//printf("Resto: %d\n", resto);
		qdeLinha = quociente + resto;	
		//printf("qdeLinha: %d\n", qdeLinha);

		bloco[i].qdeLinha = qdeLinha;
		//printf("bloco[%d]: %d\n",i, bloco[i].qdeLinha);
		bloco[i].fromLinha = qdeLinhaAux;
		//printf("bloco[%d]: %d\n",i, bloco[i].fromLinha);
		bloco[i].flagAval = FALSE;
		
		for( j = qdeLinhaProx; j < ( qdeLinhaProx + qdeLinha ); j++ )  {

			if( qdeLinhaProx == filaAval )
				bloco[i].flagAval = TRUE;
		}
		//printf("bloco[%d]: %d\n",i, bloco[i].flagAval);
		qdeLinhaProx = j++;
		denominador--;
		qdeLinhaAux += qdeLinha;		
	}
}

void jacobiRichardson( Bloco *bloco, cast **A, cast *b, cast *x, int *myRank, int *qdeProc ) {

	/*int i, j;

	printf("qdeLinha[%d]: %d\n", *myRank, bloco[*myRank].qdeLinha);
	printf("A partir linha[%d]: %d\n", *myRank, bloco[*myRank].fromLinha);
	printf("flagAval[%d]: %d\n", *myRank, bloco[*myRank].flagAval);

	for( i = bloco[*myRank].fromLinha; i < ( bloco[*myRank].qdeLinha + bloco[*myRank].fromLinha ); i++ )
		for( j = 0; j < ordem; j++ )
			printf("****SLAVE(%d)= %f\n", *myRank, A[i][j]);

	for( i = 0; i < ordem; i++ )
		printf("****SLAVE(%d)= %f\n", *myRank, b[i]);*/

	int k = 0, i, j;	
	cast thisErro = 1.0;
	cast valorAprox = 0.0;
	int flag = FALSE;
	cast x_temp[ordem];
	cast x_new[ordem];

	/* Variável linha para controle da b_equação */
	int linha = bloco[*myRank].fromLinha;

	for( i = 0; i < ordem; i++ ) {

		x_temp[i] = 0.0;
		x_new[i] = 0.0;

	}

	while( k < maxIter && !flag ) {

		for( i = linha; i < ( bloco[*myRank].qdeLinha + bloco[*myRank].fromLinha ); i++ ) {
			
			for( j = 0; j < ordem; j++ ) {

				if( A[i][j] != ordem ) {
					
					/* X = -( L* + R* )x + b */
					printf( "(%f x %f) + ", -1 * A[i][j], x[j] );
					x_new[i] += ( -1 * A[i][j] ) * x[j];
				}
			}

			/* Soma do termo independente da posição correspondente */
			x_new[i] += b[i];
			printf( " %f =  %f\n", b[i], x_new[i] );
			linha++;
		}

		/* Confere o erro */
		if( bloco[*myRank].flagAval == TRUE ) {

			//printf("x_old: %f -- x_new: %f\n", x_old[*filaAval - 1], x_new[*filaAval - 1]);
			thisErro = ( fabs( x_new[filaAval] - x[filaAval] ) ) / fabs( x_new[filaAval] );
			printf( "ERRO: %f\n", thisErro );	

			if( thisErro > erro && k < maxIter - 1 ) {

				for( j = 0; j < ordem; j++ ) {
		
					x[j] = x_new[j];
					x_new[j] = 0;
				}

			} else 
				flag = TRUE;
			
		}

		MPI_Allgather( x_temp, sizeof( x_temp ), MPI_FLOAT, x, sizeof( x_temp ), MPI_FLOAT, MPI_COMM_WORLD );

	} 

	if( bloco[*myRank].flagAval == TRUE ) {

		for( i = 0; i < ordem; i++ )
			valorAprox += A[filaAval][i] * x_new[i];
	
		valorAprox = valorAprox * A[filaAval][filaAval];
	
		printf( "\n--------------------------------------------\n" );
		printf( "Iterations: %d\n", k );
		printf( "RowTest: %d => [%f] =? %f\n", filaAval, valorAprox, ( b[filaAval] * A[filaAval][filaAval] ) );
		printf( "--------------------------------------------\n\n" ); 

		// Condição que determina que todos os slaves terminaram a tarefa 			
		MPI_Reduce( &myRank, &finalizar, 1, MPI_INT, MPI_SUM, *myRank, MPI_COMM_WORLD );	
	}

}

int main ( int argc, char *argv[] ) {

	FILE *file;
	cast **A;
	cast *b, *x;
	int finalizar = 0, i;
	int qdeProc, myRank, nameLen;		
	char processorName[20];
	struct timeval iniTempo;
	struct timeval fimTempo;
	MPI_Status status;

	MPI_Init ( &argc, &argv );

	MPI_Comm_size ( MPI_COMM_WORLD, &qdeProc );
	MPI_Comm_rank ( MPI_COMM_WORLD, &myRank );
	MPI_Get_processor_name( processorName, &nameLen );

	/* Inicia o cálculo do tempo */
	gettimeofday( &iniTempo, NULL );
	double tS = iniTempo.tv_sec + ( iniTempo.tv_usec / 1000000.0 );

	Bloco *bloco = ( Bloco* ) malloc( qdeProc * sizeof( Bloco ) );

	if( ( file = fopen ( argv[1], "r" ) ) != NULL ) {
		
		fscanf( file, "%d", &ordem );
		//printf( "Ordem: %d\n", ord );
		fscanf( file, "%d", &filaAval );
		//printf( "Fila avaliacao: %d\n", filaAval );
		fscanf( file, "%f", &erro );
		//printf( "Erro: %f\n", erro );
		fscanf( file, "%d", &maxIter );
		//printf( "MaxIteracao: %d\n", maxIter );
			
		// Alocação de memória da coluna da matriz 
		A = ( cast** ) malloc ( ( ordem ) * sizeof( cast* ) ); 

		// Alocação de memória das linhas da matriz 
		for( i = 0; i < ordem; i++ ) 
			A[i] = ( cast* ) malloc( ( ordem ) * sizeof( cast ) );

		// Alocação de memória do termo independente da matriz 
		b = ( cast* ) malloc ( ( ordem ) * sizeof( cast ) ); 

		// Alocação de memória do termo independente da matriz 
		x = ( cast* ) malloc ( ( ordem ) * sizeof( cast ) );

		tratamentoMatrizVetor( file, A, b, x, &qdeProc );

		particionamentoVetor( &qdeProc, bloco );

		fclose( file );	

	} else {

		printf("ERRO: na abertura do arquivo\n");
		MPI_Finalize();
		exit( 1 );
	}

	
	jacobiRichardson( bloco, A, b, x, &myRank, &qdeProc );
		
	printf("\n");

	/* Termina o cálculo do tempo */
	gettimeofday( &fimTempo, NULL );
	double tE = fimTempo.tv_sec + ( fimTempo.tv_usec / 1000000.0 );

	//printf( "Tempo de execução do processo: %s eh de %.3lf segundos\n\n", processorName, tE - tS );

	MPI_Finalize();
	return EXIT_SUCCESS;
}
