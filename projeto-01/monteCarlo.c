<<<<<<< .mine
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
=======
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

void inicializarVariaveis( mpf_t* pontosCirculo, mpf_t* totaisPontos, mpf_t* PI, int* nroLoop ) {

	/* Precisao de casas decimais de acordo com o número de Loop
	 * para as variaveis do tipo mpf_init
	 */
	//int numBits = ( int ) ( ceil( ( double ) nroLoop * 3.33 ) );
	mpf_set_default_prec( 10000000 );

	mpf_init( *PI );
	mpf_init( *totaisPontos );
	mpf_init( *pontosCirculo );

	mpf_set_d( *totaisPontos, *nroLoop );

}

/*
 * Nessa funçao trabalhamos com dois arquivos (uma armazena temporariamente valor de PI e outra com valor de PI)
 * a utilização desses dois arquivos foi justamente para facilitar o manuseio dos mesmos.
 */
void escrevePI( mpf_t* tmp ) {

	FILE *fileTmp, *filePI;
	/* Variaveis que serao utilizados, se necessario, para o auxilio da formatacao do valores de PI */
	int carac = 0, grupo = 0, linhas = 0;
	int grupoAux = 0, linhasAux = 0;
	int v;

	if( ( fileTmp = fopen( "pi.tmp", "w" ) ) && ( filePI = fopen( "pi.txt", "w") ) ) {

		/* Escreve o valor de PI no arquivo temporário */
		mpf_out_str( fileTmp, 10, 0, *tmp );	// Maxima precisao
		fflush( fileTmp );
		freopen( "pi.tmp", "r", fileTmp );

		/* Ajuste do ponteiro no arquivo temporario ( fileTmp ) */
		rewind( fileTmp );
		fseek( fileTmp, 2, SEEK_SET );
		v = fgetc ( fileTmp );

		fprintf( filePI, "Valor de PI: 3,\n" );

		while( ( ( v = fgetc( fileTmp ) ) != EOF ) ) {

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


		fputc( '\n', filePI );
		fputc( '\n', filePI );
		fclose( fileTmp );
		fclose( filePI );
		remove( "pi.tmp" );

	}

	else
		printf( "Erro no arquivo pi!!\n" );

}

void monteCarlo( mpf_t* pontosCirculo, mpf_t* totaisPontos, mpf_t* PI, int* nroLoop ) {

	double i, x, y;
	double cont = 0, valor;

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

int main( int argc, char* argv[] ) {

	int nroLoop;
	mpf_t pontosCirculo, totaisPontos, PI;

	if( argc == 2 )
		nroLoop = atof( argv[1] );

	inicializarVariaveis( &pontosCirculo, &totaisPontos, &PI, &nroLoop );

	monteCarlo( &pontosCirculo, &totaisPontos, &PI, &nroLoop );

	valorPI( &PI );

	escrevePI( &PI );

	return EXIT_SUCCESS;
}
>>>>>>> .r18
