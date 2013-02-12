#include <p32xxxx.h>
#include <plib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "globals.h"
//#include "main.h"
//#include "psrcan/psrcan.h"
//#include "types.h"
//#include "debugmenu.h"
//#include "gen/can_registers_gen.h"

// YOU MAY HAVE HAD TO ADD THE extern "C" line.  Check into that if UART doesn't work

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

//debounce _right_turn = {0};
//debounce _left_turn = {0};
//debounce _cruise_up = {0};
//debounce _cruise_down = {0};
//debounce _cruise_cancel = {0};
//debounce _horn = {0};

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

      //DIGITAL OUTPUT PINS
      PORTSetPinsDigitalOut(IOPORT_B, BIT_9);
    
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

int i;

int main(void)
{
    //PORTSetPinsDigitalOut(IOPORT_B, BIT_9);
    initialize ();
    while (1)
    {
        i = 100000;
        while (i--){};
        PORTToggleBits(IOPORT_B, BIT_9);
        printf("RCON (at reset)");
    }
}

//#include <p32xxxx.h>
//#include <plib.h>
//#include <stdio.h>
//
//#pragma config POSCMOD = XT
//#pragma FNOSC = PRI
//#pragma FPBDIV = DIV_1
//#pragma FWDTEN = OFF
//
//int i;
//
//int main(void)
//{
//        // Explorer16 LEDs are on lower 8-bits of PORTA and to use all LEDs, JTAG port must be disabled.
//        // mJTAGPortEnable(DEBUG_JTAGPORT_OFF);
//
//        PORTSetPinsDigitalOut(IOPORT_B, BIT_9);
//
//
//	//mPORTASetPinsDigitalOut( BIT_7 | BIT_6 | BIT_5 | BIT_5 | BIT_4 | \
//                                                     BIT_3 | BIT_2 | BIT_1 | BIT_0 );
//
//	UARTConfigure(UART2, UART_ENABLE_PINS_TX_RX_ONLY);
//	UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
//	UARTSetLineControl(UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
//	UARTSetDataRate(UART2, 10000000l, 9600);
//	UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_TX | UART_RX ));
//
//        //Changes the printf function to send to UART1, rather than UART2
//        __C32_UART = 2;
//
//	while(1)
//	{
//            i = 10000;
//            while (i--){
//            PORTToggleBits(};
//            PORTToggleBits(IOPORT_B, BIT_9);
//            //mPORTAToggleBits(BIT_7 | BIT_5 | BIT_3 | BIT_1 );
//            //printf("Hello, world!\r\n");
//            printf("Hello, world!");
//            DBPRINTF("Hello World - dbprint!n");
//	}
//}