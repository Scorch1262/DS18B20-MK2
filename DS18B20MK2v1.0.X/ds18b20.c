#define FCY 16000000UL

#include "xc.h"
#include <assert.h>
#include <stdbool.h>
#include <libpic30.h>
#include <stdint.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/i2c1.h"
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

#define true            1
#define false           0
#define Input           1
#define Output          0
#define ReadROM         0x33
#define MatchROM        0x55
#define SkipROM         0xCC
#define SearchROM       0xF0
#define AlarmSearch     0xEC
#define ConvertT        0x44
#define ReadScratchpad  0xBE
#define WriteScratchpad 0x4E
#define CopyScratchpad  0x48
#define RecallEE        0xB8
#define ReadPowerSupply 0xB4
#define DevicesNumber   2

static unsigned char DeviceAddressMap[DevicesNumber][8];
unsigned char SearchROMDevicesSucess;

void DS18B20_init(void){                                                // "DS18B20_init"
    SearchROMDevicesSucess = searchOneWireDevicesROM();                 // 
}                                                                       // 

float DS18B20_temp_GetValue(void){                                      // "DS18B20_temp_GetValue"  
    float           tempDataAnz;                                        // definiert "tempDataAnz"
    uint8_t         prisutnost = 0;                                     // definiert "prisutnost" 
    uint8_t         templ1,temph1;                                      // definiert "templ1", "temph1"
        
    prisutnost = resetOneWirePin();                                     // 
    if(prisutnost == 0){ //prisutan device                              // 
        addressOneWireDevice(DeviceAddressMap[0]);                      // 
        writeOneWireByte(ConvertT);                                     // 
        __delay_ms(750);                                                // 
        resetOneWirePin();                                              // 
        addressOneWireDevice(DeviceAddressMap[0]);                      // 
        writeOneWireByte(ReadScratchpad);                               // ruft "writeOneWireByte" mit "ReadScratchpad" auf
        templ1 = readOneWireByte();                                     // schreibt "readOneWireByte" in "templ1" 
        temph1 = readOneWireByte();                                     // schreibt "readOneWireByte" in "temph1" 
        tempDataAnz = (float)(templ1 + (temph1 * 256)) / 16;            // 
    }                                                                   //    
    return tempDataAnz;                                                 // gibt "tempDataAnz" zurück     
}                                                                       // 

void driveOneWireLow(void){                                             // "driveOneWireLow"
    Sensor_1_SetDigitalOutput();                                        // setzt "Sensor_1" auf OUTPUT
    Sensor_1_SetLow();                                                  // setzt "Sensor_1" auf 0
}                                                                       // 

void driveOneWireHigh(void){                                            // "driveOneWireHigh"
    Sensor_1_SetDigitalOutput();                                        // setzt "Sensor_1" auf OUTPUT
    Sensor_1_SetHigh();                                                 // setzt "Sensor_1" auf 1
}                                                                       // 

unsigned char sampleOneWirePin(void){                                   // "sampleOneWirePin"
    unsigned char result = 0;                                           // definiert "result"
    
    Sensor_1_SetDigitalInput();                                         // setzt "Sensor_1" auf INPUT
    result = Sensor_1_GetValue();                                       // schreibt "Sensor_1" in "result" 
    return result;                                                      // gibt "result" zurück
}                                                                       // 

unsigned char readOneWireBit(void){                                     // "readOneWireBit"
    unsigned char result = 0x00;                                        // definiert "result" 

    driveOneWireLow();                                                  // ruft "driveOneWireLow" auf
    __delay_us(6);                                                      // warte 6us
    driveOneWireHigh();                                                 // ruft "driveOneWireHigh" auf
    __delay_us(9);                                                      // warte 9us
    result = sampleOneWirePin();                                        // schreibt wert von "sampleOneWirePin" in "result"
    __delay_us(55);                                                     // warte 55us
    return result;                                                      // gibt "result" zurück
}                                                                       // 

void writeOneWireBit(unsigned char WriteArg){                           // "writeOneWireBit"
    if(WriteArg == 1){                                                  // wenn "WriteArg" gleich "1" ist
        driveOneWireLow();                                              // ruft "driveOneWireLow" auf
        __delay_us(6);                                                  // warte 6us
        driveOneWireHigh();                                             // ruft "driveOneWireHigh" auf
        __delay_us(64);                                                 // warte 64us
    }else{                                                              // sonst
        driveOneWireLow();                                              // ruft "driveOneWireLow" auf
        __delay_us(60);                                                 // warte 60us
        driveOneWireHigh();                                             // ruft "driveOneWireHigh" auf
        __delay_us(10);                                                 // warte 10us
    }                                                                   // 
}                                                                       // 

