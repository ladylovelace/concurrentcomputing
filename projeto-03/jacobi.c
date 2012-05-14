#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

typedef float **MATRIZ;
typedef float cast;

void dividirPelaDiagonal( cast **matriz, int *ordem, cast *t_indep, cast *diagonal ) {

	int i, j;
	cast valor;

	/* Dividir cada equação pelo correspondente elemento da diagonal principal */
	printf("COEFICIENTES DA MATRIZ (FUNCAO)\n");
	for( i = 0; i < *ordem; i++ ) {

		t_indep[i] = t_indep[i] / diagonal[i];

		for( j = 0; j < *ordem; j ++ ) { 

			matriz[i][j] = matriz[i][j] / diagonal[i];
			printf("%f   ", matriz[i][j] );

		}
		printf("\n");
	}

	/*Teste do critério de convergência: para este caso foi utilizado o critério das linhas */
	for( i = 0; i < *ordem; i++ ) {
		
		valor = 0.0;

		for( j = 0; j < *ordem; j++ ) {
		
			if( i != j && valor < 1 ) {
	
				valor += matriz[i][j];

			} else if( valor > 1 ) {
				
				printf( "O processo de Jacobi-Richardson aplicado ao sistema dado NAO covergerah!\n" );
				exit( 1 );
			}
		}
		//printf("Somatorio da linha: %d = %f\n", i, valor);
	}
}

void iteracao( cast **matriz, cast *t_indep, cast *x_old, cast *x_new, cast *erro, int *filaAval, int *maxIter, int *ordem ) {

	int k = 0, j, i;	
	cast thisErro = 1.0;
	int flag = FALSE;

	while( k < *maxIter && !flag ) {

		for( i = 0; i < *ordem ; i++ ) {

			for( j = 0; j < *ordem; j++ ) {

				if( i != j ) {
					
					/* X = -( L* + R* )x + b* */
					printf( "(%f x %f) + ", -1 * matriz[i][j], x_old[j] );
					x_new[i] += ( -1 * matriz[i][j] ) * x_old[j];
				}
			}

			/* Soma do termo independente */
			x_new[i] += t_indep[i];
			printf( " %f =  %f\n", t_indep[i], x_new[i] );
		}

		//printf("Vetor: %f -- VetorAux: %f\n", vetor[*filaAval - 1], vetorAux[*filaAval - 1]);
		thisErro = ( fabs( x_new[*filaAval - 1] - x_old[*filaAval - 1] ) ) / fabs( x_new[*filaAval - 1] );
		printf( "ERRO: %f\n", thisErro );
		
		if( thisErro > *erro ) {

			for( j = 0; j < *ordem; j ++ ) {
		
				x_old[j] = x_new[j];
				x_new[j] = 0;
				//printf("Vetor[%d]: %f\n", j, vetor[j]);
			}

		} else 
			flag = TRUE;

		k++;
	}

	printf( "\n--------------------------------------------\n" );
	printf( "Iterations: %d\n", k );
	printf( "RowTest: %d => [%f] =? %f\n", *filaAval, x_old[*filaAval - 1], x_new[*filaAval - 1]);
	if( flag )
		printf( "Condição de parada por ERRO\n" );
	else
		printf( "Condicao de parada por ITERACAO\n");
	printf( "--------------------------------------------\n\n" );
}

int main( int argc, char* argv[] ) {

	FILE *file;
	int ordem, maxIter, filaAval, coef;
	int i, j;
	cast erro;
	MATRIZ matriz;
	cast *t_indep, *x_old, *x_new, *diagonal;

	if( ( file = fopen( argv[1], "r" ) ) == NULL ) {

		printf( "ERRO: na abertura do arquivo\n" );
		exit( 1 );

	} else {

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

		dividirPelaDiagonal( matriz, &ordem, t_indep, diagonal );

		iteracao( matriz, t_indep, x_old, x_new, &erro, &filaAval, &maxIter , &ordem );

	}

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
