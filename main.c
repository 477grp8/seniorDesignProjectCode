#include <p32xxxx.h>
#include <plib.h>
#include <math.h>
#include "ADC.h"
#include "LCD.h"
#include "UART.h"
#include "CONFIG.h"
#include "MISCELLANEOUS.h"
#include "TIMER.h"
#include "SDCARD.h"

/* state machine */

main()
{
    // Disable JTAG (on RA0 and RA1 )
    mJTAGPortEnable( DEBUG_JTAGPORT_OFF );

    // Configure the device for maximum performance but do not change the PBDIV
    // Given the options, this function will change the flash wait states, RAM
    // wait state and enable prefetch cache but will not change the PBDIV.
    // The PBDIV value is already set via the pragma FPBDIV option above..
    SYSTEMConfig(GetSystemClock(), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    initializeUART();
    initializeADC();
    initializeLCD();

    
    /* Initialize SD card */
    setup_SDSPI();
    SD_setStart();
    /* Fill tempBuffer[] with int 0 to 63
     * Write it to the current block.
     * Empty tempBuffer[] to all 0.
     * Read from the current block to make sure that it returns the right value.
     */
    fillTempBuffer();
    testSDReadWrite(tempBuffer);

    curr_read_block = curr_block;
    
    ConfigTimer1(); // Enable Timer1 for second counts
    configureInterrupts();

    // T2CON = 0x8030; // TMR1 on, prescale 1:256 PB

    mPORTASetPinsDigitalOut( LED_MASK ); // LEDs = output
    mPORTDSetPinsDigitalIn( PB_MASK_D ); // PBs on D = input

    curr_state = READY;
    // enable interrupts
    INTEnableInterrupts();
    int i = 0;
    while( 1 )
    {
        if (getPrintToUARTFlag() == 1){
            //mPORTAToggleBits( LED_MASK );
            convertAndPrintIntegerToString("i => ", i++);
            convertAndPrintIntegerToString("timeElapse => ", timeElapsed);
            convertAndPrintIntegerToString(" 5 => ", getChannel5Value());
            printShadowDetect();
            printLightLevel();
            drawLightDetectedBar();
            controlPowerRelay();

            switch(curr_state) {
            case READY : WriteString("State => READY     ");
                        break;
            case SLEEP : WriteString("State => SLEEP    ");
                        break;
            case HIBERNATE : WriteString("State => HIBERNATE");
                        break;
            case BUSY : WriteString("State => BUSY     ");
                        break;
            }
            WriteString("\r");
            
            setPrintToUARTFlag(0);
        }
        if (NEW_BYTE_RECEIVED == 1){
            curr_state = READY;
            NEW_BYTE_RECEIVED = 0;
            //mPORTAToggleBits( LED_MASK );
            char tempArray[] = "g";
            tempArray[0] = characterByteReceived;
            WriteString(tempArray);
            if(curr_state = HIBERNATE) {
                addByteToBuffer(characterByteReceived);
            }
            else {
                PutCharacter(characterByteReceived);
            }
        }
        if(bufferIndex == 512) {
            SDWriteBlock(currBlock);
            currBlock++;
            bufferIndex = 0;
        }
         if((curr_state == READY) && (timeElapsed >= SLEEP_TIMEOUT) && (timeElapsed < HIBERNATE_TIMEOUT)) {
             curr_state = SLEEP;
         }
         else if((curr_state == SLEEP) && (timeElapsed >= HIBERNATE_TIMEOUT)) {
             curr_state = HIBERNATE;
             timeElapsed = 0;
         }
        if (transmitDataFromSDCard == 1) {
            transmitDataFromSDCard = 0;
            forwardDataToPrinter();
        }
    } // main (while) loop

    return 0;

} // main