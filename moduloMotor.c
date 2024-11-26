#include <xc.h>
#include <stdlib.h>
#include "Pic32Ini.h"
#include "UART1.h"
#include "moduloSensor.h"


#define PIN_MOTOR1 7 //RC7
#define PIN_MOTOR2 8 //RC8
#define PIN_DIR_M1 6 //RC6
#define PIN_DIR_M2 9 //RC9

#define PER_PWM_M1 499 // 10 KHz
#define PER_PWM_M2 499 // 10KHz


// El motor 1 se controla con PW2 del micro
// El motor 2 se controla con PW1 del micro

// Un 1 en el dir motor es girar las ruedas hacia la derecha (se mueve hacia adelante)
// Un 0, al reves

// M1 es el motor izquierdo, M2 el derecho

void inicializarMotor1(void){
    ANSELC &= ~((1 << PIN_MOTOR1) | (1 << PIN_DIR_M1));
    LATC &= ~((1 << PIN_MOTOR1) | (1 << PIN_DIR_M1));
    
    LATCSET = (1<< PIN_DIR_M1);
    // Ahora el PWM para el motor
    SYSKEY =0xAA996655 ;
    SYSKEY =0x556699AA ;
    RPC7R = 5; // OC1 conectado a RC7
    SYSKEY =0x1CA11CA1 ;
    OC1CON = 0;
    OC1R = 0; // Empezamos con factor de servicio al 0%
    OC1RS = 0;
    OC1CON = 0x8006; // OC ON, modo PWM sin faltas
    T2CON = 0;
    TMR2 = 0;
    PR2 = PER_PWM_M1 ;
    T2CON = 0x8000; // T2 ON, Div = 1
}

void inicializarMotor2(void){
    ANSELC &= ~((1 << PIN_MOTOR2) | (1 << PIN_DIR_M2));
    LATC &= ~((1 << PIN_MOTOR2) | (1 << PIN_DIR_M2));
    
    LATCSET = (1<< PIN_DIR_M2);
    // Ahora el PWM para el motor
    SYSKEY =0xAA996655 ;
    SYSKEY =0x556699AA ;
    RPC8R = 5; // OC2 conectado a RC8
    SYSKEY =0x1CA11CA1 ;
    OC2CON = 0;
    OC2R = 0; // Empezamos con factor de servicio al 0%
    OC2RS = 0;
    OC2CON = 0x8006; // OC ON, modo PWM sin faltas, 
    OC2CONbits.OCTSEL = 1; // Timer 3
    T3CON = 0;
    TMR3 = 0;
    PR3 = PER_PWM_M2 ;
    T3CON = 0x8000; // T3 ON, Div = 1
}

void setSpeedM1(uint8_t speed){ 
    OC1RS = PER_PWM_M1 * speed/100;
}
void setSpeedM2(uint8_t speed){
    OC2RS = PER_PWM_M2 * speed/100;
}

void apagarMotores(void){
    OC1RS = 0;
    OC2RS = 0;
}

void setMov(uint8_t mov){ 
    if (!getObstaculoDetectado()){
        switch(mov){
            case 'F':
                LATCSET = (1<< PIN_DIR_M1)|(1<< PIN_DIR_M2);
                setSpeedM1(100); //Maxima velocidad
                setSpeedM2(95); //Maxima velocidad
                break;
            case 'B':
                LATCCLR = (1<< PIN_DIR_M1)|(1 << PIN_DIR_M2);
                setSpeedM1(100); //Maxima velocidad
                setSpeedM2(95); //Maxima velocidad
                break;
            case 'I': //Forward Right
                LATCSET = (1<< PIN_DIR_M1);
                setSpeedM1(100); 
                setSpeedM2(70); 
                break;
            case 'G': //Forward Left
                LATCSET = (1<< PIN_DIR_M2);
                setSpeedM1(70);
                setSpeedM2(95); 
                break;
            case 'R': //Right
                LATCSET = (1<< PIN_DIR_M1);
                setSpeedM1(100); 
                setSpeedM2(0); 
                break;
            case 'L': //Left
                LATCSET = (1<< PIN_DIR_M2);
                setSpeedM1(0); 
                setSpeedM2(95); 
                break;
            case 'J': //Back Right
                LATCCLR = (1<< PIN_DIR_M2);
                setSpeedM1(70); 
                setSpeedM2(95); 
                break;
            case 'H': //Back Left
                LATCSET = (1<< PIN_DIR_M1);
                setSpeedM1(100); 
                setSpeedM2(70); 
                break;
            default: //Apagar    
                apagarMotores();
                break;
        }
    } else {
        switch(mov){
            case 'B':
                LATCCLR = (1<< PIN_DIR_M1)|(1 << PIN_DIR_M2);
                setSpeedM1(100); //Maxima velocidad
                setSpeedM2(95); //Maxima velocidad
                break;
            case 'J': //Back Right
                LATCCLR = (1<< PIN_DIR_M2);
                setSpeedM1(70); 
                setSpeedM2(90); 
                break;
            case 'H': //Back Left
                LATCSET = (1<< PIN_DIR_M1);
                setSpeedM1(100); 
                setSpeedM2(70); 
                break;
            default: //Apagar    
                apagarMotores();
                break;
                
        }
    }
}

