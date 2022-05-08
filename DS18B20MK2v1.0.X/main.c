#define FCY 16000000UL 
#include <assert.h>
#include <stdbool.h>
#include "mcc_generated_files/system.h"
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
#include <xc.h>
#include <string.h>
#include "ds18b20.h"

#if defined(__dsPIC33E__)
	#include <p33exxxx.h>
#elif defined(__dsPIC33F__)
	#include <p33fxxxx.h>
#elif defined(__dsPIC30F__)
	#include <p30fxxxx.h>
#elif defined(__PIC24E__)
	#include <p24exxxx.h>
#elif defined(__PIC24H__)
	#include <p24hxxxx.h>
#elif defined(__PIC24F__) || defined(__PIC24FK__)
	#include <p24fxxxx.h>
#endif

float temperatur;                                       // definiert "temperatur"

    
/*
                         Main application
 */
int main(void){                                         // "main"
    SYSTEM_Initialize();                                // ruft "SYSTEM_Initialize" auf
    __delay_ms(500);                                    // warte 500ms
    
    DS18B20_init();                                     // ruft "DS18B20_init" auf
    while (1){                                          // Endloschleife
        temperatur = DS18B20_temp_GetValue();           // schreibt ergebnis von "DS18B20_temp_GetValue" in "temperatur"
        printf("Temperatur = %0.2fC\r\n",temperatur);   // schreibt "Temperatur = %0.2fC\r\n" an UART  
        __delay_ms(500);                                // warte 500ms
    }                                                   // 
}                                                       // 
    
