#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>
#include <limits.h>
#include <pthread.h>

/* ============= Variaveis globais ============ */
/* As variaveis mutex1, mutex2 e mutex3 eh utilizada para controlar a sincronizacao das threads */
pthread_mutex_t mutex1, mutex2, mutex3;
mpf_t pontosCirculo, totaisPontos, PI;
int nroThreads;
/* Atribui o valor maximo inteiro sem sinal a variavel nroLoop, 
 * para maquina de 32 bits (4,294,967,295)
 */
unsigned long int nroLoop = ULONG_MAX;

void inicializarVariaveis() {

	/* Precisao de casas decimais de no minimo de 15 milhoes de bits
	 * para as variaveis do tipo mpf_init 
	 */
	int numBits = ( int ) ( ceil( ( double ) 500000 * 3.14 ) );
	mpf_set_default_prec( numBits );

	mpf_init( PI );
	mpf_init( totaisPontos );
	mpf_init( pontosCirculo );

	mpf_set_d( pontosCirculo, 0.0 );

	mpf_set_d( totaisPontos, nroLoop );

	pthread_mutex_init( &mutex1, NULL );
	pthread_mutex_init( &mutex2, NULL );
	pthread_mutex_init( &mutex3, NULL );

}

void valorPI() {

	printf( "\nPI: " );
	mpf_out_str( NULL, 10, 0, PI );
	printf( "\n" );
}

void *monteCarlo( void* k ) {

	int numero = ( int ) k;
	int interacao = 0;
	double x, y, valor;
	unsigned long int cont = 0, i;
	
	pthread_mutex_lock( &mutex1 );
	printf("\tThread %d inicializado\n", numero );
	pthread_mutex_unlock( &mutex1 );

	/* Divide o total de interacoes em partes iguais para cada thread */
	for( i = numero; i <= nroLoop; i += nroThreads ) {

		interacao++;
		x = drand48();		// Sorteia um numero entre 0 e 1
		y = drand48();		// Sorteia um numero entre 0 e 1	

		valor = ( ( x*x ) + ( y*y ) );

		/* Caso a condicao seja satisfeita, significa que o ponto
		 * localiza dentro do circulo
		 */
		if( valor <= 1)
			cont++;

	}

	pthread_mutex_lock( &mutex2 );
	
	/* PI = 4.0 * pontosCirculo / totaisPontos */
	mpf_set_d( pontosCirculo, cont );	// pontosCirculo = cont
	mpf_mul_ui( PI, pontosCirculo, 4L );	// PI = 4 * pontosCirculo
	mpf_div( PI, PI, totaisPontos );	// PI = PI / totaisPontos

	pthread_mutex_unlock( &mutex2 );

	pthread_mutex_lock( &mutex3 );
	printf( "Thread %d: %d interacoes\n", numero, interacao);	
	pthread_mutex_unlock( &mutex3 );

	pthread_exit( NULL );

}

void criacaoThreads( int* nroThreads, pthread_t* thread ) {

	int i, j, flag;

	for( i = 0; i < *nroThreads; i++ ) {

		flag = pthread_create( &thread[ i ], NULL, monteCarlo, ( void *)( i + 1 ) );

		if( flag ) {

			printf( "ERRO na criacao da pthread %d!!\n", i + 1 );
			exit( 0 );

		} else {

			pthread_mutex_lock( &mutex1 );
			printf("Thread %d criado\n", i + 1);
			pthread_mutex_unlock( &mutex1 );
		}
	}

	/* Uniao das threads apos a execucao de todas elas */
	for( j = 0; j < *nroThreads; j++ )

		pthread_join( thread[ j ], NULL );

}

void limpaVariaveis( ) {

	mpf_clear( pontosCirculo );
	mpf_clear( totaisPontos );
	mpf_clear( PI );
}

int main( int argc, char* argv[] ) {
		
	if( argc == 2 )  
		nroThreads = atoi( argv[ 1 ] );
		
	pthread_t thread[ nroThreads ];

	inicializarVariaveis();

	criacaoThreads( &nroThreads, thread );

	//valorPI();

	limpaVariaveis();
	
	return EXIT_SUCCESS;
}
