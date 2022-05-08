/* 
 * File:   main_postlab10.c
 * Author: Jose Pablo Petion
 *
 * Created on May 6, 2022, 11:54 PM
 */

#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>  // Para poder usar los int de 8 bits

#define _XTAL_FREQ 1000000
uint8_t indice = 0; 
uint8_t respuesta;
uint8_t poten;
void text(char txt[]);
void setup(void);

void __interrupt() isr (void){
    if(ADCON0bits.CHS == 0){            // Verificamos sea AN0 el canal seleccionado
            poten = ADRESH;
        PIR1bits.ADIF = 0;  }
    if(PIR1bits.RCIF){          // Hay datos recibidos?
        respuesta= RCREG; // Mostramos valor recibido en el PORTD
    }
    return;
}

void main(void) {
   
    setup();
    while(1){
        while (indice==0){
        TXREG=0X0D;
        text("Desea leer el potenciometro? (si/no).");
        TXREG=0X0D;
        indice++;
        }
        if (respuesta=='n'){
            text("no.");
            indice=0; 
            respuesta=0;}
        if (respuesta=='s'){
            text("si.");
            PORTD=poten; 
            indice=0; 
            respuesta=0;}
        if(ADCON0bits.GO == 0){             // No hay proceso de conversion
       ADCON0bits.GO = 1;}
     }
}

void setup(void){
    ANSEL = 0b1;                // AN0 como entrada analógica
    ANSELH = 0;                 // I/O digitales
    TRISA = 0b1;                // AN0 como entrada
    PORTA = 0;
    TRISD=0X00;
    PORTD=0X00;
    TRISB=0;
    PORTB=0;
    
    OSCCONbits.IRCF = 0b100;    // 1MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    TXSTAbits.SYNC = 0;         // Comunicación ascincrona (full-duplex)
    TXSTAbits.BRGH = 1;         // Baud rate de alta velocidad 
    BAUDCTLbits.BRG16 = 1;      // 16-bits para generar el baud rate
    
    SPBRG = 25;
    SPBRGH = 0;                 // Baud rate ~9600, error -> 0.16%
    
    RCSTAbits.SPEN = 1;         // Habilitamos comunicación
    TXSTAbits.TX9 = 0;          // Utilizamos solo 8 bits
    TXSTAbits.TXEN = 1;         // Habilitamos transmisor
    RCSTAbits.CREN = 1;         // Habilitamos receptor
    
    // Configuración ADC
    ADCON0bits.ADCS = 0b01;     // Fosc/8
    ADCON1bits.VCFG0 = 0;       // VDD
    ADCON1bits.VCFG1 = 0;       // VSS
    ADCON0bits.CHS = 0b0000;    // Seleccionamos el AN0
    ADCON1bits.ADFM = 0;        // Justificado a la izquierda
    ADCON0bits.ADON = 1;        // Habilitamos modulo ADC
    __delay_us(60);   
    
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones de perifericos
    PIE1bits.RCIE = 1;          // Habilitamos Interrupciones de recepción
    PIR1bits.ADIF = 0;          // Limpiamos bandera de ADC
    PIE1bits.ADIE = 1;   
}

void text(char txt[]){
    uint8_t i=0;
        while(txt[i]!='.'){                 // Incrementamos indice para enviar sigiente caracter
            TXREG = txt[i];    // Cargamos caracter a enviar
            i++;
            
            __delay_us(900);
            } 
        return;
    
       
}