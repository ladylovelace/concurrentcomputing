#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>
#include <omp.h>

#define TRUE 1
#define FALSE 0
#define MAX_LETRAS_SENTENCA 10000


/* tamanho dos vetores auxiliares e vetores principais (que agrupand os vet. auxiliares) são conhecidos a priori*/
#define TAM_VETOR_1 30  //26
#define TAM_VETOR_2 230 // 223
#define TAM_VETOR_3 1200 //1099
#define TAM_VETOR_4 3800 //3704
#define TAM_VETOR_5 8200 //8160

typedef struct {
    int ini;
    int fim;
	int qdePalavraTam1;
	int qdePalavraTam2;
	int qdePalavraTam3;
	int qdePalavraTam4;
	int qdePalavraTam5;
	char **v1_aux;
	char **v2_aux;
	char **v3_aux;
	char **v4_aux;
	char **v5_aux;
} Bloco;


/*
 * Função que particiona o arquivo de entrada. Esta "divide" em blocos o arquivo de acordo com o número de escravo,
 * ou seja, cada escravo recebe um parâmetro de início e fim do bloco. Não esquecendo que a função realiza um tratamento
 * para que todos os bloco tenha somente sentenças completas.
 */
void particionamentoArquivo( FILE *file, int *nroProc, Bloco *processo ){

	int i, j, continua;
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

					continua = 0;
				}
			}

			tamAuxIni = tamAuxFim;
		}

		processo[i].qdePalavraTam1 = 0;
        processo[i].qdePalavraTam2 = 0;
        processo[i].qdePalavraTam3 = 0;
        processo[i].qdePalavraTam4 = 0;
        processo[i].qdePalavraTam5 = 0;
        processo[i].v1_aux = (char**)calloc(TAM_VETOR_1,sizeof(char*));
        processo[i].v2_aux = (char**)calloc(TAM_VETOR_2,sizeof(char*));
        processo[i].v3_aux = (char**)calloc(TAM_VETOR_3,sizeof(char*));
        processo[i].v4_aux = (char**)calloc(TAM_VETOR_4,sizeof(char*));
        processo[i].v5_aux = (char**)calloc(TAM_VETOR_5,sizeof(char*));
        for(j = 0; j < TAM_VETOR_1; j++){
            processo[i].v1_aux[j] = (char*)malloc(2*sizeof(char));
        }
        for(j = 0; j < TAM_VETOR_2; j++){
            processo[i].v2_aux[j] = (char*)malloc(3*sizeof(char));
        }
        for(j = 0; j < TAM_VETOR_3; j++){
            processo[i].v3_aux[j] = (char*)malloc(4*sizeof(char));
        }
        for(j = 0; j < TAM_VETOR_4; j++){
            processo[i].v4_aux[j] = (char*)malloc(5*sizeof(char));
        }
        for(j = 0; j < TAM_VETOR_5; j++){
            processo[i].v5_aux[j] = (char*)malloc(6*sizeof(char));
        }
	}
}


/*
 * Função que recebe uma linha/frase e identifica suas palavras, gerando, se for o caso, palavras randômicas
 * do tamanho de até cinco caracteres, até que a palavra gerada e a contida na frase sejam iguais.
 * Também são alterados os vetores auxiliares da estrutura Bloco, que armazenam as palavras.
 * qdePalavraTamX, que indica a quantidade de palavras de tamanho X, é utilizado como contador
 */
void capturaPalavras(char *sentenca, Bloco *processo ){
    int i, j;
    long int tamanhoSentenca;
    char palavra[10];

    /*
    tamanhoSentenca = strlen(sentenca);
    j = -1;
    for(i=0; i< tamanhoSentenca ; i++){
        if (sentenca[i] != ' '){
            if(j<4){
                j++;
                palavra[j] = sentenca[i];
            }
            else{
                while(sentenca[i] != ' ' && i< tamanhoSentenca ){
                    i++;
                }
                j = -1;
            }
        }
        else{ //achou um espaço e a palavra[] pode ter sido formada ou não.

            palavra[++j] = '\0';
            printf("\n>%s", palavra);
            j = -1;
        }
    }
    */
    /*
    tamanhoSentenca = strlen(sentenca);
    j = -1;
    for(i=0; i< tamanhoSentenca ; i++){
        if (sentenca[i] != ' '){
            j++;
            palavra[j] = sentenca[i];
        }
        else{
            if(j<5){
                palavra[++j] = '\0';
                printf("\n>%s %d", palavra, j);

            }
            j = -1;
        }
    }
    */

    strcpy(processo->v5_aux[0], "a");
    printf("\n%s",processo->v5_aux[0]);
}

int main ( int argc, char *argv[] ) {

    char sentenca [MAX_LETRAS_SENTENCA];
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

	/* Inicia o cálculo do tempo */
	gettimeofday( &iniTempo, NULL );
	double tS = iniTempo.tv_sec + ( iniTempo.tv_usec / 1000000.0 );

    Bloco *processo = ( Bloco* ) calloc( nroProc, sizeof( Bloco ) );

	if( ( file = fopen ( "frase.txt", "r" ) ) != NULL ) {

		if ( myRank == ROOT ) {

			/* separa o arquivo filtrado em blocos */
			particionamentoArquivo( file, &nroProc, processo );

			rewind( file );

			i = 0;
            //printf("numero de processos: %d", nroProc);
            //printf("rank atual: %d", myRank);
			for ( slave = 1; slave < nroProc; slave++ ){ //são realizadas (nroProc -1) iterações
				MPI_Send ( &processo[i], sizeof( Bloco ), MPI_BYTE, slave, 0, MPI_COMM_WORLD );
				i++;
			}

			/* Condição que determina que todos os slaves terminaram a tarefa */
			MPI_Reduce( &myRank, &finalizar, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD );

		}
        else {

			MPI_Recv ( &processo[myRank - 1], sizeof( Bloco ), MPI_BYTE, ROOT, 0, MPI_COMM_WORLD, &status );

			/* Posiciona o ponteiro no início do bloco correspondente para cada slave */
			fseek( file, processo[myRank - 1].ini, SEEK_SET);

			//#pragma omp parallel num_threads(2)
			while(  ftell( file ) < processo[myRank - 1].fim ) {

				fscanf( file, "%[^\n]\n" , sentenca );
				//printf("%s\n", sentenca);
				//printf("rank atual: %d", myRank);
				capturaPalavras( sentenca, &processo[myRank - 1]);
                //printf("\n\n>%s\n",processo[myRank - 1].v1_aux[1]);
			}

//			printf( "Slave:%d encontrou %d palavras palindromos e dentre elas %d sao primos\n", myRank,
//				processo[myRank - 1].qdePalavra, processo[myRank - 1].palavraPrimo );

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
