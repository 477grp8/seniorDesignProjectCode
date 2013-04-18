/* 
 * File:   LCD.h
 * Author: team8
 *
 * Created on March 10, 2013, 3:06 PM
 */

#ifndef LCD_H
#define	LCD_H

#include <p32xxxx.h>
#include <plib.h>
#include "MISCELLANEOUS.h"

void printToLCD(char* str);

/*
 * @author - Vineeth
 *
 * @params - int LCDByte -- byte to be sent
 *         - int dataOrInstruction -- 0 if instruction, 1 if data
 *         - int RW -- 0 if write, 1 if read
 *
 * Used to send a specific data byte to the LCD to display
 */
void sendByteToLCD(char LCDByte, int dataOrInstruction, int RW) {

    PORTSetPinsDigitalOut(IOPORT_E, BIT_4); //RS
    PORTSetPinsDigitalOut(IOPORT_E, BIT_3); //R/W
    PORTSetPinsDigitalOut(IOPORT_E, BIT_2); //E
    PORTSetPinsDigitalOut(IOPORT_G, BIT_13); //DB0
    PORTSetPinsDigitalOut(IOPORT_G, BIT_12); //DB1
    PORTSetPinsDigitalOut(IOPORT_G, BIT_14); //DB2
    PORTSetPinsDigitalOut(IOPORT_E, BIT_1); //DB3
    PORTSetPinsDigitalOut(IOPORT_E, BIT_0); //DB4
    PORTSetPinsDigitalOut(IOPORT_A, BIT_7); //DB5
    PORTSetPinsDigitalOut(IOPORT_A, BIT_6); //DB6
    PORTSetPinsDigitalOut(IOPORT_G, BIT_0); //DB7


    (dataOrInstruction == 0) ? PORTClearBits(IOPORT_E, BIT_4) : PORTSetBits(IOPORT_E, BIT_4);
    (RW == 0) ? PORTClearBits(IOPORT_E, BIT_3) : PORTSetBits(IOPORT_E, BIT_3);
    ((LCDByte & 0x01) == 0x01) ? PORTSetBits(IOPORT_G, BIT_13) : PORTClearBits(IOPORT_G, BIT_13);
    (((LCDByte >> 1) & 0x01) == 0x01) ? PORTSetBits(IOPORT_G, BIT_12) : PORTClearBits(IOPORT_G, BIT_12);
    (((LCDByte >> 2) & 0x01) == 0x01) ? PORTSetBits(IOPORT_G, BIT_14) : PORTClearBits(IOPORT_G, BIT_14);
    (((LCDByte >> 3) & 0x01) == 0x01) ? PORTSetBits(IOPORT_E, BIT_1) : PORTClearBits(IOPORT_E, BIT_1);
    (((LCDByte >> 4) & 0x01) == 0x01) ? PORTSetBits(IOPORT_E, BIT_0) : PORTClearBits(IOPORT_E, BIT_0);
    (((LCDByte >> 5) & 0x01) == 0x01) ? PORTSetBits(IOPORT_A, BIT_7) : PORTClearBits(IOPORT_A, BIT_7);
    (((LCDByte >> 6) & 0x01) == 0x01) ? PORTSetBits(IOPORT_A, BIT_6) : PORTClearBits(IOPORT_A, BIT_6);
    (((LCDByte >> 7) & 0x01) == 0x01) ? PORTSetBits(IOPORT_G, BIT_0) : PORTClearBits(IOPORT_G, BIT_0);

    PORTSetBits(IOPORT_E, BIT_2);
    delay(1);
    PORTClearBits(IOPORT_E, BIT_2);

}

/*
 * @author - Vineeth & Fabian
 *
 * @params - void
 *
 * Intializes the LCD by sending the various initializations
 */
