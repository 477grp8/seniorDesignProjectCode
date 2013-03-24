/* 
 * File:   SDCARD.h
 * Author: team8
 *
 * Created on March 10, 2013, 3:31 PM
 */

#ifndef SDCARD_H
#define	SDCARD_H

#include <p32xxxx.h>
#include <plib.h>

/*
 *  PIN Assignment for prototyping at the PIC32MX360F512L
 *  Breakout Board      Micro
 *  CD                  RC14
 *  DO                  RG8
 *  GND                 GND
 *  SCK                 RF6
 *  Vcc                 3.3V
 *  DI                  RG7
 *  CS                  RC13
 */

        // Chip Select Signal
        #define SD_CS               LATCbits.LATC13
        #define SD_CS_TRIS          TRISCbits.TRISC13
        
        // Card detect signal
        #define SD_CD               PORTCbits.RC14
        #define SD_CD_TRIS          TRISCbits.TRISC14




#endif	/* SDCARD_H */

