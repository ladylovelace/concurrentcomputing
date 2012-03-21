/* 
 * File:   borwein.h
 * Author: lovelace
 *
 * Created on March 18, 2012, 6:10 PM
 */

#ifndef BORWEIN_H
#define	BORWEIN_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif
extern void inicializarBorweinSeq( mpf_t a, mpf_t y, int nroLoop ) ;
extern void resolverBorwein(mpf_t a, mpf_t y, int nroLoop);
#endif	/* BORWEIN_H */

