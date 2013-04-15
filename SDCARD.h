/*
 * File:   SDCARD.h
 * Author: team8
 *
 * Created on March 10, 2013, 3:31 PM
 *
 * This custom modified SDCard Driver is based on the driver by ECE477 Team 1, Spring '07. Retrieved March 15th, 2013 from https://engineering.purdue.edu/ece477/Webs/S07-Grp01/docs/SDro.c
 */

#ifndef SDCARD_H
#define	SDCARD_H

#include <p32xxxx.h>
#include <plib.h>
#include "UART.h"
#include "CONFIG.h"

/*
 *  PIN Assignment for prototyping at the PIC32MX360F512L
 *  Breakout Board      Micro
 *  CD                  RG2
 *  DO                  RF8
 *  GND                 GND
 *  SCK                 RF6
 *  Vcc                 3.3V
 *  DI                  RF7
 *  CS                  RG3
 */

/* Declaration */
void InitSPI(void);
unsigned char InitSD(void);
unsigned char SD_WriteCommand(unsigned char* cmd);
unsigned char SPIRead(void);
void SPIWrite(unsigned char data);
unsigned char printbyte(unsigned char byte);
unsigned char printhexdigit(unsigned char digit);
unsigned char SDReadBlock(unsigned long block);


unsigned char setup_SDSPI(void);
void SD_setStart(void);
unsigned char SD_GetSample(int);
void SD_SetSample(int, unsigned char);
void SD_writeCurr(void);
void SD_readCurr(void);
unsigned char SDReadBlock(unsigned long);
unsigned char SDWriteBlock(unsigned long);




#define OK 0x00
#define PRINT_ERR 0x01
#define INIT_ERR 0x02
#define SD_ERR 0x03

#define MAX_BLOCKS 55000
#define SD_BLOCK_SIZE 64

//internal macros
#define HEX 16
#define READ_CMD 17
#define DUMMY 0xFF
#define START_BLOCK_TOKEN 0xFE
// R1 Response Codes (from SD Card Product Manual v1.9 section 5.2.3.1)
#define R1_IN_IDLE_STATE    (1<<0)   // The card is in idle state and running initializing process.
#define R1_ERASE_RESET      (1<<1)   // An erase sequence was cleared before executing because of an out of erase sequence command was received.
#define R1_ILLEGAL_COMMAND  (1<<2)  // An illegal command code was detected
#define R1_COM_CRC_ERROR    (1<<3)   // The CRC check of the last command failed.
#define R1_ERASE_SEQ_ERROR  (1<<4)  // An error in the sequence of erase commands occured.
#define R1_ADDRESS_ERROR    (1<<5)  // A misaligned address, which did not match the block length was used in the command.
#define R1_PARAMETER        (1<<6)  // The command's argument (e.g. address, block length) was out of the allowed range for this card.

//ports
#define LED PORTAbits.RA3
#define LED_DIR TRISAbits.TRISA3
#define SD_PWR PORTAbits.RA2
#define SD_PWR_DIR TRISAbits.TRISA2
#define SD_CS PORTGbits.RG3
#define SD_CS_DIR TRISGbits.TRISG3
#define SD_CD PORTGbits.RG2
#define SD_CD_DIR TRISGbits.TRISG2
#define SDI PORTFbits.RF7
#define SDI_DIR TRISFbits.TRISF7
#define SCK PORTFbits.RF6
#define SCK_DIR TRISFbits.TRISF6
#define SDO PORTFbits.RF8
#define SDO_DIR TRISFbits.TRISF8
#define SD_PowerOn() SD_PWR = 1
#define SD_PowerOff() SD_PWR = 0
#define SD_Enable() SD_CS = 0
#define SD_Disable() SD_CS = 1

//#define SPIFastClock() SPI1CON = 0x007F

//internal types
typedef unsigned char buffer_typ;
buffer_typ buffer[SD_BLOCK_SIZE * 8];
buffer_typ tempBuffer[SD_BLOCK_SIZE * 8];
int bufferIndex = 0;

unsigned long curr_block;
unsigned long curr_read_block = 0;
unsigned char synced;
unsigned long total_blocks;

/* Function */
int currBlock = 1;

void WriteSamples(unsigned char* buff) {
    int i = 0;
    //put in write queue
    for (i = 0; i < SD_BLOCK_SIZE * 8; i++) {
        buffer[i] = *buff;
        buff++;
    }
    SDWriteBlock(currBlock);
    SDReadBlock(currBlock);
    currBlock++;
    return;
}

