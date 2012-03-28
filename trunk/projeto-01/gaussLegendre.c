#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

void inicializarVariaveis( mpf_t* a, mpf_t* b, mpf_t* p, mpf_t* y, mpf_t* t, mpf_t* tmp, int* nroLoop ) {
	
	int precisao = 1 << *nroLoop;	// precisao = 2 ^ nroLoop

	/* Precisao de casas decimais de acordo com os numeros de Loop 
	 * para as variaveis do tipo mpf_init 
	 */
	int numBits = ( int ) ( ceil( ( double ) precisao * 3.14 ) );
	mpf_set_default_prec( numBits );

	mpf_init( *a );
	mpf_init( *b );
	mpf_init( *p );
	mpf_init( *y );
	mpf_init( *t );
	mpf_init( *tmp );

	mpf_set_d( *a, 1.0 );		// a = 1	
	mpf_set_d( *t, 0.25 );		// t = 1 / 4
	mpf_set_d( *p, 1.0 );		// p = 1

	mpf_sqrt_ui( *tmp, 2L );	
	mpf_ui_div( *b, 1L, *tmp );	// b = 1 / sqrt( 2 ) 
}

void valorPI( mpf_t* a, mpf_t *b, mpf_t* y, mpf_t* t, mpf_t* tmp) {
	
	// calculo do PI... 
	mpf_add( *tmp, *a, *b );
	mpf_mul( *tmp, *tmp, *tmp );
	mpf_mul_ui( *y, *t, 4L );
	mpf_div( *tmp, *tmp, *y );			// PI = ( a + b ) ^ 2 / ( 4 * t )

	mpf_out_str( NULL, 10, 0, *tmp );
	printf("\n");

}

void gaussLegendre( mpf_t* a, mpf_t *b, mpf_t* p, mpf_t* y, mpf_t* t, mpf_t* tmp, int* nroLoop ) {

	int i;

	for( i = 0; i < *nroLoop; i++ ){
	
		/* Imprime o valor de Pi a cada iteração */
		//valorPI( a, b, y, t, tmp );	

		mpf_set( *y, *a );			// y = a

		mpf_add( *tmp, *a, *b );
		mpf_div_ui( *a, *tmp, 2L );		// a = ( a + b ) / 2

		mpf_mul( *tmp, *b, *y );
		mpf_sqrt( *b, *tmp );			// b = sqrt( b * y )

		mpf_sub( *tmp, *y, *a );
		mpf_mul( *y, *tmp, *tmp );
		mpf_mul( *tmp, *p, *y );
		mpf_set( *y, *t );
		mpf_sub( *t, *y, *tmp );		// t = t - p * ( y - a ) ^ 2

		/* Na ultima iteracao nao ha necessiade de atualizar o valor de p */
		if( i == ( *nroLoop - 1 ) ) {
			
			mpf_set( *tmp, *p );
			mpf_mul_ui( *p, *tmp, 2L );	// p = 2 * p

		}
	}	
}

void limpaVariaveis( mpf_t* a, mpf_t *b, mpf_t* p, mpf_t* y, mpf_t* t, mpf_t* tmp ) {

	mpf_clear( *a );	
	mpf_clear( *b );
	mpf_clear( *p );
	mpf_clear( *y );
	mpf_clear( *t );
	mpf_clear( *tmp );

}

int main( int argc, char* argv[] ) {

	int nroLoop;
	mpf_t a, b, p, y, t, tmp;

	/* Caso o numero de iteracao nao for recebida por parametro, este manterah o padrao de 24 iteracao, 
	 * este valor eh suficiente para gerar casas decimais acima de 10 milhoes de digitos
	*/
	if( argc == 2 )
		nroLoop = atoi( argv[1] );
	
	else
		nroLoop = 25;

	inicializarVariaveis( &a, &b, &p, &y, &t, &tmp, &nroLoop ); 

	gaussLegendre( &a, &b, &p, &y, &t, &tmp, &nroLoop );

	limpaVariaveis( &a, &b, &p, &y, &t, &tmp );

	return EXIT_SUCCESS;
}