void initializeLCD() {
    /*
     * LCD PIN # || Symbol || PIC32MX795F512H pin assignment
     *     1          Vss                GND
     *     2          Vdd                3.3V
     *     3          Vo                 0.1V(contrast)
     *     4          RS                 100
     *     5          R/W                99
     *     6          E                  98
     *     7          DB0                97
     *     8          DB1                96
     *     9          DB2                95
     *     10         DB3                94
     *     11         DB4                93
     *     12         DB5                92
     *     13         DB6                91
     *     14         DB7                90
     *     15         LED+               3.3V(Backlight Power)
     *     16         LED-               GND (Backlight GND)
     */

    PORTSetPinsDigitalOut(IOPORT_E, BIT_4); //RS
    PORTSetPinsDigitalOut(IOPORT_E, BIT_3); //R/W
    PORTSetPinsDigitalOut(IOPORT_E, BIT_2); //E
    PORTSetPinsDigitalOut(IOPORT_G, BIT_13); //DB0
    PORTSetPinsDigitalOut(IOPORT_G, BIT_12); //DB1
    PORTSetPinsDigitalOut(IOPORT_G, BIT_14); //DB2
    PORTSetPinsDigitalOut(IOPORT_E, BIT_1); //DB3
    PORTSetPinsDigitalOut(IOPORT_E, BIT_0); //DB4
    PORTSetPinsDigitalOut(IOPORT_A, BIT_7); //DB5
    PORTSetPinsDigitalOut(IOPORT_A, BIT_6); //DB6
    PORTSetPinsDigitalOut(IOPORT_G, BIT_0); //DB7

    PORTClearBits(IOPORT_E, BIT_2); // E = 0
    delay(100);
    sendByteToLCD(0x30, 0, 0); // Wake up
    delay(30);
    sendByteToLCD(0x30, 0, 0); // WAKE UP!
    delay(10);
    sendByteToLCD(0x30, 0, 0); // I WILL BEAT YOU TO DEATH IF YOU DONT WAKE UP NOW
    delay(10);
    sendByteToLCD(0x38, 0, 0); // 8-bit and 2 line
    sendByteToLCD(0x10, 0, 0); // set cursor
    sendByteToLCD(0x0c, 0, 0); // display on; cursor on; blinking cursor on
    sendByteToLCD(0x06, 0, 0); // entry mode on
    sendByteToLCD(0x01, 0, 0); // clear the display

    printToLCD(" LCD Initialization       Complete");
}



/*
 * @author - Vineeth
 *
 * @params - char charToLCD -- character to be displayed
 *
 * Used for writing a character onto the LCD
 */
void displayCharacterOnLCD(char charToLCD) {
    sendByteToLCD(charToLCD, 1, 0);
}




/*
 * @author - Fabian
 *
 * @params - char* str -- string to be displayed
 *
 * Used for writing string onto the LCD
 */
void printToLCD(char* str){
    char* x = str;
    while (*x != '\0'){ //loop through the string and display it per character
        displayCharacterOnLCD(*x);
        x++; //increment pointer
    }
}

/*
 * @author - Vineeth
 *
 * @params -    stringToBePrinted => Optional string that needs to be printed on the LCD
 *         -    valueToBePrinted => Is the integer value that needs to be converted
 *                              into a string and printed out on the LCD
 *
 * This function accepts a integer value, and does the necessary conversions
 * required to send the value out to the LCD
 */
void convertAndPrintIntegerToStringToLCD(char * stringToBePrinted, int valueToBePrinted) {
    int temp = valueToBePrinted;
    int lengthOfInteger = 0;

    //WriteString(stringToBePrinted);
    printToLCD(stringToBePrinted);
    /*
     *  Loop to count number of digits in the integer to be printed.
     */
    while(temp != 0) {
        temp = temp / 10;
        lengthOfInteger++;
    }

    int modDivideValue = 0;
    int digitToBePrinted = 0;

    /*
     *  Loop to actually start printing out each digit in the integer from left
     *  to right.
     */
    while(lengthOfInteger != 0) {
        modDivideValue = pow(10, --lengthOfInteger);
        digitToBePrinted = valueToBePrinted / modDivideValue;
        valueToBePrinted = valueToBePrinted % modDivideValue;

        switch(digitToBePrinted) {
            case 0 : printToLCD("0");
                     break;
            case 1 : printToLCD("1");
                     break;
            case 2 : printToLCD("2");
                     break;
            case 3 : printToLCD("3");
                     break;
            case 4 : printToLCD("4");
                     break;
            case 5 : printToLCD("5");
                     break;
            case 6 : printToLCD("6");
                     break;
            case 7 : printToLCD("7");
                     break;
            case 8 : printToLCD("8");
                     break;
            case 9 : printToLCD("9");
                     break;
            default : printToLCD("");
                     break;
        }
    }
    printToLCD(" ");
}

#endif	/* LCD_H */

