
#include <stdio.h>
#include <gmp.h>
#include <math.h>
#include <pthread.h>
void inicializarBorwein( mpf_t a, mpf_t y, int nroLoop ) {
	
	int precisao = 1 << nroLoop;	// precisao = 2 ^ nroLoop
                mpf_t tmpr,eq;
                
	mpf_set_default_prec( 10000000 );

	mpf_init( a );
	mpf_init( y );
                mpf_init(tmpr);
                mpf_init( eq );
                
                mpf_sqrt_ui( tmpr, 2L);
                
                mpf_mul_ui(eq,tmpr,4L);
                mpf_ui_sub(a,6L,eq); //a0 = 6-4*sqr(2)
                mpf_sub_ui(y,tmpr,1L); //y0 = sqr(2) - 1
}
void resolverBorweinSeq(mpf_t a, mpf_t y, int nroLoop) {
    mpf_t va[100], vy[100],ptemp,stemp,atemp,mtemp,f1,f2,f3;
    int i,sum;
    unsigned long  res;
    mpf_init( va[0] );
    mpf_init( vy[0] );
    mpf_init( ptemp );
    mpf_init( stemp );
    mpf_init( atemp );
    mpf_init( mtemp );
    mpf_init(f1);
    mpf_init(f2);
    mpf_init(f3);
    mpf_init_set(va[0],a);
     mpf_init_set(vy[0],y);
    for (i=1;i<12;i++){
        mpf_init_set_ui(va[i],0.0);
        mpf_init_set_ui(vy[i],0.0);
    }
    for (i=0;i<nroLoop;i++){
        
        // Encontrando y(k+1) = 1- ()/1+()
        mpf_pow_ui(ptemp,vy[i],4L); // y(k)^4
        mpf_ui_sub(stemp,1L,ptemp);//1- y(k)^4
        mpf_sqrt(ptemp,stemp); //^1/2
        mpf_sqrt(mtemp,ptemp); //^1/2 again = 1/4
        mpf_ui_sub(stemp,1L,mtemp); // 1- ()
        mpf_add_ui(atemp,mtemp,1L); //1+()
        mpf_div(vy[i+1],stemp,atemp); // y(k+1)
        
        //Encontrando a(k+1) = a(k)*(1+ y(k+1))^4 -2^(2k+3)*y(k+1)*(1+y(k+1)+y(k+1)^2)
        
        // f1 = a(k)*(1+ y(k+1))^4
        mpf_add_ui(atemp,vy[i+1],1L); // (1+ y(k+1)) 
        mpf_pow_ui(ptemp,atemp,4L);// (1+ y(k+1)) )^4
        mpf_mul(f1,va[i],ptemp);// a(k)*(1+ y(k+1))^4
        
        //f2 = -2^(2k+3)*y(k+1)
        sum = 2*i +3;
        res = pow(2,sum);
        mpf_add(atemp,vy[i+1],ptemp);
        mpf_mul_ui(f2,vy[i+1],res);
        
        //f3 = * (1+y(k+1)+y(k+1)^2)
        mpf_pow_ui(ptemp,vy[i+1],2L); //y(k+1)^2
        mpf_add(atemp,vy[i+1],ptemp); //y(k+1)+y(k+1)^2
        mpf_add_ui(f3,atemp,1L); //(1+y(k+1)+y(k+1)^2
        
        
        mpf_mul(mtemp,f2,f3);//f2*f3
        mpf_sub(va[i+1],f1,mtemp);//f1-f2*f3 
        
        gmp_printf("%.100Ff\n",va[i+1]);
        mpf_clear(va[i]);
        mpf_clear(vy[i]);
    }
}

void resolverBorweinPar(){
    
}