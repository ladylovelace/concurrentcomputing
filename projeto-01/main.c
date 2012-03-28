/* 
 * File:   main.c
 * Author: lovelace
 *
 * Created on March 18, 2012, 6:00 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "borwein.h"

/*
 * 
 */

int main(int argc, char** argv) {
    
    inicializarBorwein();
    resolverBorweinSeq();
    //resolverBorweinPar();
    
    return (EXIT_SUCCESS);
}

