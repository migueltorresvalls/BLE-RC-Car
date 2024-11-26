#include <xc.h>
#include <stdlib.h>
#include "moduloMotor.h"
#include "Pic32Ini.h"

#define PIN_TRIG 5          // RC5
#define PIN_ECHO 14          // RB14

#define PER_10_US 49

// Para que se pare en 10 cm el ancho del pulso es 580us. El factor es 58.

// El maximo del pulso es 36ms. Redondeamos a 40ms. Para ello necesitamos prescaler 4

// Usamos Timer 4 para controlar el trigger y Timer 5 para medir el ancho del pulso

static int ticks = 0;
static int distancia = 0; 
static int medida = 0; 
static int obstaculo_detectado = 0; // 0 no detectado. 1 detectado 

void inicializarSensor(){
    ANSELC &= ~(1 << PIN_TRIG);
    ANSELB &= ~(1 << PIN_ECHO);
    
    TRISB |= (1 << PIN_ECHO);
    
    // Timer 4 para generar trigger
    T4CON = 0;
    PR4 = PER_10_US; // 10 us
    TMR4 = 0;
    IFS0bits.T4IF = 0; 
    IEC0bits.T4IE = 1; 
    IPC4bits.T4IP = 2; 
    IPC4bits.T4IS = 0; 
    T4CON = 0x8000; // Timer 4 ON
    
    // Timer 5 para medir echo
    T5CON = 0;
    TMR5 = 0;
    IFS0bits.T5IF = 0; 
    IEC0bits.T5IE = 1; 
    IPC5bits.T5IP = 3; 
    IPC5bits.T5IS = 0; 
    T5CON = 0x80A0; // Timer 5 ON, TGATE = 1, TCKPS = 2 (prescalado 4)
    
    SYSKEY = 0xAA996655; // Se desbloquean los registros
    SYSKEY = 0x556699AA; // de configuracion.
    T5CKR = 1; // T5CK conectado al RB14.
    SYSKEY = 0x1CA11CA1; // Se vuelven a bloquear.
}

void detectarObstaculos(int distancia_maxima){
    
    if (medida){
        asm(" di");
        medida = 0; 
        asm(" ei");
        
        
        if (distancia/58 < distancia_maxima){
            LATCCLR = 0xF;
            obstaculo_detectado = 1;
            apagarMotores();
        }else{
            LATCSET = 0xF;
            obstaculo_detectado = 0;
        }
    }
}


__attribute__((vector(_TIMER_4_VECTOR), interrupt(IPL2SOFT), nomips16))
void InterrupcionTimer4 (void)
    {
    ticks ++; 
    IFS0bits.T4IF = 0;
    LATCCLR = (1 << PIN_TRIG);
  
    if (ticks >= 5000){
        LATCSET = (1 << PIN_TRIG);
        ticks = 0; 
    }
}

__attribute__((vector(_TIMER_5_VECTOR), interrupt(IPL3SOFT), nomips16))
void InterrupcionTimer5 (void){
    IFS0bits.T5IF = 0; 
 
    distancia = TMR5; 
    TMR5 = 0; 
    medida = 1;
}

int getObstaculoDetectado(){
    return obstaculo_detectado; 
}
