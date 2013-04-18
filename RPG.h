/*
 * File: RPG.h
 * Author: team8
 *
 * Created on April 8, 2013, 10:02 PM
 */
#ifndef RPG_H
#define	RPG_H

#include <p32xxxx.h>
#include <plib.h>
#include "UART.h"
#include "LCD.h"
#include "TIMER.h"
#include "MISCELLANEOUS.h"

unsigned int MSB = 1;
unsigned int prevpb = 1;
unsigned int LSB = 1;
unsigned int encoded = 0b11;
unsigned int sum = 0b1111;
unsigned int lastEncoded = 0b11;
int _turnCount = 0;
unsigned int _moved = 0;
int encoderValue = 1;
int enter = 0;
int clockwise = 0;
int counterclockwise = 0;
int page = 0;
int disp = 0;
int location = 1234;
int count1 = 0;
int count2 = 0;
int count3 = 0;
int count4 = 0;
int hibernateTimeoutIncrement = 10000;
int sleepTimeoutIncrement = 1000;

void initializeRPG() {
    CNPUEbits.CNPUE12 = 1;
    CNPUEbits.CNPUE4 = 1;
    CNPUEbits.CNPUE5 = 1;
    PORTSetPinsDigitalIn(IOPORT_B, BIT_3);
    PORTSetPinsDigitalIn(IOPORT_B, BIT_2);
    PORTSetPinsDigitalIn(IOPORT_B, BIT_15);
    sendByteToLCD(0x01, 0, 0); // clear the display
    printToLCD("                       Hit The Button    Project PRINT Menu                     ");
}

void pushbuttonloop()
{
    if(PORTBbits.RB15 != prevpb){
        if(PORTBbits.RB15 == 1)
            {prevpb = 1;}
        else if(PORTBbits.RB15 == 0){
            //WriteString(" Pushbutton ");
            enter ++;
            page ++;
            if (enter > 4){
                enter = 0;
            }
            if (page > 5){
                page = 0;
            }

            prevpb = 0;}}
}

void updateEncoder()
{
    MSB = PORTBbits.RB2 ; //MSB = most significant bit
    LSB = PORTBbits.RB3 ; //LSB = least significant bit

    encoded = (MSB << 1)|LSB; //converting the 2 pin value to single number
    sum  = (lastEncoded << 2)|encoded; //adding it to the previous encoded value

    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        {_turnCount ++;}

    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        {_turnCount --;}

    lastEncoded = encoded; //store this value for next time

    if (encoded == 0b11){
        if (_turnCount > 0){
            _moved = 1;
            _turnCount = 0;
            encoderValue --;
            clockwise = 1;}

        else if (_turnCount < 0){
            _moved = 1;
            _turnCount = 0;
            encoderValue ++;
            counterclockwise = 1;}

        else{
            _moved = 0;
            _turnCount = 0;}}
}

