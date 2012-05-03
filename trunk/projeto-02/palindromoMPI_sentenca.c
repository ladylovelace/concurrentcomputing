#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define MAX_LETRAS_PALAVRA 50
#define MAX_LETRAS_SENTENCA 10000

#define FROM_MASTER 1          /* setting a message type (tag) apagar isso depois*/
#define FROM_WORKER 2          /* setting a message type (tag), parecido com o slave = 1 e ROOT = 0 na main*/

//int qdePalindromo = 0;
//int qdePrimo = 0;

typedef struct {

    int ini;
    int fim;
	int qdeSentenca;
	int qdePalavra;
	int sentencaPrimo;
	int palavraPrimo;

} Posicao;

void crivoEratostenes( int nroPrimo, int* qdePrimo ) {

	int raiz;
	int cond = TRUE;
	int i, j;
	int vetor[nroPrimo];
	long startTime;

	raiz = sqrt( nroPrimo );

	if( vetor == NULL ) {

		printf( "ERRO: Falta de memória\n" );
		exit( EXIT_FAILURE );
	}

	/* Preenchimento do vetor de acordo com o número primo desejado */
	for( i = 2; i <= nroPrimo; i++ ) {

		vetor[i] = i;
	}

	for( i = 2; i <= raiz; i++ ) {

		/* Invalida números não primos */
		for( j = i+i; j <= nroPrimo; j += i )
			vetor[j] = 0;

	}

	/* Checagem se o valor recebido é primo */
	for( i = 2; i <= raiz && cond; i++ ) {

		if ( vetor[i] ) {

			/* Se o resto da divisão for diferente de zero não faça nada */
			if( ( nroPrimo % vetor[i] ) != 0 );

			else
				cond = FALSE;
		}
	}

	if ( cond )
		qdePrimo += 1;

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
 * A análise é dada da seguinte forma: confere a primeira letra com a última
 * a segunda com a penúltima e assim por diante. A conferencia é dada na colecao
 * inteira menos no meio dela, ou seja, esta não casa com nenhuma das outras.
*/
void palindromoPalavra( char *colecao, int qdePalindromo, int qdePrimo ) {

	int n, k , nroPrimo;
	int cond, i = 0 , j = 0, l;
	char palavra[MAX_LETRAS_PALAVRA];

	while( i < ( strlen( colecao ) + 1 ) ) {

		/* carrega as palavras da sentença na string word */
		if( colecao[i] != ' ' && colecao[i] != '\0') {

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

					qdePalindromo += 1;
					crivoEratostenes( nroPrimo, &qdePrimo );

				}
			}
		}

		i++;
	}
	//printf("\n>> Saindo da função palíndromo\n");
}


void particionamentoArquivo( FILE *file, int *num_slaves, Posicao *processo ){

	int i, continua;
	int tamAuxIni, tamAuxFim;
	int tamanho, bloco;
	char palavra[30], sentenca[1000];
	char c;

	/* Posiciona o ponteiro para o fim do arquivo */
    fseek( file, 0, SEEK_END );
    tamanho = ftell( file );

    //printf("O arquivo possui %d bytes.\n", tamanho);
	//printf("%d processos escravos.\n", *num_slaves);
	rewind( file );

        bloco = ( int ) tamanho / *num_slaves;
        printf("1-Tamanho do bloco: %d\n", bloco);

	tamAuxIni = 0;
	tamAuxFim = 0;
    printf("\n");
	for ( i = 0; i< *num_slaves; i++){

        /* Cuida do último processo */
        if( i == *num_slaves -1 ) {
            tamAuxFim = ftell( file );           // ver isso aqui
            processo[i].ini = tamAuxFim;
            processo[i].fim = tamanho;
            processo[i].qdeSentenca = 0;
            processo[i].qdePalavra = 0;
            processo[i].sentencaPrimo = 0;
            processo[i].palavraPrimo = 0;
        }
        /* Cuida dos outros processos */
        else{
            continua = 1;
            fseek(file, tamAuxIni + bloco, SEEK_SET);
            while(continua && !feof( file )){
                fscanf( file, "%c", &c );
                if ( c == '\n'){
                    /* Voltar o ponteiro uma posição, ou seja, descartar a posição do '\n' */
                    //fseek( file, -1, SEEK_CUR ); // erro aqui, é necessário capturar o \n
                    tamAuxFim = ftell( file );

                    processo[i].ini = tamAuxIni;
                    processo[i].fim = tamAuxFim;
                    processo[i].qdeSentenca = 0;
                    processo[i].qdePalavra = 0;
                    processo[i].sentencaPrimo = 0;
                    processo[i].palavraPrimo = 0;
                    continua = 0;
                }
            }

            fseek( file, 2, SEEK_CUR);
            tamAuxIni = ftell( file );

        }
	printf("2-Bloco #%d começa em %d termina em: %d\n", i, processo[i].ini, processo[i].fim);
	}

}


