/* 
 * File:   TIMER.h
 * Author: team8
 *
 * Created on March 10, 2013, 3:20 PM
 */

#ifndef TIMER_H
#define	TIMER_H

#include <p32xxxx.h>
#include <plib.h>
#include "LCD.h"
#include "ADC.h"
#include "UART.h"
#include "MISCELLANEOUS.h"
#include "SDCARD.h"

unsigned short int SecondParts;
unsigned short int TenthSecond = 0;
unsigned short int SecondCount = 0;
int timeElapsed = 0;
int timeElapsedLEDSample = 0;
int LED_SAMPLE_COUNT = 1000; // Sample every 10 seconds
int timeElapsedLEDTurnedOff = 0;
int LED_TURN_OFF_COUNT = 1000; // Sample every 10 seconds
int sampleLEDNow = 0;
int printToUARTflag = 0;

#define T1_FREQ 100
#define TENTH_SEC       10 // count var for tenth of a second

// Macro to instruct micro that a 'new second'
#define startNewSecond() SecondParts=T1_FREQ;

// One 'second part' has gone by, decrement the count.
// When the count reaches zero (i.e. - has no parts left),
// the second is old and should be decremented no more.
#define secondPartElapsed() if(SecondParts>0) SecondParts--;

// Macro to test whether second is 'old'
// i.e. - that it has not parts left
#define isOldSecond() ( SecondParts<2 ? 1 : 0 )


// actions to take every second
#define EverySecondDo(x)                if( SecondCount > 0 ) { SecondCount--; } else { SecondCount = T1_FREQ-1; x }

/**************************************************************************************
        Function Prototypes
**************************************************************************************/
void ConfigTimer1( void );

/**************************************************************************************
        Functions  //THIS IS A 10mS Timer set up
**************************************************************************************/
void ConfigTimer1() {
    #define T1_TICK (( GetPeripheralClock() / 256 / T1_FREQ ))
    OpenTimer1( T1_ON | T1_SOURCE_INT | T1_PS_1_256, T1_TICK );
    ConfigIntTimer1( T1_INT_ON | T1_INT_PRIOR_3 );
}

int getPrintToUARTFlag (){
    return printToUARTflag;
}

//This function is used to time when the debug print is made to screen
//Used when debugging the LED light sensing circuitry
void setPrintToUARTFlag (int newValue){
    printToUARTflag = newValue;
}
/**************************************************************************************
        Interrupt handling routines
**************************************************************************************/
// Note: Priority 1 (lowest) to 7 (highest)

/**************************************
                Timer 1
                        Priority 3
**************************************/
void __ISR(_TIMER_1_VECTOR, ipl3) Timer1Handler(void)
{
    // clear the interrupt flag
        INTClearFlag( INT_T1 );
       
        EverySecondDo({
            //SampleLexmarkLEDVoltage();
            setPrintToUARTFlag(1);
         })
         if((timeElapsedLEDSample >= LED_SAMPLE_COUNT) && (sampleLEDNow == 0)) {
             TurnOffLexmarkLED();
             timeElapsedLEDTurnedOff = 0;
             sampleLEDNow = 1;

         }
         if((timeElapsedLEDTurnedOff >= LED_TURN_OFF_COUNT) && (sampleLEDNow == 1)) {
             sampleLEDNow = 0;
             SampleLexmarkLEDVoltage();
             TurnOnLexmarkLED();
             timeElapsedLEDSample = 0;
             timeElapsedLEDTurnedOff = 0;
         }
         timeElapsedLEDSample++;
         timeElapsedLEDTurnedOff++;
       
          timeElapsed++;

    // one second part has elapsed
    secondPartElapsed();

    // Make sure the flag is cleared
    INTClearFlag( INT_T1 );
}

#endif	/* TIMER_H */