void LCDMenuControl()
{
        /*convertAndPrintIntegerToString("clockwise => ", clockwise);
        convertAndPrintIntegerToString("enter => ", enter);
        convertAndPrintIntegerToString("page => ", page);
        convertAndPrintIntegerToString("count1 => ", count1);
        convertAndPrintIntegerToString("count2 => ", count2);
        convertAndPrintIntegerToString("count3 => ", count3);
        convertAndPrintIntegerToString("count4 => ", count4);
        convertAndPrintIntegerToString("location => ", location);
         */
        WriteString("\r");
        if ((enter == 1) && (page == 1)){page = 1; location = 0;} //MAIN PAGE

        if (location == 0){
            printCurrStateToLCD();
        }

        if ((enter == 2) && (page == 1) && (count1 == 0)){page = 2; location = 1; count1 = 0; count2 = 0; count3 = 0; count4 = 0; disp = 1;} //PRINTER ON/OFF PAGE
        if ((enter == 2) && (page == 1) && (count1 == 1)){page = 3; location = 2; count1 = 1; count2 = 5; count3 = 0; count4 = 0; disp = 4;} //HIBERNATE TIMEOUT PAGE
        if ((enter == 2) && (page == 1) && (count1 == 2)){page = 4; location = 3; count1 = 2; count2 = 5; count3 = 0; count4 = 0; disp = 4;} //SLEEP TIMEOUT PAGE

        if ((enter == 3) && (page == 1) && (count2 == 2)){page = 1; location = 0; enter = 1; count1 = 0; count2 = 0; count3 = 0; count4 = 0; disp = 1;} // BACK TO MAIN PAGE
        if ((enter == 3) && (page == 1) && (count3 == 1)){page = 1; location = 0; enter = 1; count1 = 0; count2 = 0; count3 = 0; count4 = 0; disp = 1;} // BACK TO MAIN PAGE
        if ((enter == 3) && (page == 1) && (count4 == 1)){page = 1; location = 0; enter = 1; count1 = 0; count2 = 0; count3 = 0; count4 = 0; disp = 1;} // BACK TO MAIN PAGE

        if ((enter == 3) && (page == 1) && (count2 == 0)){page = 2; location = 1; enter = 2; count1 = 0; count2 = 0; count3 = 0; count4 = 0; disp = 1;
            mPORTEClearBits(BIT_11 );
            mPORTDSetBits(BIT_7 );} // POWER ON SELECTION
        if ((enter == 3) && (page == 1) && (count2 == 1)){page = 2; location = 1; enter = 2; count1 = 0; count2 = 0; count3 = 0; count4 = 0; disp = 1;
            mPORTESetBits(BIT_11 );
            mPORTDClearBits(BIT_7 );} // POWER OFF SELECTION

        if ((enter == 3) && (page == 1) && (location == 2)){enter == 3; page = 5; location = 2; count1 = 0; count2 = 0; count3 = 1; count4 = 0; disp = 4;} //HIBERNATE TIMEOUT ADJUST
        if ((enter == 3) && (page == 1) && (location == 3)){enter == 3; page = 6; location = 3; count1 = 0; count2 = 0; count3 = 0; count4 = 1; disp = 4;} //SLEEP TIMEOUT ADJUST
        if ((enter == 4) && (page == 1) && (location == 2)){page = 3; location = 2; count1 = 0; count2 = 5; count3 = 0; count4 = 0; disp = 4; enter = 2;} // HIBERNATE TIMEOUT SET
        if ((enter == 4) && (page == 1) && (location == 3)){page = 4; location = 3; count1 = 0; count2 = 5; count3 = 0; count4 = 0; disp = 4; enter = 2;} // SLEEP TIMEOUT SET

        if ((clockwise == 1) && (enter == 1) && (location == 0)){
            count1 ++;
            //convertAndPrintIntegerToString("Count1 => ", count1);
            //WriteString("and Clockwise ");
            if(count1 > 2){count1 = 0;}
            if(count1 == 0){disp = 1;} // set cursor
            if(count1 == 1){disp = 2;} // set cursor
            if(count1 == 2){disp = 3;} // set cursor
            //convertAndPrintIntegerToString("Disp => ", disp);
            clockwise = 0;}
        if ((counterclockwise == 1) && (enter == 1) && (location == 0)){
            count1 --;
            //convertAndPrintIntegerToString("Count1 => ", count1);
            //WriteString("and Counterclockwise ");
            if(count1 < 0){count1 = 2;}
            if(count1 == 0){disp = 1;} // set cursor
            if(count1 == 1){disp = 2;} // set cursor
            if(count1 == 2){disp = 3;} // set cursor
            //convertAndPrintIntegerToString("Disp => ", disp);
            counterclockwise = 0;}

        if ((clockwise == 1) && (enter == 2) && (location == 1)){
            count2 ++;
            //convertAndPrintIntegerToString("Count2 => ", count2);
            //WriteString("and Clockwise ");
            if(count2 > 2){count2 = 0;}
            if(count2 == 0){disp = 1;} // set cursor
            if(count2 == 1){disp = 2;} // set cursor
            if(count2 == 2){disp = 3;} // set cursor
            //convertAndPrintIntegerToString("Disp => ", disp);
            clockwise = 0;}
        if ((counterclockwise == 1) && (enter == 2) && (location == 1)){
            count2 --;
            //convertAndPrintIntegerToString("Count2 => ", count2);
            //WriteString("and Counterclockwise ");
            if(count2 < 0){count2 = 2;}
            if(count2 == 0){disp = 1;} // set cursor
            if(count2 == 1){disp = 2;} // set cursor
            if(count2 == 2){disp = 3;} // set cursor
            //convertAndPrintIntegerToString("Disp => ", disp);
            counterclockwise = 0;}

        if ((clockwise == 1) && (enter == 2) && (location == 2)){
            count3 ++;
            //convertAndPrintIntegerToString("Count3 => ", count3);
            //WriteString("and Clockwise ");
            if(count3 > 1){count3 = 0;}
            if(count3 == 0){disp = 4;} // set cursor
            if(count3 == 1){disp = 5;} // set cursor
            //convertAndPrintIntegerToString("Disp => ", disp);
            clockwise = 0;}
        if ((counterclockwise == 1) && (enter == 2) && (location == 2)){
            count3 --;
            //convertAndPrintIntegerToString("Count3 => ", count3);
            //WriteString("and Counterclockwise ");
            if(count3 < 0){count3 = 1;}
            if(count3 == 0){disp = 4;} // set cursor
            if(count3 == 1){disp = 5;} // set cursor
            //convertAndPrintIntegerToString("Disp => ", disp);
            counterclockwise = 0;}

        if ((clockwise == 1) && (enter == 2) && (location == 3)){
            count4 ++;
            //convertAndPrintIntegerToString("Count4 => ", count4);
            //WriteString("and Clockwise ");
            if(count4 > 1){count4 = 0;}
            if(count4 == 0){disp = 4;} // set cursor
            if(count4 == 1){disp = 5;} // set cursor
            //convertAndPrintIntegerToString("Disp => ", disp);
            clockwise = 0;}
        if ((counterclockwise == 1) && (enter == 2) && (location == 3)){
            count4 --;
            //convertAndPrintIntegerToString("Count4 => ", count4);
            //WriteString("and Counterclockwise ");
            if(count4 < 0){count4 = 1;}
            if(count4 == 0){disp = 4;} // set cursor
            if(count4 == 1){disp = 5;} // set cursor
            //convertAndPrintIntegerToString("Disp => ", disp);
            counterclockwise = 0;}

         if ((clockwise == 1) && (enter == 3) && (location == 2) && (count3 == 1)){
            HIBERNATE_TIMEOUT = HIBERNATE_TIMEOUT + hibernateTimeoutIncrement;
            convertAndPrintIntegerToString("  Timeout = ", HIBERNATE_TIMEOUT);
            sendByteToLCD(0b10000000, 0, 0);
            convertAndPrintIntegerToStringToLCD("  Timeout = ", HIBERNATE_TIMEOUT);
            clockwise = 0;}
        if ((counterclockwise == 1) && (enter == 3) && (location == 2) && (count3 == 1)){
            HIBERNATE_TIMEOUT = HIBERNATE_TIMEOUT - hibernateTimeoutIncrement;
            convertAndPrintIntegerToString("  Timeout = ", HIBERNATE_TIMEOUT);
            sendByteToLCD(0b10000000, 0, 0);
            convertAndPrintIntegerToStringToLCD("  Timeout = ", HIBERNATE_TIMEOUT);
            counterclockwise = 0;}

        if ((clockwise == 1) && (enter == 3) && (location == 3) && (count4 == 1)){
            SLEEP_TIMEOUT = SLEEP_TIMEOUT + sleepTimeoutIncrement;
            convertAndPrintIntegerToString("  Timeout = ", SLEEP_TIMEOUT);
            sendByteToLCD(0b10000000, 0, 0);
            convertAndPrintIntegerToStringToLCD("  Timeout = ", SLEEP_TIMEOUT);
            clockwise = 0;}
        if ((counterclockwise == 1) && (enter == 3) && (location == 3) && (count4 == 1)){
            SLEEP_TIMEOUT = SLEEP_TIMEOUT - sleepTimeoutIncrement;
            convertAndPrintIntegerToString("  Timeout = ", SLEEP_TIMEOUT);
            sendByteToLCD(0b10000000, 0, 0);
            convertAndPrintIntegerToStringToLCD("  Timeout = ", SLEEP_TIMEOUT);
            counterclockwise = 0;}
        
        switch(page){
            case 1: //MAIN PAGE
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD("> Printer On/Off    ");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD("  Hibernate Timeout ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD("  Sleep Timeout     ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD("  State:            ");
                page = 0;
                break;
            case 2: //PRINTER ON/OFF PAGE
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD("  Power On           ");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD("  Printer Off        ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD("  Back               ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD("                     ");
                page = 0;
                break;
            case 3: //HIBERNATE TIMEOUT PAGE
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD("  Set Timeout        ");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD("  Back               ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD("                     ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD("                     ");
                page = 0;
                break;
            case 4: //SLEEP TIMEOUT PAGE
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD("  Set Timeout        ");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD("  Back               ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD("                     ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD("                     ");
                page = 0;
                break;
            case 5: //SET TIMEOUT PAGE
                sendByteToLCD(0b10000000, 0, 0);
                convertAndPrintIntegerToStringToLCD("  Timeout = ", HIBERNATE_TIMEOUT);
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD("  Back               ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD("                     ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD("                     ");
                page = 0;
                break;
            case 6: //SET TIMEOUT PAGE
                sendByteToLCD(0b10000000, 0, 0);
                convertAndPrintIntegerToStringToLCD("  Timeout = ", SLEEP_TIMEOUT);
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD("  Back               ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD("                     ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD("                     ");
                page = 0;
                break;}

        switch(disp){
            case 1:
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD(">");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD(" ");
                break;
            case 2:
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD(">");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD(" ");
                break;
            case 3:
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD(">");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD(" ");
                break;
            case 4:
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD(">");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD(" ");
                break;
            case 5:
                sendByteToLCD(0b10000000, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b11000000, 0, 0);
                printToLCD(">");
                sendByteToLCD(0b10010100, 0, 0);
                printToLCD(" ");
                sendByteToLCD(0b11010100, 0, 0);
                printToLCD(" ");
                break;}
}

void printCurrStateToLCD() {
    sendByteToLCD(0b11010100, 0, 0);
    switch(curr_state){
                    case READY : printToLCD("  State: READY      ");
                                 break;
                    case SLEEP : printToLCD("  State: SLEEP      ");
                                break;
                    case HIBERNATE : printToLCD("  State: HIBERNATE");
                                break;
                    case BUSY : printToLCD("  State: BUSY       ");
                                break;
                }
}
#endif	/* RPG_H */

