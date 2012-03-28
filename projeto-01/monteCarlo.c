#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <gmp.h>

void inicializarVariaveis( mpf_t* pontosCirculo, mpf_t* totaisPontos, mpf_t* PI, unsigned long int* nroLoop ) {

	/* Precisao de casas decimais de no minimo de 15 milhoes de bits
	 * para as variaveis do tipo mpf_init 
	 */
	int numBits = ( int ) ( ceil( ( double ) 500000 * 3.14 ) );
	mpf_set_default_prec( numBits );

	mpf_init( *PI );
	mpf_init( *totaisPontos );
	mpf_init( *pontosCirculo );

	mpf_set_d( *totaisPontos, *nroLoop );

}

void monteCarlo( mpf_t* pontosCirculo, mpf_t* totaisPontos, mpf_t* PI, unsigned long int* nroLoop ) {

	double x, y;
	double valor;
	unsigned long int cont = 0, i;

	for( i = 0; i < *nroLoop; i++ ) {

		x = drand48();		// Sorteia um numero entre 0 e 1
		y = drand48();		// Sorteia um numero entre 0 e 1
		
		valor = ( ( x*x ) + ( y*y ) );

		if ( valor <= 1 )
			cont++;	

	}
	
	/* PI = 4.0 * pontosCirculo / totaisPontos */
	mpf_set_d( *pontosCirculo, cont);	// pontosCirculo = cont
	mpf_mul_ui( *PI, *pontosCirculo, 4L );	// PI = 4 * pontosCirculo
	mpf_div( *PI, *PI, *totaisPontos);	// PI = PI / totaisPontos

}

void valorPI( mpf_t*PI ) {

	mpf_out_str( NULL, 10, 0, *PI );
	printf("\n");
}

void limpaVariaveis( mpf_t* pontosCirculo, mpf_t* totaisPontos, mpf_t* PI ) {

	mpf_clear( *pontosCirculo );
	mpf_clear( *totaisPontos );
	mpf_clear( *PI );
}

int main( int argc, char* argv[] ) {

	/* Atribui o valor maximo inteiro sem sinal a variavel nroLoop 
	 * para maquina de 32 bits (4,294,967,295)
	 */
	unsigned long int nroLoop = ULONG_MAX;
	mpf_t pontosCirculo, totaisPontos, PI;

	inicializarVariaveis( &pontosCirculo, &totaisPontos, &PI, &nroLoop );

	monteCarlo( &pontosCirculo, &totaisPontos, &PI, &nroLoop );

	//valorPI( &PI );

	limpaVariaveis( &pontosCirculo, &totaisPontos, &PI );

	return EXIT_SUCCESS;
}

