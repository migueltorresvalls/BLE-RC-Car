#ifndef UART1_H
#define	UART1_H
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

void inicializarUART1(int baudios) ;
void putsUART(char s[]);
char getcUART(void);



#ifdef	__cplusplus
}
#endif

#endif	/* UART1_H */
