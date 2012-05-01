#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <ctype.h>
#include <omp.h>

#define TRUE 1
#define FALSE 0
#define MAX_LETRAS_PALAVRA 100
#define MAX_LETRAS_SENTENCA 10000

void crivoEratostenes( int *nroPrimo, int* qdePrimo ) {

	int raiz;
	int cond = TRUE;
	int i, j; 
	int vetor[*nroPrimo];
	long startTime;

	raiz = sqrt( *nroPrimo );

	/* Preenchimento do vetor os valores */
	#pragma omp parallel for private( i )
	for( i = 2; i <= *nroPrimo; i++ ) {

		vetor[i] = i;
	}

	#pragma omp parallel for private( i, j ) schedule( dynamic )
	for( i = 2; i <= raiz; i++ ) {
	
		/* Invalida números não primos */
		for( j = i+i; j <= *nroPrimo; j += i )
			vetor[j] = 0; 

	}

	/* Checagem se o valor recebido é primo */
	for( i = 2; i <= raiz && cond; i++ ) {

		if ( vetor[i] ) {

			/* Se o resto da divisão for diferente de zero não faça nada */
			if( ( *nroPrimo % vetor[i] ) != 0 ); 
				
			else 
				cond = FALSE;	
		}
	}
	
	if ( cond )
		*qdePrimo += 1;	
}

/*
 * Retira todas as pontuações que podem eventualmente atrapalhar na análise do palindromo (sentemca)
 */
void checkPunctuationMark( char *sentenca ) {

	char punctmarks[] = ". , ; : / ? ! # $ % ^ * ' ( ) [ ] { } | ~ + - _ < > @";
	char sentenca_aux[ strlen( sentenca ) ];
	int i, j, k = 0;
	int flag = FALSE;

	/* Como as aspas duplas não podem ser adicionadas acima, precisa-se da linha de código abaixo. */
	punctmarks[ strlen( punctmarks ) ] = '"';
	
	for( i = 0; i < strlen( sentenca ); i++ ) {

		/* Para cada caracter da palavra verifica-se esse não é uma 'punctmark' */
		for( j = 0; j < strlen( punctmarks ) && !flag; j++ ) {

			if( sentenca[i] == punctmarks[j] )
				flag = TRUE;
		}

		if( !flag )
			sentenca_aux[k++] = sentenca[i];

		else
			flag = FALSE;
	}
	
	sentenca_aux[k] = '\0';

	strcpy( sentenca, sentenca_aux );
}

/* A função abaixo confere se a variável colecao é palindromo.
 * A análise é dada da seguinte forma: confere a primeira letra com a última
 * a segunda com a penúltima e assim por diante. A conferencia é dada na colecao
 * inteira menos no meio dela, ou seja, esta não casa com nenhuma das outras.
*/
void palindromoPalavra( char *colecao, int *qdePalindromo, int* qdePrimo ) {

	int n, k , nroPrimo;
	int cond, i = 0 , j = 0, l;
	char palavra[MAX_LETRAS_PALAVRA];

	while( i < ( strlen( colecao ) + 1 ) ) {
		
		/* carrega as palavras da sentença coleção */
		if( colecao[i] != ',' && colecao[i] != '\0') {
			
			if( isupper( colecao[i] ) )

				colecao[i] = tolower( colecao[i] );

			palavra[j++] = colecao[i];
			palavra[j] = '\0';

		} else {
			
			j = 0;

			checkPunctuationMark( palavra );

			/* A palavra palindromo tem no mínimo três letras */
			if( strlen( palavra ) > 2 ) {

				cond = TRUE;
				l = 0;
				n = strlen( palavra );
				k = n/2;

				/* A variavel nroPrimo recebe o valor correspondente ao mapeamento do código ASCII 
				 * da letra do meio da palavra. 
				 */
				nroPrimo = ( int ) palavra[k];

				/* Mapeamento do código ASCII para cada caracter da sentença */
				while( cond && l < k ) {
					
					if( palavra[l] == palavra[n - l - 1] ) 
				
						nroPrimo += ( 2 * palavra[l++] );
						
					else
						cond = FALSE;	
				}
				
				if( cond ) { 

					*qdePalindromo += 1;
					crivoEratostenes( &nroPrimo, qdePrimo );

				} 
			}
		}

		i++;
	}
}

/* 
 * Essa função verifica a ocorrência de palindromo de sentença e palavras, respectivamente 
 */
