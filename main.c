#include <p32xxxx.h>
#include <plib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Define system clock

#define Fcy 60000000l
#define u8  unsigned char
#define bool u8

#define u32  unsigned int


//Configuration bits
#pragma config FSRSSEL = PRIORITY_7     // SRS Select (SRS Priority 7)
#pragma config FMIIEN = ON              // Ethernet RMII/MII Enable (MII Enabled)
#pragma config FETHIO = ON              // Ethernet I/O Pin Select (Default Ethernet I/O)
#pragma config FCANIO = ON              // CAN I/O Pin Select (Default CAN I/O)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)

#pragma config FPLLIDIV = DIV_4         // PLL Input Divider (4x Divider)
#pragma config FPLLMUL = MUL_24         // PLL Multiplier (24x Multiplier)

#pragma config UPLLIDIV = DIV_12        // USB PLL Input Divider (12x Divider)
#pragma config UPLLEN = OFF             // USB PLL Enable (Disabled and Bypassed)

#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)

#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)

#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
#pragma config DEBUG = ON               // Background Debugger Enable (Debugger is enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (ICE EMUC1/EMUD1 pins shared with PGC1/PGD1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

bool _millisecond = 0;
u32 _system_time = 0;
u32 _second_time = 0;

// Reset troubleshooting vars
int _cached_RCON = 0;

unsigned int channel4;	// conversion result as read from result buffer
unsigned int channel5;	// conversion result as read from result buffer
unsigned int offset;	// buffer offset to point to the base of the idle buffer

void initializeATD(void)
{
    SYSTEMConfig(Fcy, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    CloseADC10();    // ensure the ADC is off before setting the configuration

    // define setup parameters for OpenADC10
                // Turn module on | ouput in integer | trigger mode auto | enable autosample
    #define PARAM1  ADC_MODULE_ON | ADC_FORMAT_INTG | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON

    // define setup parameters for OpenADC10
                // ADC ref external    | disable offset test    | disable scan mode | perform 2 samples | use dual buffers | use alternate mode
    #define PARAM2  ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_2 | ADC_ALT_BUF_ON | ADC_ALT_INPUT_ON

    // define setup parameters for OpenADC10
    //                   use ADC internal clock | set sample time
    #define PARAM3  ADC_CONV_CLK_INTERNAL_RC | ADC_SAMPLE_TIME_15


    // define setup parameters for OpenADC10
    // do not assign channels to scan
    #define PARAM4    SKIP_SCAN_ALL


    // define setup parameters for OpenADC10
                // set AN4 and AN5 as analog inputs
    #define PARAM5    ENABLE_AN4_ANA | ENABLE_AN5_ANA

        // use ground as neg ref for A | use AN4 for input A      |  use ground as neg ref for A | use AN5 for input B

     // configure to sample AN4 & AN5
    SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN4 |  ADC_CH0_NEG_SAMPLEB_NVREF | ADC_CH0_POS_SAMPLEB_AN5); // configure to sample AN4 & AN5
    OpenADC10( PARAM1, PARAM2, PARAM3, PARAM4, PARAM5 ); // configure ADC using parameter define above

    EnableADC10(); // Enable the ADC
}

void initialize(void)
{
    //Disables JTAG bit
    DDPCONbits.JTAGEN = 0;

      //Enables interrupts
//    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
//    INTEnableInterrupts();
//
//    // Configure the CAN interrupt priority, level 3
//    INTSetVectorPriority( INT_CAN_1_VECTOR, INT_PRIORITY_LEVEL_3);
//
//    // Configure the CAN interrupt sub-priority, level 2
//    INTSetVectorSubPriority( INT_CAN_1_VECTOR, INT_SUB_PRIORITY_LEVEL_2);
//
//    // Configure the Timer interrupt priority, level 2
//    INTSetVectorPriority( INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_2);
//
//    // Configure the Timer interrupt sub-priority, level 2
//    INTSetVectorSubPriority( INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_2);
//
//    OpenTimer1(T1_ON | T1_IDLE_STOP | T1_PS_1_256, .001*Fcy/256); // One millisecond
//    EnableIntT1;
//    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_2 | T1_INT_SUB_PRIOR_2);
//
//    INTClearFlag(INT_CAN1);
//    INTClearFlag(INT_T1);
//
//    INTEnable(INT_CAN1, INT_ENABLED);
//    INTEnable(INT_T1, INT_ENABLED);

      //initCAN(Fcy);
      //Configures UART - Option 1
        UARTConfigure(UART2, UART_ENABLE_PINS_TX_RX_ONLY);
	UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
	UARTSetLineControl(UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
	UARTSetDataRate(UART2, Fcy, 19200);
	UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_TX | UART_RX ));


      //Changes the printf function to send to UART1, rather than UART2
        __C32_UART = 2;

      //Configures UART - Option 2

