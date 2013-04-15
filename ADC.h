/* 
 * File:   ADC.h
 * Author: team8
 *
 * Created on March 10, 2013, 2:55 PM
 */

/*
 Assume that the diode is connected with the 
 
 * Anode on pin RB4/AN4 along with RB2
 * Cathode on pin RB5/AN5 along with RB3
 
 These two extra pins (RB2 and RB3) act as the GPIO pins that reverse bias the diode.  They
 need to be turned into input mode once they charge up the circuit so that they
 don't mess up the analog readings.
*/


#ifndef ADC_H
#define	ADC_H

#include <p32xxxx.h>
#include <plib.h>
#include "UART.h"
#include "TIMER.h"
#include "MISCELLANEOUS.h"

#define LED_DISCHARGE_TIME 5  // (3)*10ms TIMER1 = 30ms

unsigned int adcSampledInputChannel4 = 12345; // ADC sampled input for channel 4 should be stored here
unsigned int adcSampledInputChannel5 = 12345; // ADC sampled input for channel 5 should be stored here
unsigned int adcSampledInputChannel5After30ms = 0; // ADC sampled input for channel 5 should be stored here

unsigned int PREV_adcSampledInputChannel4 = 0; // ADC sampled input for channel 4 should be stored here
unsigned int PREV_adcSampledInputChannel5 = 0; // ADC sampled input for channel 5 should be stored here

unsigned int MIN_LIGHT_THRESHOLD = 250;
unsigned int MIN_SHADOW_DETECTED_THRESHOLD = 200;
void SampleLexmarkLEDVoltage();
void TurnOffLexmarkLED();
void TurnOnLexmarkLED();
/*
 * Initializes the ADC config
 */
void initializeADC(void) {
    CloseADC10(); // ensure ADC is off before adjusting settings

    PORTSetPinsAnalogIn( IOPORT_B, BIT_5 | BIT_3); // AtD inputs for sampling LED
    //ConfigIntADC10( ADC_INT_DISABLE ); // disable AtD interrupts

    // all these PARAMS are defined in adc10.h
    #define ADC_CONFIG1 ADC_MODULE_ON | ADC_FORMAT_INTG16 | ADC_CLK_MANUAL | ADC_AUTO_SAMPLING_ON | ADC_SAMP_OFF
    #define ADC_CONFIG2 ADC_VREF_EXT_EXT | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_2 | ADC_ALT_BUF_ON | ADC_ALT_INPUT_ON
    #define ADC_CONFIG3 ADC_CONV_CLK_INTERNAL_RC | ADC_SAMPLE_TIME_15
    #define ADC_CONFIG4 ENABLE_AN3_ANA | ENABLE_AN5_ANA
    #define ADC_CONFIG5 SKIP_SCAN_ALL

    SetChanADC10(ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN3 | ADC_CH0_NEG_SAMPLEB_NVREF | ADC_CH0_POS_SAMPLEB_AN5 );
    OpenADC10( ADC_CONFIG1, ADC_CONFIG2, ADC_CONFIG3, ADC_CONFIG4, ADC_CONFIG5 );
    EnableADC10();
}

/*
 * @author - Vineeth
 *
 * @params - void
 *
 * sample the analog voltage on both the channels
 */
void SampleLexmarkLEDVoltage() {
    PREV_adcSampledInputChannel4 = adcSampledInputChannel4;
    PREV_adcSampledInputChannel5 = adcSampledInputChannel5;

    ConvertADC10(); // start a conversion
     while( BusyADC10() ) {} // while busy, wait
  
     unsigned int offset;
     offset = 8 * ((~ReadActiveBufferADC10() & 0x01));

     
     adcSampledInputChannel4 = ReadADC10( offset ); // this is RB3
     adcSampledInputChannel5 = ReadADC10( offset + 1); // this is RB5
     
}

/*
 * @author - Vineeth
 *
 * @params - void
 *
 * returns the sampled value on AN4 channel
 */
int getChannel4Value() {
    return adcSampledInputChannel4;
}

/*
 * @author - Vineeth
 *
 * @params - void
 *
 * returns the sampled value on AN5 channel
 */
int getChannel5Value() {
    return adcSampledInputChannel5;
}

/*
 * @author - Vineeth & Sriram
 *
 * @params - void
 *
 * Turns off the RB3 pin to turn off the LED
 */
void TurnOffLexmarkLED() {
   //PORTSetPinsAnalogIn( IOPORT_A, BIT_3);
     mPORTASetPinsDigitalIn( BIT_3 );    //Ensures the pin is in GPIO mode
    mPORTAClearBits(BIT_3);              //Turn off the bit
}

/*
 * @author - Vineeth & Sriram
 *
 * @params - void
 *
 * Turns on the RB3 pin to turn on the LED
 */
void TurnOnLexmarkLED() {
    mPORTASetPinsDigitalOut( BIT_3 );   
    mPORTASetBits(BIT_3);
}

/*
 * @author - Sriram
 *
 * @params - void
 *
 * returns the amount of discharge that has happened on channel 4 (aka
 */
int getPreviousCharge4 (){
    return (PREV_adcSampledInputChannel4);
}

/*
 * @author - Vineeth
 *
 * @params - void
 *
 * Draws the current ADC sampled light level as a bar on UART, for visual purposes
 */
void drawLightDetectedBar() {
    int NUMBER_OF_BARS = 20;
    int currentLightLevelBars = getChannel5Value() / (1023 / NUMBER_OF_BARS);
    //convertAndPrintIntegerToString(" bars => ", currentLightLevelBars);
    int i;
    WriteString(" [");
    for(i = 0; i < NUMBER_OF_BARS; i++) {
        if(i < currentLightLevelBars) {
            WriteString("#");
        }
        else {
            WriteString(" ");
        }
    }
    WriteString("]");
}


/*
 * @author - Vineeth
 *
 * @params - void
 *
 * Print light level based on current sampled value
 */
void printLightLevel() {
    if(getChannel5Value() < MIN_LIGHT_THRESHOLD) {
            WriteString(" Light Level : LOW ");
    }
    else if(getChannel5Value() < (2*MIN_LIGHT_THRESHOLD)) {
            WriteString(" Light Level : MEDIUM ");
    }
    else {
            WriteString(" Light Level : HIGH ");
    }

}

/*
 * @author - Vineeth
 *
 * @params - void
 *
 * Detect shadow based on current sampled value
 */
void printShadowDetect() {
    if(getChannel5Value() < MIN_SHADOW_DETECTED_THRESHOLD) {
            WriteString(" Shadow - DETECTED ");
    }
    else {
            WriteString(" Shadow - NOT DETECTED ");
    }

}

/*
 * @author - Vineeth
 *
 * @params - void
 *
 * Power Relay Control based on light levels
 */
void controlPowerRelay() {
    mPORTESetPinsDigitalOut( BIT_7 );
    mPORTDSetPinsDigitalOut( BIT_11 );
    if(getChannel5Value() < MIN_LIGHT_THRESHOLD) {
        mPORTEClearBits(BIT_7 );
        mPORTDSetBits(BIT_11 );
        if((curr_state == READY) || (curr_state == SLEEP)) {
            curr_state = HIBERNATE;
        }
    }
    else {
        mPORTESetBits(BIT_7 );
        mPORTDClearBits(BIT_11 );
        if(curr_state == HIBERNATE) {
            curr_state = READY;
            timeElapsed = 0;
            transmitDataFromSDCard = 1;
        }
    }
}
#endif	/* ADC_H */

