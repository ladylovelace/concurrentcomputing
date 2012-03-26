#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>
#include <pthread.h>
#include <sys/types.h>
#include <errno.h>

/*======================== Variaveis Globais ========================*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct controleThreads controle_0;
struct controleThreads controle_1;
mpf_t a, a1, b, b1, p, p1, t, t1, tmp, tmp1;
int nroLoop;

/*=========================== Cabecalhos =============================*/
int controleThreads( struct controleThreads*, int );
int bloqueioAcesso( struct controleThreads* );
void inicializarControleThreads();
void valorPI();
void* calculo_a( void* );
void* calculo_b( void* );
void* calculo_t( void* );
void* calculo_p( void* );

struct controleThreads {

	pthread_mutex_t	mutex;
	pthread_cond_t	condicao;
		int 	contador;
		int 	limiar;
};

void inicializarVariaveis() {
	
	int precisao = 1 << nroLoop;	// precisao = 2 ^ nroLoop

	/* A variavel precisao controla as casas decimais de acordo com os numeros de Loop 
	 * para as variaveis do tipo mpf_init. Para nroLoop igual a precisão e da ordem 
	 * de 45 milhoes de casas decimais
	 */
	int numBits = ( int ) ( ceil( ( double ) precisao * 3.14 ) );
	mpf_set_default_prec( numBits );

	mpf_init( a );
	mpf_init( a1 );
	mpf_init( b );
	mpf_init( b1 );
	mpf_init( p );
	mpf_init( p1 );
	mpf_init( t );
	mpf_init( t1 );
	mpf_init( tmp );
	mpf_init( tmp1 );

	mpf_set_d( a, 1.0 );		// a = 1		
	mpf_set_d( a1, 1.0 );		// a1 = 1
	mpf_set_d( t, 0.25 );		// t = 1 / 4
	mpf_set_d( t1, 0.25 );		// t1 = 1 / 4
	mpf_set_d( p, 1.0 );		// p = 1
	mpf_set_d( p1, 1.0 );		// p1 = 1

	mpf_sqrt_ui( tmp, 2L );	
	mpf_ui_div( b, 1L, tmp );	// b = 1 / sqrt( 2 )
	mpf_ui_div( b1, 1L, tmp );	// b1 = 1 / sqrt( 2 )

	inicializarControleThreads();

}

void inicializarControleThreads() {

	if ( 0 != controleThreads( &controle_0, 5 ) ) {

		printf("ERRO na inicializacao controle de threads 0\n");
		exit( 1 );
	}

	if ( 0 != controleThreads( &controle_1, 5 ) ) {

		printf("ERRO na inicializacao controle de threads 1\n");
		exit( 1 );
	}
}

int controleThreads( struct controleThreads* controle, int contador ) {
	
	controle->contador = contador;
	controle->limiar = contador;
	pthread_mutex_init( &controle->mutex, NULL );
	pthread_cond_init( &controle->condicao, NULL );

	return 0;
}

void criacaoThreads( pthread_t thread_a, pthread_t thread_b, pthread_t thread_t, pthread_t thread_p ) {

	if( ( pthread_create( &thread_a, NULL, calculo_a, NULL ) ) == 0 )
		printf( "Thread_a criado\n" );
	else
		printf( "ERRO na criacao da Thread_a\n" );

	if( ( pthread_create( &thread_b, NULL, calculo_b, NULL ) ) == 0 )
		printf( "Thread_b criado\n" );
	else
		printf( "ERRO na criacao da Thread_b\n" );

	if( ( pthread_create( &thread_t, NULL, calculo_t, NULL ) ) == 0 )
		printf( "Thread_t criado\n" );
	else
		printf( "ERRO na criacao da Thread_t\n" );

	if( ( pthread_create( &thread_p, NULL, calculo_p, NULL ) ) == 0 )
		printf( "Thread_p criado\n" );
	else
		printf( "ERRO na criacao da Thread_p\n" );

	int i;	
	for( i = 0; i < nroLoop; i++ ) {

		bloqueioAcesso( &controle_0 );
		
		valorPI();

		bloqueioAcesso( &controle_1 );	
	}

	pthread_join( thread_a, NULL );
	pthread_join( thread_b, NULL );
	pthread_join( thread_t, NULL );
	pthread_join( thread_p, NULL );

}

