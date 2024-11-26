/* 
 * File:   moduloUART.c
 * Author: mtorr
 *
 * Created on 13 de marzo de 2023, 18:25
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include "Pic32Ini.h"
#include "UART1.h"
#include <math.h>

#define VECTOR_UART_1 32
#define PIN_U1RX 13
#define PIN_U1TX 7
#define TAM_COLA 100

static char cola_rx[TAM_COLA]; // cola de recepción
static int icab_rx=0; // índice para añadir
static int icol_rx=0; // índice para leer
static char cola_tx[TAM_COLA]; // cola de transmisión
static int icab_tx=0; // índice para añadir
static int icol_tx=0; // índice para leer

void inicializarUART1(int baudios){
        
    ANSELB &= ~(1 << PIN_U1RX);
    ANSELB &= ~(1 << PIN_U1TX);
    
    TRISB |= (1 << PIN_U1TX) | (1 << PIN_U1RX);
    TRISB &= ~(1 << PIN_U1TX);
    
    LATB |= (1 << PIN_U1TX);
    
    U1MODE = 0x0000;
        
    if (baudios > 57600){
        U1MODEbits.BRGH = 1;
        U1BRG = round((5.E6)/(4*baudios)-1);
    } else {
        U1BRG = round((5.E6)/(16*baudios)-1);
    }
    
    IFS1bits.U1RXIF = 0; // Borro flag por si acaso
    IEC1bits.U1RXIE = 1; // Habilito interrupciones
    IFS1bits.U1TXIF = 0; // Borro flag del transmisor por si acaso
    IPC8bits.U1IP = 3; // Prioridad 3
    IPC8bits.U1IS = 1; // Subprioridad 1
    
    SYSKEY=0xAA996655; // Se desbloquean los registros
    SYSKEY=0x556699AA; // de configuración.
    U1RXR = 3; // U1RX conectado a RB13.
    RPB7R = 1; // U1TX conectado a RB7.
    SYSKEY=0x1CA11CA1; // Se vuelven a bloquear.
    
    U1STAbits.URXISEL = 0; // Interrupción cuando llegue 1 char.
    U1STAbits.UTXISEL = 2; // Interrupción cuando FIFO vacía.
    U1STAbits.URXEN = 1; // Se habilita el receptor.
    U1STAbits.UTXEN = 1; // Y el transmisor
    
    U1MODEbits.ON = 1;
}

__attribute__((vector(VECTOR_UART_1), interrupt(IPL3SOFT),
nomips16))
void InterrupcionUART1(void) {
    if(IFS1bits.U1RXIF == 1){ // Ha interrumpido el receptor
        if( (icab_rx+1 == icol_rx) ||
            (icab_rx+1 == TAM_COLA && icol_rx == 0)){
            // La cola está llena
        }else{
            cola_rx[icab_rx] = U1RXREG; // Lee carácter de la UART
            icab_rx++;
            if(icab_rx == TAM_COLA){
                icab_rx = 0;
            }
        }
        IFS1bits.U1RXIF = 0; // Y para terminar se borra el flag
    }
    
    if(IFS1bits.U1TXIF == 1){ // Ha interrumpido el transmisor
        // Se extrae un carácter de la cola y se envía
        if(icol_tx != icab_tx){ // Hay datos nuevos
            U1TXREG = cola_tx[icol_tx];
            icol_tx++;
            if(icol_tx == TAM_COLA){
            icol_tx = 0;
            }
        }else{ // Se ha vaciado la cola.
            IEC1bits.U1TXIE = 0; // Para evitar bucle sin fin
        }
        IFS1bits.U1TXIF = 0; // Y para terminar se borra el flag
    }
}


char getcUART(void) {
    char c;
    if(icol_rx != icab_rx){ // Hay datos nuevos
        c = cola_rx[icol_rx];
        icol_rx++;
        if(icol_rx == TAM_COLA){
            icol_rx=0;
        }
    }else{ // no ha llegado nada
        c = '\0';
    }
    return c;
}

void putsUART(char *ps){
    if ((ps == NULL) || (*ps == 0)){
        return;
    }
    
    while(*ps != '\0'){
        if( (icab_tx+1 == icol_tx) ||
        (icab_tx+1 == TAM_COLA && icol_tx == 0)){
            // La cola está llena. Se aborta el envío de
            // los caracteres que restan
            break;
        }else{
            cola_tx[icab_tx] = *ps; // Copia el carácter en la cola
            ps++; // Apunto al siguiente carácter de la cadena
            icab_tx++;
            if(icab_tx == TAM_COLA){
                icab_tx = 0;
            }
        }
    }
    // Se habilitan las interrupciones del transmisor para
    // comenzar a enviar
    IEC1bits.U1TXIE = 1;
}