unsigned char resetOneWirePin(void){                                    // "resetOneWirePin"
    unsigned char presence;                                             // definiert "presence" 
    
    driveOneWireLow();                                                  // ruft "driveOneWireLow" auf
    __delay_us(240);                                                    // warte 240us
    __delay_us(240);                                                    // warte 240us
    driveOneWireHigh();                                                 // ruft "driveOneWireHigh" auf
    __delay_us(70);                                                     // warte 70us
    presence = sampleOneWirePin();                                      // schreibt wert von "sampleOneWirePin" in "result"
    __delay_us(205);                                                    // warte 205us
    __delay_us(205);                                                    // warte 205us
    driveOneWireHigh();                                                 // ruft "driveOneWireHigh" auf
    return presence; //zero if present; one if not present              // gibt "presence" zurück
}                                                                       // 

void writeOneWireByte(unsigned char data){                              // "writeOneWireByte"
    int i;                                                              // definiert "i"
    
    for(i = 0; i < 8; i++){                                             // 
        writeOneWireBit(data & 0x01);                                   // 
        data >>= 1;                                                     // 
    }                                                                   // 
}                                                                       // 

unsigned char readOneWireByte(void){                                    // "readOneWireByte"
    unsigned char data = 0;                                             // definiert "data"
    int i;                                                              // definiert "i"
    
    for(i = 0; i < 8; i++){                                             // 
        data >>= 1;                                                     // 
        if(readOneWireBit()){                                           // 
        data |= 0x80;                                                   // 
        }                                                               // 
    }                                                                   // 
    return data;                                                        // gibt "data" zurück
}                                                                       // 

unsigned char addressOneWireDevice(unsigned char CurrentAdressedOneWireDevice[]){ // "addressOneWireDevice"
    int i;                                                              // definiert "i"
    
    if(resetOneWirePin() == 0){                                         // 
        writeOneWireByte(MatchROM);                                     // 
        for(i = 0; i < 8; i++){                                         // 
            writeOneWireByte(CurrentAdressedOneWireDevice[i]);          // 
        }                                                               // 
        return 1; //sucessful                                           // gibt "1" zurück
    }else{                                                              // sonst
        return 0;//non sucesfull                                        // gibt "0" zurück
    }                                                                   // 
}                                                                       // 

unsigned char searchOneWireDevicesROM(void){                            // "searchOneWireDevicesROM"
    unsigned char SearchedROMDevice[8];                                 // definiert "SearchedROMDevice"
    unsigned char LastConflictBitNum = 0;                               // definiert "LastConflictBitNum"
    int i,j,k;                                                            // definiert "i", "j", "k"
    
    for(i = 0; i < DevicesNumber; i++){                                 //
        unsigned char mask = 0x01;                                      // definiert "mask"
        for(k = 0; k < 8; k++){                                         // 
            SearchedROMDevice[k] = 0x00;                                // 
        }                                                               // 
        if(resetOneWirePin()){                                          // 
            return 0;                                                   // gibt "0" zurück
        }                                                               // 
        writeOneWireByte(SearchROM);                                    // 
        int bitnum = 1;                                                 // definiert "bitnum"
        unsigned char CurrentConflictBitPos = 0;                        //
        while(bitnum < 65){                                             //
            unsigned char SelectedBit = 0;                              //
            unsigned char InputBitStatus = 0x00;                        //
            if(readOneWireBit() == 1){                                  //
                InputBitStatus |= 2;                                    //
            }                                                           // 
            if(readOneWireBit() == 1){                                  //
                InputBitStatus |= 1;                                    //
            }                                                           // 
            if(InputBitStatus == 3){                                    //
                return 0;                                               // gibt "0" zurück
            }                                                           // 
            if(InputBitStatus > 0){                                     //
                SelectedBit = InputBitStatus >> 1;                      //
            }else{                                                      // sonst
                if(bitnum == LastConflictBitNum){                       //
                    SelectedBit = 1;                                    //
                }else{                                                  // sonst
                    SelectedBit = 0;                                    //
                    CurrentConflictBitPos = bitnum;                     //
                }                                                       // 
            }                                                           // 
            writeOneWireBit(SelectedBit);                               //
            mask = 0x01;                                                //
            mask <<= (bitnum - 1) %8;                                   //
            if(SelectedBit == 1){                                       //
                SearchedROMDevice[(bitnum-1)/8] |= mask;                //
            }                                                           // 
            bitnum++;                                                   //
        }                                                               // 
        LastConflictBitNum = CurrentConflictBitPos;                     //
        for(j = 0; j < 8; j++){                                     //
            DeviceAddressMap[i][j] = SearchedROMDevice[j];              //
        }                                                               // 
    }                                                                   // 
    return 1;                                                           // gibt "1" zurück
}                                                                       // 

