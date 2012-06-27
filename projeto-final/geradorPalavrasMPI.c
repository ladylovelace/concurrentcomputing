#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>

#define ROOT 0
#define TRUE 1
#define FALSE 0
#define MAX_LETRAS_PALAVRA 50 
#define MAX_LETRAS_SENTENCA 100000

typedef struct {

   	int ini;
    int fim;
	int qdePalavra;
	int totalPalavra;

} Bloco;

typedef struct {

	char palavra1[30][2];
	char palavra2[300][3];
	char palavra3[1000][4];
	char palavra4[4000][5];
	char palavra5[9000][6];

} Vetor;

char aux_palavra4[2500][6];
char palavra4[3800][6];
int quantidade4 = 0, global = 0;

MPI_Status status;

/*
 * Função que particiona o arquivo de entrada. Esta "divide" em blocos o arquivo de acordo com o número de escravo, 
 * ou seja, cada escravo recebe um parâmetro de início e fim do bloco. Não esquecendo que a função realiza um tratamento
 * para que todos os bloco tenha somente sentenças completas.
 */
void particionamentoArquivo( FILE *file, int *qdeProc, Bloco *processo ){

	int i, continua;
	int tamAuxIni, tamAuxFim;
	int tamanho, bloco;
	char c;

	/* Posiciona o ponteiro para o fim do arquivo */
	fseek( file, 0, SEEK_END );
	tamanho = ftell( file );
	
	rewind( file );

	bloco = ( int ) tamanho / ( *qdeProc - 1 );

	tamAuxIni = 0;
	tamAuxFim = 0;

	for ( i = 0; i < *qdeProc - 1; i++ ){

		/* Particionamento do último bloco */
		if( i == *qdeProc - 2 ) {

			tamAuxFim = ftell( file );    

			processo[i].ini = tamAuxFim;
			processo[i].fim = tamanho;
			processo[i].qdePalavra = 0;
			processo[i].totalPalavra = 0;

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
					processo[i].totalPalavra = 0;

					continua = 0;
				}
			}

			tamAuxIni = tamAuxFim;
		}

		//printf("Bloco #%d começa em %d termina em: %d (%d)\n", i+1, processo[i].ini, processo[i].fim, 
					//(processo[i].fim - processo[i].ini));
	}
}

