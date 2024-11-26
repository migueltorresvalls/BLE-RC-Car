#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include "Pic32Ini.h"
#include "UART1.h"
#include "moduloMotor.h"
#include "moduloSensor.h"


#define DISTANCIA_MAXIMA 40         // 15 cm aprox

int main(void){
    char car_rx;
    
    ANSELC &= ~((0xF));
    
    LATA = 0;
    LATB = 0;
    LATC = 0xF;
    
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    
    INTCONbits.MVEC = 1; 
    asm(" ei");
    
    inicializarUART1(9600); // Inicializamos la UART 1 a 9600 baudios
    
    inicializarMotor1(); // Inicializamos motor con factor servicio 0%
    inicializarMotor2(); // Inicializamos motor con factor servicio 0%
        
    inicializarSensor(); // Inicializamos sensor con periodo de trigger 50ms
    
    while(1){
        detectarObstaculos(DISTANCIA_MAXIMA);
        
        car_rx = getcUART();
        if (car_rx != 0){ //Ha llegado algo y con la sintaxis correcta
            setMov(car_rx);
        }
    }
    return 0;
}