unsigned char setup_SDSPI(void) {
    unsigned char res = 0;
    unsigned long start_block = 0xD6D6;

    /* Set up I/O Port */
    //ADPCFG=0xFFFF;          /* Analog ports as Digital I/O */
    TRISB = 0x0000; /* Port B output Data as well as serial port */
    LATB = 0x0000; /* Initial Value of 0 */

    // Configure output pins
    LED_DIR = 0;
    LED = 0;

    total_blocks = 1; //this is necessary!!

    InitSPI();
    res = InitSD();
    do {
        while (res) {
            res = InitSD();
            LED = 0;
        }
        res = SDReadBlock(start_block);
    } while (res);

    LED = 1;

    //char msg[] = "init\n";
    //WriteString(msg);
    /*
      unsigned char curr = 'M';
      int i = 0;
      for(i = 0; i < SD_BLOCK_SIZE; i++)
      {
        buffer1[i] = curr;
        if(curr++ == 'Z') curr = 'A';
      } */

    /* test */
    //SDWriteBlock(0xB0);
    //SDReadBlock(0xB0);
    //PrintDebug(0xB0);
    //PrintBlock();

    curr_block = 1;
    synced = 0;
    /*
     * The TRISB and LATB has to be LOW when accessing the SD card
     * but it has to be high for the ADC to receive the right amount of voltage
     */
    TRISB = 0xFFFF; /* Port B output Data as well as serial port */
    //LATB = 0xFFFF; /* Initial Value of 0 */

 
    return OK;
}

void SD_setStart(void) {
    curr_block = 1;
    return;
}

unsigned char SD_GetSample(int cnt) {
    return buffer[cnt];
}

void SD_SetSample(int cnt, unsigned char sample) {
    buffer[cnt] = sample;
    return;
}

void SD_writeCurr(void) {
    SDWriteBlock(curr_block);
    curr_block++;
    return;
}

void SD_readCurr(void) {
    SDReadBlock(curr_block);
    curr_block++;
    return;
}

unsigned char SDReadBlock(unsigned long block) {
    buffer_typ* theData;
    unsigned char read_cmd[6];
    unsigned char status = 0x0;
    unsigned int offset = 0;
    unsigned char res = 1;


    TRISB = 0x0000; /* Port B output Data as well as serial port */
    //LATB = 0x0000; /* Initial Value of 0 */

    /*
      if(block >= total_blocks)
      {
        //too large for small disc
        return SD_ERR;
      } */

    while (res) {
        res = InitSD();
    }

    //LED = 0;

    //send the read command
    block = block * SD_BLOCK_SIZE * 8; //need to be correct offset
    read_cmd[0] = READ_CMD;
    read_cmd[1] = ((block & 0xFF000000) >> 24);
    read_cmd[2] = ((block & 0x00FF0000) >> 16);
    read_cmd[3] = ((block & 0x0000FF00) >> 8);
    read_cmd[4] = ((block & 0x000000FF));
    read_cmd[5] = DUMMY;
    SD_Enable();
    status = SD_WriteCommand(read_cmd);
    if (status != 0) {
        //printbyte(status);
        return SD_ERR;
    }

    //find the start of the read
    do {
        status = SPIRead();
    } while (status != START_BLOCK_TOKEN);

    //read the bytes
  /*  theData = buffer;
    for (offset = 0; offset < SD_BLOCK_SIZE * 8; offset++) {
        *theData = (512 - offset) % 255;
        //printbyte(*theData);
        theData++;
    }*/
    int i = 0;
    theData = buffer;
    for (offset = 0; offset < SD_BLOCK_SIZE * 8; offset++) {
        while(i < 150){
            i++;
       }
        *theData = SPIRead();
        //printbyte(*theData);
        theData++;
    }
    SD_Disable();

    //pump for eight cycles according to spec
    SPIWrite(0xFF);

        /*
     * The TRISB and LATB has to be LOW when accessing the SD card
     * but it has to be high for the ADC to receive the right amount of voltage
     */
    TRISB = 0xFFFF;
    //LATB = 0xFFFF;
    
    //LED = 0;
    return OK;
}

