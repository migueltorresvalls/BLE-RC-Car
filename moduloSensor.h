#ifndef MODULOSENSOR_H
#define	MODULOSENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif

void inicializarSensor();
void detectarObstaculos(int distancia_maxima);
int getTicks();

#ifdef	__cplusplus
}
#endif

#endif	/* MODULOSENSOR_H */