//    UARTConfigure(UART2, UART_ENABLE_PINS_TX_RX_ONLY);
//    UARTSetFifoMode(UART2, 0);
//    UARTSetLineControl(UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
//    UARTSetDataRate(UART2, Fcy, 19200);
//    UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_TX | UART_RX ));
//
//    Changes the printf function to send to UART1, rather than UART2
//    __C32_UART = 0;

    
//    PORTSetPinsDigitalOut(YELLOW_LED);
//    PORTSetPinsDigitalOut(GREEN_LED);
//
//    //CLEAR OUTPUT PINS
//    PORTClearBits(RED_LED);
//    PORTClearBits(YELLOW_LED);
//    PORTClearBits(GREEN_LED);
//
//    //DIGITAL INPUT PINS
//    PORTSetPinsDigitalIn(CR_UP);
//    PORTSetPinsDigitalIn(CR_DW);
//    PORTSetPinsDigitalIn(CR_CL);
//    PORTSetPinsDigitalIn(HORN);
//    PORTSetPinsDigitalIn(LEFT_T);
//    PORTSetPinsDigitalIn(RIGHT_T);
}

/*
 * @author - Vineeth
 *
 * @params - valueToBePrinted => Is the integer value that needs to be converted
 *                              into a string and printed out on UART
 *
 * This function accepts a integer value, and does the necessary conversions
 * required to send the value out on UART using printf
 */
void convertAndPrintIntegerToString(char * stringToBePrinted, int valueToBePrinted) {
    int temp = valueToBePrinted;
    int lengthOfInteger = 0;

    printf(stringToBePrinted);
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
            case 0 : printf("0");
                     break;
            case 1 : printf("1");
                     break;
            case 2 : printf("2");
                     break;
            case 3 : printf("3");
                     break;
            case 4 : printf("4");
                     break;
            case 5 : printf("5");
                     break;
            case 6 : printf("6");
                     break;
            case 7 : printf("7");
                     break;
            case 8 : printf("8");
                     break;
            case 9 : printf("9");
                     break;
            default : printf("");
                     break;
        }
    }
    printf(" ");
}

int i;

int main(void)
{
    initialize ();
    initializeATD(); // ATD initialization function

    PORTSetPinsDigitalOut(IOPORT_B, BIT_9);
    PORTSetBits(IOPORT_B, BIT_9);
    
    while ( ! mAD1GetIntFlag() ) { } // wait for the first conversion to complete so there will be vaild data in ADC result registers

    while (1)
    {
        offset = 8 * ((~ReadActiveBufferADC10() & 0x01));  // determine which buffer is idle and create an offset

        channel4 = ReadADC10(offset);          // read the result of channel 4 conversion from the idle buffer
        channel5 = ReadADC10(offset + 1);      // read the result of channel 5 conversion from the idle buffer
        
        i = 500000;
        while (i--){};
        if(channel4 > 100) {
            PORTSetBits(IOPORT_B, BIT_9);
            //printf(" Set ");
        }
        else {
            PORTClearBits(IOPORT_B, BIT_9);
            //printf(" Clear ");
        }
        //printf(" loop ");
        if(channel4 != NULL) {
            convertAndPrintIntegerToString("channel4 => ", channel4);
            convertAndPrintIntegerToString("channel5 => ", channel5);
        }
        else {
            //printf("NULL");
        }
        printf("\n");
    }


}