unsigned char SDWriteBlock(unsigned long block) {
    buffer_typ* theData;
    unsigned int i;
    unsigned char status;
    unsigned char res = 1;

    TRISB = 0x0000; /* Port B output Data as well as serial port */
    //LATB = 0x0000; /* Initial Value of 0 */


    while (res) {
        res = InitSD();
    }

    unsigned char CMD24_WRITE_SINGLE_BLOCK[] = {24, 0x00, 0x00, 0x00, 0x00, 0xFF};
    block = block * SD_BLOCK_SIZE * 8; //need to be correct offset
    CMD24_WRITE_SINGLE_BLOCK[1] = ((block & 0xFF000000) >> 24);
    CMD24_WRITE_SINGLE_BLOCK[2] = ((block & 0x00FF0000) >> 16);
    CMD24_WRITE_SINGLE_BLOCK[3] = ((block & 0x0000FF00) >> 8);
    CMD24_WRITE_SINGLE_BLOCK[4] = ((block & 0x000000FF));

    SD_Enable();

    // Send the write command
    status = SD_WriteCommand(CMD24_WRITE_SINGLE_BLOCK);

    if (status != 0) {
        //printbyte(status);
        // ABORT: invalid response for write single command
        return 1;
    }

    //write data start token
    SPIWrite(0xFE);

    //write all the bytes in the block
    theData = buffer;
    for (i = 0; i < SD_BLOCK_SIZE * 8; ++i) {
        SPIWrite(*theData);
        //printbyte(*theData);
        theData++;
    }

    // Write CRC bytes
    SPIWrite(0xFF);
    SPIWrite(0xFF);

    //prints(msg);

    //wait to complete
    status = SPIRead();
    while (status != 0xFF) {
        //prints(msg);
        status = SPIRead();
    }

    SD_Disable();

    //wait 8 clock cycles
    SPIWrite(0xFF);

    /*
     * The TRISB and LATB has to be LOW when accessing the SD card
     * but it has to be high for the ADC to receive the right amount of voltage
     */
    TRISB = 0x0000;
    //LATB = 0x0000;

    return (0);

}

//internal functions

unsigned char InitSD(void) {
    unsigned int i = 0;
    unsigned char status;

    // Turn off SD Card
    SD_Disable();

    // Wait for power to really go down
    for (i = 0; i; i++);
    for (i = 0; i; i++);
    for (i = 0; i; i++);
    for (i = 0; i; i++);

    // Turn on SD Card
    SD_PowerOn();

    // Wait for power to really come up
    for (status = 0; status < 10; ++status) {
        for (i = 0; i; i++);
        for (i = 0; i; i++);
        for (i = 0; i; i++);
        for (i = 0; i; i++);
    }

    // We need to give SD Card about a hundred clock cycles to boot up
    for (i = 0; i < 16; ++i) {
        SPIWrite(0xFF); // write dummy data to pump clock signal line
    }

    SD_Enable();

    // This is the only command required to have a valid CRC
    // After this command, CRC values are ignore unless explicitly enabled using CMD59
    unsigned char CMD0_GO_IDLE_STATE[] = {00, 0x00, 0x00, 0x00, 0x00, 0x95};

    // Wait for the SD Card to go into IDLE state
    i = 0;
    do {
        status = SD_WriteCommand(CMD0_GO_IDLE_STATE);

        // fail and return
        if (i++ > 250) {
            return 1;
        }

    } while (status != 0x01);

    // Wait for SD Card to initialize
    unsigned char CMD1_SEND_OP_COND[] = {01, 0x00, 0x00, 0x00, 0x00, 0xFF};

    i = 0;
    do {
        status = SD_WriteCommand(CMD1_SEND_OP_COND);
        if (i++ > 50) {
            return 2;
        }
    } while ((status & R1_IN_IDLE_STATE) != 0);

    // Send CMD55, required to precede all "application specific" commands
    unsigned char CMD55_APP_CMD[] = {55, 0x00, 0x00, 0x00, 0x00, 0xFF};
    status = SD_WriteCommand(CMD55_APP_CMD); // Do not check response here

    // Send the ACMD41 command to initialize SD Card mode (not supported by MMC cards)
    i = 0;
    unsigned char ACMD41_SD_SEND_OP_COND[] = {41, 0x00, 0x00, 0x00, 0x00, 0xFF};
    do {
        status = SD_WriteCommand(ACMD41_SD_SEND_OP_COND);
        // Might return 0x04 for Invalid Command if MMC card is connected

        if (i++ > 50) {
            return 3;
        }
    } while ((status & R1_IN_IDLE_STATE) != 0);

    // Set the SPI bus to full speed now that SD Card is initialized in SPI mode
    SD_Disable();
    //SPIFastClock();

    return 0;
}

void InitSPI(void) {
    SD_PowerOff();
    SD_PWR_DIR = 0; // output
    SD_PowerOff();

    SD_Disable();
    SD_CS_DIR = 0; // output
    SD_Disable();

    SDI_DIR = 1; // input
    SCK_DIR = 1;
    SDO_DIR = 1;


    SPI1CON = 0;
    SPI1STAT = 0;
    SPI1BRG = 0x0004; //BAUD = 4Mhz = Fpb/(2*(SPI1BRG+1))    //0x60;    // 50 khz BRG
    SPI1CONbits.CKE = 1; //Rising edge
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.ON = 1;
    SPI1CONbits.MODE16 = 0;
    SPI1CONbits.MODE32 = 0;
}

