#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

void inicializarVariaveis( mpf_t a, mpf_t b, mpf_t x, mpf_t y, mpf_t t, mpf_t tmp, int nroLoop ) {
	
	int precisao = 1 << nroLoop;	// precisao = 2 ^ nroLoop

	/* Precisao do tamanho das variaveis do tipo mpf_init */
	//int numBits = ( int ) ( ceil( ( double ) precisao * 3.33 ) );
	mpf_set_default_prec( 10000000 );

	mpf_init( a );
	mpf_init( b );
	mpf_init( x );
	mpf_init( y );
	mpf_init( t );
	mpf_init( tmp );

	mpf_set_d( a, 1.0 );		// a = 1	
	mpf_set_d( t, 0.25 );		// t = 1 / 4
	mpf_set_d( x, 1.0 );		// x = 1

	mpf_sqrt_ui( tmp, 2L );	
	mpf_ui_div( b, 1L, tmp );	// b = 1 / sqrt( 2 ) 
}

/*
 * Nessa funçao trabalhamos com dois arquivos (armazena temporariamente valor de PI e outra com valor de PI) 
 * a utilização desses dois arquivos foi justamente para facilitar o manuseio dos mesmos.
 */
void escrevePI( mpf_t tmp, int nroLoop ) {

	FILE *fileTmp, *filePI; 
	/* Variaveis que serao utilizados, se necessario, para o auxilio da formatacao do valores de PI */
	int carac = 0, grupo = 0, linhas = 0, casasDecimais = 0;
	int grupoAux = 0, linhasAux = 0;
	int v, i;

	if( ( fileTmp = fopen( "pi.tmp", "w" ) ) && ( filePI = fopen( "pi.txt", "a+") ) ) {

		/* Escreve o valor de PI no arquivo temporário */
		mpf_out_str( fileTmp, 10, 0, tmp );
		fflush( fileTmp );
		freopen( "pi.tmp", "r", fileTmp );
	
		for( i = 0; i < nroLoop; i++ ) {
			
			casasDecimais = 1 << i + 1;		// casasDecimais = 2 ^ ( i + 1)
			
			/* Ajuste do ponteiro no arquivo temporario ( fileTmp ) */
			rewind( fileTmp );
			fseek( fileTmp, 2, SEEK_SET );
			v = fgetc ( fileTmp );

			fprintf( filePI, "Valor de PI: 3,\n" );

			while( ( ( v = fgetc( fileTmp ) ) != EOF ) && ( carac < casasDecimais ) ) {
			
				fputc( v, filePI );
				carac++;

				/*============== Formatacao da saida das dizimas periodicas ==================*/
				if( ( carac % 10 ) == 0 ) {
				
					fputc( ' ', filePI );
					grupoAux = ( ( grupo + 1 ) % 5 );

					if( grupoAux == 0 ) {

						fputc( '\n', filePI );
						linhasAux = ( ( linhas + 1 ) % 20 );

						if( linhasAux == 0 )
							
							fputc( '\n', filePI );
					}

				}
				/*=============================================================================*/
			}

			carac = 0;
			fputc( '\n', filePI );
			fputc( '\n', filePI );
		}

		fputc( '\n', filePI );
		fputc( '\n', filePI );
		fclose( fileTmp );
		fclose( filePI );
		//remove( "pi.tmp" );

	}

	else 
		printf( "Erro no arquivo pi!!\n" );

}

void gaussLegendre( mpf_t a, mpf_t b, mpf_t x, mpf_t y, mpf_t t, mpf_t tmp, int nroLoop ) {

	int i;

	for( i = 0; i < nroLoop; i++ ){
	
		mpf_set( y, a );			// y = a

		mpf_add( tmp, a, b );
		mpf_div_ui(a, tmp, 2L);			// a = ( a + b ) / 2

		mpf_mul( tmp, b, y );
		mpf_sqrt( b, tmp );			// b = sqrt( b * y )

		mpf_sub( tmp, y, a );
		mpf_mul( y, tmp, tmp );
		mpf_mul( tmp, x, y );
		mpf_set( y, t );
		mpf_sub( t, y, tmp );			// t = t - x * ( y - a ) ^ 2

		mpf_set( tmp, x );
		mpf_mul_ui( x, tmp, 2L );		// x = 2 * x

		// calculo do PI 
		mpf_add( tmp, a, b );
		mpf_mul( x, tmp, tmp );
		mpf_mul_ui( y, t, 4L );
		mpf_div( tmp, x, y );			// PI = ( a + b ) ^ 2 / ( 4 * t )

	}
	
	escrevePI( tmp, nroLoop );	
		
}

int main( int argc, char* argv[] ) {

	int nroLoop = 10;

	mpf_t a, b, x, y, t, tmp;

	inicializarVariaveis( a, b, x, y, t, tmp, nroLoop );
	
	gaussLegendre( a, b, x, y, t, tmp, nroLoop );

	return EXIT_SUCCESS;
}
