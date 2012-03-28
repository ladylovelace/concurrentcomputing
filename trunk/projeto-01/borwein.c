
#include <stdio.h>
#include <gmp.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    mpf_t va, vy;
    mpf_t f1, f2, f3;
    int id;
} param;
param p[15];
void inicializarBorwein(void);
void resolverBorweinSeq(void);
void resolverBorweinPar(void);
void funcao_y(void *arg);
void *funcao_a(void *arg);
void *funcao_a1(void *arg);
void *funcao_a2(void *arg);
void *funcao_a3(void *arg);

void inicializarBorwein() {
    
    int i;
    mpf_t tmpr, eq;
    
    mpf_set_default_prec(10000000*log2(10));
    
    for (i = 0; i < 14; i++) {
        mpf_init(p[i].va);
        mpf_init_set_ui(p[i].va, 0.0);
        mpf_init(p[i].vy);
        mpf_init_set_ui(p[i].vy, 0.0);
        mpf_init(p[i].f1);
        mpf_init_set_ui(p[i].f1, 0.0);
        mpf_init(p[i].f2);
        mpf_init_set_ui(p[i].f2, 0.0);
        mpf_init(p[i].f3);
        mpf_init_set_ui(p[i].f3, 0.0);
    }
    mpf_init(tmpr);
    mpf_init(eq);
    p[0].id =0;
    
    mpf_sqrt_ui(tmpr, 2L);
    
    mpf_mul_ui(eq, tmpr, 4L);
    mpf_ui_sub(p[0].va, 6L, eq); //a0 = 6-4*sqr(2)
    mpf_sub_ui(p[0].vy, tmpr, 1L); //y0 = sqr(2) - 1
}

void resolverBorweinSeq() {
    time_t inicio,fim;
    mpf_t pi;
    mpf_t ptemp, stemp, atemp, mtemp, f1, f2, f3;
    int i = 0, sum;
    unsigned long res;
    inicio = time(NULL);
    mpf_init(pi);
    mpf_init(ptemp);
    mpf_init(stemp);
    mpf_init(atemp);
    mpf_init(mtemp);
    mpf_init(f1);
    mpf_init(f2);
    mpf_init(f3);
    do {
        
        // Encontrando y(k+1) = 1- ()/1+()
        mpf_pow_ui(ptemp, p[i].vy, 4L); // y(k)^4
        mpf_ui_sub(stemp, 1L, ptemp); //1- y(k)^4
        mpf_sqrt(ptemp, stemp); //^1/2
        mpf_sqrt(mtemp, ptemp); //^1/2 again = 1/4
        mpf_ui_sub(stemp, 1L, mtemp); // 1- ()
        mpf_add_ui(atemp, mtemp, 1L); //1+()
        mpf_div(p[i + 1].vy, stemp, atemp); // y(k+1)
        
        //Encontrando a(k+1) = a(k)*(1+ y(k+1))^4 -2^(2k+3)*y(k+1)*(1+y(k+1)+y(k+1)^2)
        
        // f1 = a(k)*(1+ y(k+1))^4
        mpf_add_ui(atemp, p[i + 1].vy, 1L); // (1+ y(k+1)) 
        mpf_pow_ui(ptemp, atemp, 4L); // (1+ y(k+1)) )^4
        mpf_mul(f1, p[i].va, ptemp); // a(k)*(1+ y(k+1))^4
        //f2 = -2^(2k+3)*y(k+1)
        sum = 2 * i + 3;
        res = pow(2, sum);
        mpf_add(atemp, p[i + 1].vy, ptemp);
        mpf_mul_ui(f2, p[i + 1].vy, res);
        //f3 = * (1+y(k+1)+y(k+1)^2)
        mpf_pow_ui(ptemp, p[i + 1].vy, 2L); //y(k+1)^2
        mpf_add(atemp, p[i + 1].vy, ptemp); //y(k+1)+y(k+1)^2
        mpf_add_ui(f3, atemp, 1L); //(1+y(k+1)+y(k+1)^2
        
        mpf_mul(mtemp, f2, f3); //f2*f3
        mpf_sub(p[i + 1].va, f1, mtemp); //f1-f2*f3 
        mpf_clear(p[i].va);
        mpf_clear(p[i].vy);
        i++;
    }    while (i < 13); // nao para
    mpf_ui_div(pi,1L,p[i-1].va);
    gmp_printf("PI: %.10Ff\n",pi);
    fim = time(NULL);
    printf("Tempo em segundos: %f\n",difftime(fim, inicio));
}