int main (int argc, char *argv[]) {

	MPI_Status status;
	int nroProc, myRank, nameLen;
	char sentenca [MAX_LETRAS_SENTENCA];
	char processorName[MPI_MAX_PROCESSOR_NAME];
	int slave= 1, ROOT = 0;                                     //nota: slave = 1 não é mais utilizada
	struct timeval iniTempo;
	struct timeval fimTempo;

    /* Variáveis novas*/
	int tipo_tag;
    int num_slaves;
    int sending_to_slave;
    int receiving_from_slave;
    int slave_receives;
    int slave_sends;
    int indice_do_processo;
    int i;
    int quantidadeDePalavras = 0;
    int quantidadeDePrimos = 0;
    int terminou_slave = 0;

	MPI_Init (&argc, &argv);

	MPI_Comm_size (MPI_COMM_WORLD, &nroProc);
	MPI_Comm_rank (MPI_COMM_WORLD, &myRank);
	MPI_Get_processor_name( processorName, &nameLen );

    num_slaves = nroProc -1;

	//printf( "Hello from processor: %s, myRank: %d out of %d processors.\n", processorName, myRank, nroProc );

	gettimeofday( &iniTempo, NULL );
	double tS = iniTempo.tv_sec + ( iniTempo.tv_usec / 1000000.0 );


    Posicao *processo = ( Posicao* ) calloc(num_slaves, sizeof( Posicao ));

    /** Abre o arquivo **/
    FILE *file = fopen ( "teste.txt", "r" );

    /** Arquivo aberto com sucesso **/
    if ( file != NULL ) {

        if ( myRank == ROOT ) {

            particionamentoArquivo( file, &num_slaves, processo );
            rewind( file );

            /* Tag, o quinto campo do SMPI_end recebe um identificador e envia sentenças aos escravos*/
            tipo_tag = FROM_MASTER;
            for (sending_to_slave=1; sending_to_slave<=num_slaves; sending_to_slave++){
                indice_do_processo = sending_to_slave - 1;
                MPI_Send ( &processo[indice_do_processo], sizeof( Posicao ), MPI_BYTE, sending_to_slave, tipo_tag, MPI_COMM_WORLD ); /****/

            }

             /* Recebe os dados depois de já terem sido processados pelos escravos */
            /*tipo_tag = FROM_WORKER;
            for (receiving_from_slave = 1; receiving_from_slave<=num_slaves; receiving_from_slave++){
                indice_do_processo = receiving_from_slave - 1;
                MPI_Recv ( &1, sizeof( MPI_INT ), MPI_INT, receiving_from_slave, tipo_tag, MPI_COMM_WORLD, &status);
            }*/

            MPI_Reduce(&terminou_slave, &num_slaves, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);

            /** calcula o número total de palíndromos e primos **/
            for(i=0; i<num_slaves; i++){
                quantidadeDePalavras += processo[i].qdePalavra;
                quantidadeDePrimos += processo[i].palavraPrimo;
            }
            printf("\n\n\n**>>%d\n**>>%d", quantidadeDePalavras, quantidadeDePrimos);
            MPI_Abort( MPI_COMM_WORLD, 911 );

        }

        if (myRank > ROOT) {

            tipo_tag = FROM_MASTER;
            //não precisa usar MPI_Type_create_struct, é possível passagem por endereço e indicar por MPI_BYTE
            MPI_Recv ( &processo[indice_do_processo], sizeof(  Posicao ), MPI_BYTE, ROOT, tipo_tag, MPI_COMM_WORLD, &status); /****/
            //printf ( "*** olá: %d to %d \n", processo[indice_do_processo].ini, processo[indice_do_processo].fim );

            fseek( file, processo[indice_do_processo].ini, SEEK_SET);
            while( ftell(file) <= processo[indice_do_processo].fim){
                printf("\n>> :D\n");
                fscanf( file, "%[^\n]\n" ,sentenca);
                palindromoPalavra( sentenca, processo[indice_do_processo].qdePalavra, processo[indice_do_processo].palavraPrimo );
            }
            /*
            tipo_tag = FROM_WORKER;
            //Tem de retornar para o processo principal
            MPI_Send ( 1, sizeof( MPI_INT ), MPI_INT, ROOT, tipo_tag, MPI_COMM_WORLD);
            */
            terminou_slave++;
        }

        fclose ( file );
        //MPI_Abort( MPI_COMM_WORLD, 911 );
    }


    /** ERRO ao abrir o arquivo**/
    else {
        printf("ERRO: na abertura do arquivo\n");
        //MPI_Finalize();
        exit( 1 );
    }

	gettimeofday( &fimTempo, NULL );
	double tE = fimTempo.tv_sec + ( fimTempo.tv_usec / 1000000.0 );

	//printf( "Ha %d palavras palindromos e dentre elas %d sao primos\n", qdePalindromo, qdePrimo );
	printf( "Tempo de execução: %.3lf\n", tE - tS );

	MPI_Finalize();

	return 0;
}