/* A função abaixo confere se a variável colecao é palindromo.
 * A análise é dada da seguinte forma: confere a primeira letra com a última a segunda com a penúltima e assim por diante. 
 * A conferencia é dada na colecao inteira menos no meio dela, ou seja, esta não casa com nenhuma das outras.
*/
void geraPalavra( int *myRank, char *sentenca, Bloco *processo, int *TAMANHO, int *quant_palavras ) {

	char letras[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	char palavra[MAX_LETRAS_PALAVRA], palavra_aux[MAX_LETRAS_PALAVRA];
	int i, indice, flag;
	int tamSent = 0, tam = 0, continua = FALSE;

	while( !continua ) {

		while( sentenca[tamSent] != ' ' && sentenca[tamSent] != '\0' ) {	

			if( isupper( sentenca[tamSent] ) )
				palavra[tam] = tolower( sentenca[tamSent++] );
			else
				palavra[tam] = sentenca[tamSent++];

			if( sentenca[tamSent + 1] == '\0' )
				continua = TRUE;

			palavra[++tam] = '\0';
		}

		processo[*myRank - 1].totalPalavra += 1;
		if( tam == *TAMANHO ) {

			srand( time( NULL ) );
			flag = FALSE;
			
			while( !flag ) {

				for( i = 0; i < tam; i++ ) {

					/* Gera número entre 0 a 26 */
					indice = rand()%26;

					palavra_aux[i] = letras[indice];
					palavra_aux[i+1] ='\0';
				}

				if( !strcmp( palavra, palavra_aux ) ) {
					
					*quant_palavras += 1;
					processo[*myRank - 1].qdePalavra += 1;
					/* Para cada tamanho de palavras armazena na lista correspondente */			
					strcpy( aux_palavra4[global++], palavra );
					flag = TRUE;
				}			
			} 
		}

		tamSent++;
		tam = 0;
	}
}

void geraPalavraMaiores( char *sentenca ) {

	char palavra[MAX_LETRAS_PALAVRA], palavra_aux[MAX_LETRAS_PALAVRA];
	int i, j, flag1 = FALSE, flag2 = FALSE;
	int tamSent = 0, tam = 0, continua = FALSE, posicao;

	while( !continua ) {

		while( sentenca[tamSent] != ' ' && sentenca[tamSent] != '\0' ) {	

			if( isupper( sentenca[tamSent] ) )
				palavra[tam] = tolower( sentenca[tamSent++] );
			else
				palavra[tam] = sentenca[tamSent++];

			if( sentenca[tamSent + 1] == '\0' )
				continua = TRUE;

			palavra[++tam] = '\0';
		}
		
		if( tam == 8 ) {
			//printf("-- %s\n", palavra);
			for( i = 0; i < quantidade4 && !flag1; i++ ) {

				/* Procura por uma substring dentro de uma string a partir da posicao zero */
				if( existeSubstring( palavra, palavra4[i], 0 ) ) {
					//printf( "PRIMEIRA: %s\n", palavra4[i] );
					strcpy( palavra_aux, palavra4[i] );
					flag1 = TRUE;

					posicao = ( ( strlen( palavra ) ) - ( strlen( palavra ) - strlen( palavra4[i] ) ) );
					//printf("RESTANTE: %d\n", posicao);
					for( j = 0; j < quantidade4 && !flag2; j++ ) {

						/* Procura por uma substring dentro de uma string */
						if( existeSubstring( palavra, palavra4[j], posicao ) ) {
							//printf( "SEGUNDA: %s\n", palavra4[j] );
							strcat( palavra_aux, palavra4[j] );
							printf( "FINAL: %s\n", palavra_aux );
							flag2 = TRUE;				
						}
					}
				}
			}	
		}
		
		tamSent++;
		tam = 0;
	}	
}

int existeSubstring( char *string, char *sub_string, int ini ) {

	int i = ini, j = 0;

	while( j < strlen( sub_string ) ) {

		if( ( string[i] != sub_string[j] ) )
			return FALSE;
		else {
			j++;
			i++;
		}
	}

	return TRUE;
}

int main( int argc, char *argv[] ) {

	char sentenca [MAX_LETRAS_SENTENCA];
	char processorName[MPI_MAX_PROCESSOR_NAME];
	int slave, i, j;
	int finalizar = 0, quant_palavras = 0;
	FILE *file;
	struct timeval iniTempo;
	struct timeval fimTempo;
	int qdeProc, myRank, nameLen;	
	int TAMANHO = atoi( argv[1] );	

	MPI_Init ( &argc, &argv );

	MPI_Comm_size ( MPI_COMM_WORLD, &qdeProc );
	MPI_Comm_rank ( MPI_COMM_WORLD, &myRank );
	MPI_Get_processor_name( processorName, &nameLen );

	/* Inicia o cálculo do tempo */
	gettimeofday( &iniTempo, NULL );
	double tS = iniTempo.tv_sec + ( iniTempo.tv_usec / 1000000.0 );

    Bloco *processo = ( Bloco* ) calloc( qdeProc-1, sizeof( Bloco ) );

	if( ( file = fopen ( "entrada.txt", "r" ) ) != NULL ) {

		if ( myRank == ROOT ) {

			particionamentoArquivo( file, &qdeProc, processo );
			
			rewind( file );
			
			i = 0;

			for ( slave = 1; slave < qdeProc; slave++ ){

				MPI_Send( &processo[i], sizeof( Bloco ), MPI_BYTE, slave, 0, MPI_COMM_WORLD ); 
				i++;
			}
		
			j = 0;
			/* Copia todas as palavras de mesmo tamanho em um vetor */
			for( slave = 1; slave < qdeProc; slave++ ) {

				/* Recebe a quantidade de palavras criadas por um determinado Slave */
				MPI_Recv( &quant_palavras, 1, MPI_INT, slave, 0, MPI_COMM_WORLD, &status );
				
				/* Recebe o vetor de palavras criadas por um determinado Slave */
				MPI_Recv( &aux_palavra4, quant_palavras * 6, MPI_CHAR, slave, 0, MPI_COMM_WORLD, &status );

				for(i = 0; i < quant_palavras; i++) 		
					strcpy( palavra4[j++], aux_palavra4[i] );
			}
		
			quantidade4 = j; 	
			/* Atualiza a variável (quantidade) de todos os processos */
			MPI_Bcast( &quantidade4, 1, MPI_INT, ROOT, MPI_COMM_WORLD );

			/* Atualiza os vetores das palavras de todos os processos */
			MPI_Bcast( palavra4, ( quantidade4 * 6 ), MPI_CHAR, ROOT, MPI_COMM_WORLD );	
	
			/* Condição que determina que todos os slaves terminaram a tarefa */			
			MPI_Reduce( &myRank, &finalizar, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD );	
					
		} else {
			
			MPI_Recv( &processo[myRank - 1], sizeof( Bloco ), MPI_BYTE, ROOT, 0, MPI_COMM_WORLD, &status ); 
			
			/* Posiciona o ponteiro no início do bloco correspondente para cada slave */			
			fseek( file, processo[myRank - 1].ini, SEEK_SET );
		
			/* Gera todas as palavras randomicamente menores igual a 5 */
			while(  ftell( file ) < processo[myRank - 1].fim ) {

				fscanf( file, "%[^\n]\n" , sentenca );
				strcat( sentenca, "\0" );
				geraPalavra( &myRank, sentenca, processo, &TAMANHO, &quant_palavras );
			}
			
			/* Envia a quantidade de palavras criadas para ROOT */
			MPI_Send( &quant_palavras, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD );
			
			/* Envia o vetor de palavras criadas para ROOT */
			MPI_Send( aux_palavra4, ( quant_palavras ) * 6, MPI_CHAR, ROOT, 0, MPI_COMM_WORLD );

			MPI_Bcast( &quantidade4, 1, MPI_INT, ROOT, MPI_COMM_WORLD );
			MPI_Bcast( palavra4, ( quantidade4 * 6 ), MPI_CHAR, ROOT, MPI_COMM_WORLD );

			/*if( myRank == 1 )	
				for( i = 0; i < quantidade4; i++ )
					printf("%d -- %s\n", i, palavra4[i] );	*/
			
			/* Posiciona o ponteiro no início do bloco correspondente para cada slave */			
			fseek( file, processo[myRank - 1].ini, SEEK_SET );
		
			/* Gera palavras maiores que 5 associando com as palvras já geradas */
			while(  ftell( file ) < processo[myRank - 1].fim ) {

				fscanf( file, "%[^\n]\n" , sentenca );
				strcat( sentenca, "\0" );
				geraPalavraMaiores( sentenca );
			}

			//printf( "Slave:%d gerou %d palavras de tamanho %d no Total de palavras %d\n",
					//myRank, processo[myRank - 1].qdePalavra, TAMANHO, processo[myRank - 1].totalPalavra );
			
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