unsigned char SD_WriteCommand(unsigned char* cmd) {
    unsigned int i;
    unsigned char response;
    unsigned char savedSD_CS = SD_CS;

    // SD Card Command Format
    // (from Section 5.2.1 of SanDisk SD Card Product Manual v1.9).
    // Frame 7 = 0
    // Frame 6 = 1
    // Command (6 bits)
    // Address (32 bits)
    // Frame 0 = 1

    // Set the framing bits correctly (never change)
    cmd[0] |= (1 << 6); // Set Transmission bit = '1'
    cmd[0] &= ~(1 << 7); // Set Start bit = '0'
    cmd[5] |= (1 << 0); // Set Stop bit = '1'

    // Send the 6 byte command
    SD_Enable();

    for (i = 0; i < 6; ++i) {
        SPIWrite(*cmd);
        cmd++;
    }

    // Wait for the response
    i = 0;
    do {
        response = SPIRead();

        if (i > 60000) //instead of 100
        {
            break;
        }
        i++;
    } while (response == 0xFF);

    SD_Disable();

    // Following any command, the SD Card needs 8 clocks to finish up its work.
    // (from SanDisk SD Card Product Manual v1.9 section 5.1.8)
    SPIWrite(0xFF);

    SD_CS = savedSD_CS;
    return (response);
}

void SPIWrite(unsigned char data) {
    // DO NOT WAIT FOR SPITBF TO BE CLEAR HERE
    // (for some reason, it doesn't work on this side of the write data).

    // Write the data!
    //SPI1BUF = data;
    SPI1BUF = data;
    // Wait until send buffer is ready for more data.
    while (SPI1STATbits.SPIBUSY); // Initially it was SPITBF but cannot resolve
}

unsigned char SPIRead(void) {
    unsigned char data;

    if (SPI1STATbits.SPIRBF) {
        // already have some data to return, don't initiate a read
        data = SPI1BUF;

        SPI1STATbits.SPIROV = 0;
        return data;
    }

    // We don't have any data to read yet, so initiate a read
    SPI1BUF = 0xFF; // write dummy data to initiate an SPI read
    while (SPI1STATbits.SPIBUSY); // wait until the data is finished reading
    data = SPI1BUF;

    SPI1STATbits.SPIROV = 0;
    return data;
}

unsigned char printbyte(unsigned char val) {
    unsigned char nibble;
    unsigned char res;
    //build rep of high byte and send
    nibble = val / HEX;
    if (res = printhexdigit(nibble))
        return PRINT_ERR;
    nibble = val % HEX;
    if (res = printhexdigit(nibble))
        return PRINT_ERR;
    return OK;
}
//makes the non-ASCII digit into a hex digit
//and prints it

unsigned char printhexdigit(unsigned char digit) {
    if (digit >= 0x0A && digit <= 0x0F) {
        //have A - F
        digit = digit + 0x37;
    } else if (digit <= 0x09) {
        //have 0 - 9
        digit = digit + 0x30;
    } else {
        return PRINT_ERR;
    }
    while (U2STAbits.UTXBF == 1);
    U2TXREG = digit;
    return OK;
}

void testSPI() {
    SPIWrite(0xAA);
    //    SPIWrite(0xFF);
    //    SPIWrite(0xF0);
    //    SPIWrite(0x0F);
    //    SPIWrite(0x3C);
}

void fillTempBuffer() {
    int i;
    for (i = 0; i < SD_BLOCK_SIZE * 8; i++) {
        tempBuffer[i] = i % 255;
    }
}

void emptyBuffer() {
    int i;
    for (i = 0; i < SD_BLOCK_SIZE * 8; i++) {
        buffer[i] = 0;
    }
}

/* Fill tempBuffer[] with int 0 to 63
 * Write it to the current block.
 * Empty tempBuffer[] to all 0.
 * Read from the current block to make sure that it returns the right value.
 */

void testSDReadWrite(char* buff) {
    int i = 0;
    //put in write queue
    for (i = 0; i < SD_BLOCK_SIZE * 8; i++) {
        buffer[i] = *buff;
        buff++;
    }

    SDWriteBlock(currBlock);
    emptyBuffer();
    SDReadBlock(currBlock);
    //currBlock++;
    return;


}

void forwardDataToPrinter() {
    //WriteString("asdklfjasdjklfhasdkfhasdfhasdfasdgfjasdgfsdfggfgasjkfaskfgasdjkfgasdkjfgasdjkfhgasdf");
    //return;
    int j;
    while(curr_read_block != currBlock) {
        SDReadBlock(curr_read_block);
        char tempArray[512];
        for(j = 0; j < 512; j++) {
            tempArray[j] = (char) buffer[j];
        }
        WriteString(tempArray);

        curr_read_block++;
    }
}
void addByteToBuffer(char characterToWrite) {
    buffer[bufferIndex++] = characterToWrite;
}
#endif	/* SDCARD_H */