void funcao_y(void *arg) {
    param *a = (param *) arg;
    mpf_t ptemp, stemp, atemp, mtemp;
    mpf_init(ptemp);
    mpf_init(stemp);
    mpf_init(atemp);
    mpf_init(mtemp);
    
    // Encontrando y(k+1) = 1- ()/1+()
    mpf_pow_ui(ptemp,p[a->id-1].vy, 4L); // y(k)^4
    mpf_ui_sub(stemp, 1L, ptemp); //1- y(k)^4
    mpf_sqrt(ptemp, stemp); //^1/2
    mpf_sqrt(mtemp, ptemp); //^1/2 again = 1/4
    mpf_ui_sub(stemp, 1L, mtemp); // 1- ()
    mpf_add_ui(atemp, mtemp, 1L); //1+()
    mpf_div(p[a->id].vy, stemp, atemp); // y(k+1)
    mpf_clear(p[a->id-1].vy);
}

void *funcao_a1(void *arg) {
    param *a = (param *) arg;
    mpf_t ptemp, atemp;
    mpf_init(ptemp);
    mpf_init(atemp);
    
    // f1 = a(k)*(1+ y(k+1))^4
    mpf_add_ui(atemp, p[a->id].vy, 1L); // (1+ y(k+1))
    mpf_pow_ui(ptemp, atemp, 4L); // (1+ y(k+1)) )^4
    mpf_mul(a->f1, p[a->id - 1].va, ptemp); // a(k)*(1+ y(k+1))^4
    pthread_exit(&a->f1);
}

void *funcao_a2(void *arg) {
    param *a = (param *) arg;
    mpf_t ptemp, atemp;
    int sum;
    unsigned long res;
    mpf_init(ptemp);
    mpf_init(atemp);
    sum = 2 * (a->id - 1) + 3;
    res = pow(2, sum);
    mpf_add(atemp, p[a->id].vy, ptemp);
    mpf_mul_ui(a->f2, p[a->id].vy, res);
    pthread_exit(&a->f2);
}

void *funcao_a3(void *arg) {
    param *a = (param *) arg;
    mpf_t ptemp, atemp, mtemp;
    mpf_init(ptemp);
    mpf_init(atemp);
    mpf_init(mtemp);
    //f3 = * (1+y(k+1)+y(k+1)^2)
    mpf_pow_ui(ptemp, p[a->id].vy, 2L); //y(k+1)^2
    mpf_add(atemp, p[a->id].vy, ptemp); //y(k+1)+y(k+1)^2
    mpf_add_ui(a->f3, atemp, 1L); //(1+y(k+1)+y(k+1)^2
    pthread_exit(&a->f3);
}

void *funcao_a(void *arg) {
    param *a = (param *) arg;
    mpf_t mtemp;
    mpf_init(mtemp);
    mpf_mul(mtemp, a->f2, a->f3); //f2*f3
    mpf_sub(a->va, a->f1, mtemp); //f1-f2*f3
    pthread_exit(&a->va);
}

void resolverBorweinPar() {
    time_t inicio,fim;
    mpf_t pi;
    mpf_init(pi);
    pthread_t thread_res[5];
    int j = 1;
    mpf_t *res;
    inicio = time(NULL);
    do {
        p[j].id = j;
        pthread_create(&thread_res[0], NULL, (void *) &funcao_y, &p[j]);
        pthread_join(thread_res[0], (void *) &res);
        pthread_create(&thread_res[1], NULL, (void *) &funcao_a1, &p[j]);
        pthread_create(&thread_res[2], NULL, (void *) &funcao_a2, &p[j]);
        pthread_create(&thread_res[3], NULL, (void *) &funcao_a3, &p[j]);
        pthread_join(thread_res[1], (void *) &res);
        pthread_join(thread_res[2], (void *) &res);
        pthread_join(thread_res[3], (void *) &res);
        pthread_create(&thread_res[4], NULL, (void *) &funcao_a, &p[j]);
        pthread_join(thread_res[4], (void *) &res);
        j++;
    } while (j < 13 );
    mpf_ui_div(pi,1L,p[j-1].va);
    gmp_printf("PI: %.10Ff\n",pi);
    fim = time(NULL);
    printf("Tempo em segundos: %f\n",difftime(fim, inicio));
}