/* Abaixo estao as quatros threads para cada operacao matematica */
void* calculo_a( void* arg ) {

	int i;

	printf( "Thread_a inicializado\n" );

	for( i = 0; i < nroLoop; i++ ) {
		
		bloqueioAcesso( &controle_0 );

		mpf_add( tmp, a, b );
		mpf_div_ui( a1, tmp, 2L );	// a1 = ( a + b ) / 2
		
		bloqueioAcesso( &controle_1 );
		/* Atualizacao dos dados */
		mpf_set( a, a1 );

	}

	pthread_exit( NULL );
}

void* calculo_b( void* arg ) {

	int i;

	printf( "Thread_b inicializado\n" );

	for( i = 0; i < nroLoop; i++ ) {
		
		bloqueioAcesso( &controle_0 );
		
		mpf_mul( tmp, b, a );
		mpf_sqrt( b1, tmp );		// b1 = sqrt( b * a )
		
		bloqueioAcesso( &controle_1 );
		/* Atualizacao dos dados */
		mpf_set( b, b1 );
	}

	pthread_exit( NULL );
}

void* calculo_t( void* arg ) {

	int i;

	printf( "Thread_t inicializado\n" );

	for( i = 0; i < nroLoop; i++ ) {

		bloqueioAcesso( &controle_0 );
		
		mpf_sub( tmp, a, a1 );
		mpf_mul( tmp, tmp, tmp );
		mpf_mul( tmp, p, tmp );
		mpf_sub( t1, t, tmp );		// t1 = t - p * ( a - a1 ) ^ 2
		
		bloqueioAcesso( &controle_1 );
		/* Atualizacao dos dados */
		mpf_set( t, t1 );

	}

	pthread_exit( NULL );
}

void* calculo_p( void* arg ) {

	int i;

	printf( "Thread_p inicializado\n" );

	for( i = 0; i < nroLoop; i++ ) {

		bloqueioAcesso( &controle_0 );

		mpf_mul_ui( p1, p, 2L);		// p1 = 2 * p

		bloqueioAcesso( &controle_1 );
		/* Atualizacao dos dados */
		mpf_set( p, p1 );	
	}

	pthread_exit( NULL );
}

/* Bloqueio de acesso para prover exclusão mútua. O bloqueio permite a uma thread 
 * ter total controle de uma variável, ou seja, previne-se que outras threads acessem 
 * a mesma região de memória até que se faça o desbloqueio.
 */
int bloqueioAcesso( struct controleThreads* controle ) {

	int retorno = 0;

	/* Se argumento for inválido retorna o codigo de erro */
	if ( NULL == controle )
		return EINVAL; 

	/* Se argumento for inválido retorna o codigo de erro */
	if ( 0 != pthread_mutex_lock( &controle->mutex ) )
		return EINVAL;

	controle->contador--;
	
	if ( 0 != controle->contador ) {

		pthread_cond_wait( &controle->condicao, &controle->mutex );

	/* Ultima thread atinge o limite de bloqueio de acesso */
	} else {
		/* Acorda todas as threads */	
		pthread_cond_broadcast( &controle->condicao );
		controle->contador = controle->limiar;
		retorno = -1;
	}

	pthread_mutex_unlock( &controle->mutex );

	return retorno;
}

void valorPI() {
	
	// calculo do PI 
	printf( "calculando PI...\n" );
	mpf_add( tmp, a, b );
	mpf_mul( tmp, tmp, tmp );
	mpf_mul_ui( tmp1, t, 4L );
	mpf_div( tmp, tmp, tmp1 );		// PI = ( a + b ) ^ 2 / ( 4 * t )

	mpf_out_str( NULL, 10, 0, tmp );
	printf( "\n" );

}

void limparVariaveis() {

	mpf_clear( a );	
	mpf_clear( a1 );	
	mpf_clear( b );	
	mpf_clear( b1 );	
	mpf_clear( t );	
	mpf_clear( t1 );	
	mpf_clear( p );	
	mpf_clear( p1 );
	mpf_clear( tmp );
	mpf_clear( tmp1 );

}

int main( int argc, char* argv[] ) {

	pthread_t thread_a, thread_b, thread_t, thread_p;

	/* Numeros de iteracao recebida pode ser recebida por parametro ou mantera o padrao de 24 iteracao, 
	 * ou seja, o valor padrao eh suficiente para gerar casas decimais acima de 10 milhoes
	*/
	if( argc == 2 ) 

		nroLoop = atoi( argv[1] );
		
	else
		nroLoop = 25;

	inicializarVariaveis();

	criacaoThreads( thread_a, thread_b, thread_t, thread_p );
		
	limparVariaveis();
	
	return EXIT_SUCCESS;
}