void palindromoSentenca( char *colecao, int *palindromoSen, int* palindromoPal, int* primoSen, int* primoPal ) {

	int n, k, aux, nroPrimo;
	int cond = TRUE, i = 0 , j = 0, l;
	char palavra[MAX_LETRAS_PALAVRA];
	char sentenca[MAX_LETRAS_SENTENCA];

	/* ======================== SENTENÇAS ======================== */
	strcpy( sentenca, colecao );

	/* Troca todas as letras maiúscula por minúscila, se necessário */
	#pragma omp sections nowait
	{
	#pragma omp section
	while( sentenca[i] != '\0' ) {

		if( sentenca[i] != ' ' ) { 
			
			if( isupper( sentenca[i] ) )

				sentenca[i] = tolower( sentenca[i] );
		}

		i++;
	}
	}

	checkPunctuationMark( sentenca );

	n = strlen( sentenca );
	k = n/2;
		
	nroPrimo = ( int ) sentenca[k];

	/* Mapeamento do código ASCII para cada caracter da sentença */
	while( cond && l < k ) {

		if( sentenca[l] == sentenca[n - l - 1] )

			nroPrimo += ( 2 * sentenca[l++] );
			
		else 
			cond = FALSE;
	}
	
	if( cond ) { 
		
		*palindromoSen += 1;
		crivoEratostenes( &nroPrimo, primoSen );

	}

	i = 0;
	cond = TRUE;

	/* ======================== PALAVRAS ======================== */
	while( i < ( strlen( colecao ) + 1 ) ) {
		
		/* carrega as palavras da sentença na string word */
		if( colecao[i] != ' ' && colecao[i] != '\0' ) {

			if( isupper( colecao[i] ) ) 

				colecao[i] = tolower( colecao[i] );

			palavra[j++] = colecao[i];
			palavra[j] = '\0';

		} else {
			
			j = 0;
		
			checkPunctuationMark( palavra );

			/* A palavra palindromo tem no mínimo três letras */
			if( strlen( palavra ) > 2 ) {
				
				cond = TRUE;
				l = 0;
				n = strlen( palavra );
				k = n/2;

				nroPrimo = ( int ) palavra[k];

				/* Mapeamento do código ASCII de cada caracter da palavra */
				while( cond && l < k ) {

					if( palavra[l] == palavra[n - l - 1] ) 

						nroPrimo += ( 2 * palavra[l++] );

					else
						cond = FALSE;	
				}
				
				if( cond ) {

					*palindromoPal += 1;
					crivoEratostenes( &nroPrimo, primoPal );

				}
			} 
		}

		i++;
	}
}

int main( int argc, char* argv[] ) {

	int qdePalindromo = 0, qdePrimo = 0;
	int palindromoPal = 0, palindromoSen = 0, primoPal = 0, primoSen = 0;
	char sentenca[MAX_LETRAS_SENTENCA];
	long startTime;
	FILE *file;
	struct timeval iniTempo;
	struct timeval fimTempo;

	printf("%s -- %s\n", argv[1], argv[2]);
	

	if( !( file = fopen( argv[1], "r" ) ) ) {

		printf( "ERRO: ao abrir o arquivo\n" );
		exit( EXIT_FAILURE );

	}
		
	if(  strcmp( "palavra", argv[2] ) == 0 ) {
			
		gettimeofday( &iniTempo, NULL );
		double tS = iniTempo.tv_sec + ( iniTempo.tv_usec / 1000000.0 );

		#pragma omp parallel num_threads(4)  
		while( !feof( file ) ) {

			fscanf( file, "%[^\n]\n", sentenca );
			strcat( sentenca, "\0" );

			palindromoPalavra( sentenca, &qdePalindromo, &qdePrimo );
		}
		
		gettimeofday( &fimTempo, NULL );
		double tE = fimTempo.tv_sec + ( fimTempo.tv_usec / 1000000.0 );

		printf( "Ha %d palavras palindromos e dentre elas %d sao primos\n", qdePalindromo, qdePrimo );	
		printf( "Tempo de execução: %.3lf\n", tE - tS );

	} else {

		gettimeofday( &iniTempo, NULL );
		double tS = iniTempo.tv_sec + ( iniTempo.tv_usec / 1000000.0 );

		#pragma omp parallel num_threads(4)  
		while( !feof( file ) ) {

			fscanf( file, "%[^\n]\n", sentenca );
			strcat( sentenca, "\0" );

			palindromoSentenca( sentenca, &palindromoSen, &palindromoPal, &primoSen, &primoPal );
		}

		gettimeofday( &fimTempo, NULL );
		double tE = fimTempo.tv_sec + ( fimTempo.tv_usec / 1000000.0 );

		printf( "Ha %d sentenca(s) palindromo(s) e dentre ela(s) %d são primo(s)\n", palindromoSen, primoSen );
		printf( "Ha %d palavra(s) palindromo(s) e dentre ela(s) %d são primo(s)\n", palindromoPal, primoPal );
		printf( "Tempo de execução: %.3lf segundos\n", tE - tS );

	}

	fclose( file );

	exit( EXIT_SUCCESS );
}
