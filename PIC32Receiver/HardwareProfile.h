/********************************************************************
 FileName:      HardwareProfile.h
 Dependencies:  See INCLUDES section
 Company:       Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company? for its PIC?Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS?CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
********************************************************************/

#ifndef _HARDWARE_PROFILE_H_
#define _HARDWARE_PROFILE_H_

#define MEDIA_SOFT_DETECT

/*******************************************************************/
/******** Clock Definitions ****************************************/
/*******************************************************************/
#ifndef SYS_CLOCK
    #error "Define SYS_CLOCK (ex. -DSYS_CLOCK=80000000) on compiler command line"
#endif


/*******************************************************************/
/******** MDD File System selection options ************************/
/*******************************************************************/
#define USE_PIC32
#define USE_32BIT

#define USE_SD_INTERFACE_WITH_SPI

//SPI Configuration
#define SPI_START_CFG_1     (PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON)
#define SPI_START_CFG_2     (SPI_ENABLE)

// Define the SPI frequency
#define SPI_FREQUENCY       (20000000)

#define SD_CS               PORTBbits.RB1
#define SD_CS_TRIS          TRISBbits.TRISB1

#define SD_CD               PORTDbits.RD6
#define SD_CD_TRIS          TRISDbits.TRISD6

#define SD_WE               PORTFbits.RF1
#define SD_WE_TRIS          TRISFbits.TRISF1

// Registers for the SPI module you want to use
#define SPICON1             SPI1CON
#define SPISTAT             SPI1STAT
#define SPIBUF              SPI1BUF
#define SPISTAT_RBF         SPI1STATbits.SPIRBF
#define SPICON1bits         SPI1CONbits
#define SPISTATbits         SPI1STATbits
#define SPIENABLE           SPICON1bits.ON
#define SPIBRG              SPI1BRG
// Tris pins for SCK/SDI/SDO lines
#define SPICLOCK            TRISFbits.TRISF6
#define SPIIN               TRISFbits.TRISF7
#define SPIOUT              TRISFbits.TRISF8

#define putcSPI             putcSPI1
#define getcSPI             getcSPI1
#define OpenSPI(config1, config2)   OpenSPI1(config1, config2)
    
#endif  //_HARDWARE_PROFILE_H_
