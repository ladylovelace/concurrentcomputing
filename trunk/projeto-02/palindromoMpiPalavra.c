#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>
#include <omp.h>

#define TRUE 1
#define FALSE 0
#define MAX_LETRAS_PALAVRA 50
#define MAX_LETRAS_SENTENCA 10000

typedef struct {

    	int ini;
    	int fim;
	int qdePalavra;
	int palavraPrimo;

} Bloco;

void crivoEratostenes( int *nroPrimo, int *qdePrimo ) {

	int raiz;
	int cond = TRUE;
	int i, j;
	int vetor[*nroPrimo];

	raiz = sqrt( *nroPrimo );

	if( vetor == NULL ) {

		printf( "ERRO: Falta de memória\n" );
		exit( EXIT_FAILURE );
	}

	/* Preenchimento do vetor de acordo com o número primo desejado */
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
 * Retira todas as pontuações que podem eventualmente atrapalhar na análise do palindromo (sentenca)
 */
void checkPunctuationMark( char *sentenca ) {


	char punctmarks[] = ". , ; : / ? ! # $ % ^ & * ' ( ) [ ] { } | + = - _ < >";
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
 * A análise é dada da seguinte forma: confere a primeira letra com a última a segunda com a penúltima e assim por diante. 
 * A conferencia é dada na colecao inteira menos no meio dela, ou seja, esta não casa com nenhuma das outras.
*/
void palindromoPalavra( char *colecao, int *qdePalindromo, int *qdePrimo ) {

	//printf("%s\n", colecao);
	//printf("qdePalindromo: %d -- qdePrimo: %d\n", qdePalindromo, qdePrimo);
	int n, k , nroPrimo;
	int cond, i = 0 , j = 0, l;
	char palavra[MAX_LETRAS_PALAVRA];

	while( i < ( strlen( colecao ) + 1 ) ) {

		/* carrega as palavras da sentença na string word */
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
 * Função que particiona o arquivo de entrada. Esta "divide" em blocos o arquivo de acordo com o número de escravo, 
 * ou seja, cada escravo recebe um parâmetro de início e fim do bloco. Não esquecendo que a função realiza um tratamento
 * para que todos os bloco tenha somente sentenças completas.
 */
void particionamentoArquivo( FILE *file, int *nroProc, Bloco *processo ){

	int i, continua;
	int tamAuxIni, tamAuxFim;
	int tamanho, bloco;
	char c;

	/* Posiciona o ponteiro para o fim do arquivo */
	fseek( file, 0, SEEK_END );
	tamanho = ftell( file );

	//printf("\nO arquivo possui %d bytes.\n", tamanho);
	
	rewind( file );

	bloco = ( int ) tamanho / ( *nroProc - 1 );
	//printf("Tamanho do bloco: %d\n\n", bloco);

	tamAuxIni = 0;
	tamAuxFim = 0;

	for ( i = 0; i < *nroProc - 1; i++){

		/* Particionamento do último bloco */
		if( i == *nroProc - 2 ) {

			tamAuxFim = ftell( file );    

			processo[i].ini = tamAuxFim;
			processo[i].fim = tamanho;
			processo[i].qdePalavra = 0;
			processo[i].palavraPrimo = 0;

		} else {

			continua = 1;
			fseek( file, tamAuxIni + bloco, SEEK_SET );
		
			while( continua && !feof( file ) ){

				fscanf( file, "%c", &c );

				if ( c == '\n'){

					/* Pega a posição do ponteiro */
					tamAuxFim = ftell( file );

					processo[i].ini = tamAuxIni;
					processo[i].fim = tamAuxFim;
					processo[i].qdePalavra = 0;
					processo[i].palavraPrimo = 0;

					continua = 0;
				}
			}

			tamAuxIni = tamAuxFim;
		}

		//printf("Bloco #%d começa em %d termina em: %d (%d)\n", i+1, processo[i].ini, processo[i].fim, 
					//(processo[i].fim - processo[i].ini));
	}
}

int main ( int argc, char *argv[] ) {

	char sentenca [MAX_LETRAS_SENTENCA];
	char processorName[MPI_MAX_PROCESSOR_NAME];
	int slave, i, finalizar = 0;
	int ROOT = 0;  
	FILE *file;
	struct timeval iniTempo;
	struct timeval fimTempo;
	MPI_Status status;
	int nroProc, myRank, nameLen;		
	
	MPI_Init ( &argc, &argv );

	MPI_Comm_size ( MPI_COMM_WORLD, &nroProc );
	MPI_Comm_rank ( MPI_COMM_WORLD, &myRank );
	MPI_Get_processor_name( processorName, &nameLen );

	//printf( "Hello from processor: %s, myRank: %d out of %d processors.\n", processorName, myRank, nroProc );

	/* Inicia o cálculo do tempo */
	gettimeofday( &iniTempo, NULL );
	double tS = iniTempo.tv_sec + ( iniTempo.tv_usec / 1000000.0 );

    	Bloco *processo = ( Bloco* ) calloc( nroProc, sizeof( Bloco ) );

	if( ( file = fopen ( "mobythes.txt", "r" ) ) != NULL ) {

		if ( myRank == ROOT ) {

			particionamentoArquivo( file, &nroProc, processo );
			
			rewind( file );
			
			i = 0;

			for ( slave = 1; slave < nroProc; slave++ ){

				MPI_Send ( &processo[i], sizeof( Bloco ), MPI_BYTE, slave, 0, MPI_COMM_WORLD ); 
				
				//printf("SEND to Slave: %d -- %d -- %d -- Bloco: %d\n", slave, processo[i].ini, 
							//processo[i].fim, sizeof( Bloco ) );
				i++;
			}
			
			/* Condição que determina que todos os slaves terminaram a tarefa */			
			MPI_Reduce( &myRank, &finalizar, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD );						

		} else {
			
			MPI_Recv ( &processo[myRank - 1], sizeof( Bloco ), MPI_BYTE, ROOT, 0, MPI_COMM_WORLD, &status ); 
			//printf("=== SLAVE %d ===\n", myRank);
			//printf( "Ini: %d -- Fim: %d -- bloco: %d\n\n", processo[myRank - 1].ini, 
							//processo[myRank - 1].fim, sizeof( Bloco) );
			//printf("Palavra: %d -- Primo: %d\n",processo[myRank - 1].qdePalavra, processo[myRank - 1].palavraPrimo);

			/* Posiciona o ponteiro no início do bloco correspondente para cada slave */			
			fseek( file, processo[myRank - 1].ini, SEEK_SET);
		
			#pragma omp parallel num_threads(2)  
			while(  ftell( file ) < processo[myRank - 1].fim ) {

				fscanf( file, "%[^\n]\n" ,sentenca );
				//printf("%s\n", sentenca);
				palindromoPalavra( sentenca, &processo[myRank - 1].qdePalavra, &processo[myRank - 1].palavraPrimo );
				
			}

			printf( "Slave:%d encontrou %d palavras palindromos e dentre elas %d sao primos\n", myRank, 
				processo[myRank - 1].qdePalavra, processo[myRank - 1].palavraPrimo );

			MPI_Reduce( &myRank, &finalizar, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD );
		}

		fclose ( file );		

	} else {

		printf("ERRO: na abertura do arquivo\n");
		MPI_Finalize();
		exit( 1 );
	}

	/* Termina o cálculo do tempo */
	gettimeofday( &fimTempo, NULL );
	double tE = fimTempo.tv_sec + ( fimTempo.tv_usec / 1000000.0 );

	printf( "Tempo de execução do processo: %d eh de %.3lf segundos\n\n", myRank, tE - tS );

	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
