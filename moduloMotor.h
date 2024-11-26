#ifndef MODULOMOTOR_H
#define	MODULOMOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

void inicializarMotor1(void);
void inicializarMotor2(void);
void apagarMotores(void);
void setSpeedM1(uint8_t speed);
void setSpeedM2(uint8_t speed);
void setMov(uint8_t mov);
int checkSintaxis(char word_rx);

#ifdef	__cplusplus
}
#endif

#endif	/* MODULOMOTOR_H */
