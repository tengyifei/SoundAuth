unsigned int upper_128[128];

#include <GenericTypeDefs.h>
#include <plib.h>
#include "SFlash.h"

int DFlash_CurrentSector, DFlash_CurrentPage, DFlash_CurrentBlock;

void Delayms(unsigned int msec)
{
	unsigned int tWait, tStart;
		
    tWait=(80000000L/1000)*msec;
    tStart=ReadCoreTimer();
    while((ReadCoreTimer()-tStart)<tWait);		// wait for the time to pass
}

//#=================================================================================================

//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
// send one byte of data and receive one back at the same time
int __attribute__ ((section ("Sector_OSBoot"))) DF_WriteSPI2( int i) 
{
    SPI2BUF = i;                  // write to buffer for TX
    while( !SPI2STATbits.SPIRBF); // wait for transfer complete
    return SPI2BUF;               // read the received value
}//DF_WriteSPI2
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) DF_WriteEnable( void)
{
    // send a Write Enable command
    DF_CS = 0;               // select the Serial EEPROM
    DF_WriteSPI2( SEE_WEN );    // write enable command
    DF_CS = 1;               // deselect to complete the command
}// DF_WriteEnable 
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) DF_Init_Ports( void)
{
    // init the SPI2 peripheral 
    DF_CS = 1;                   // de-select the Serial EEPROM
    DF_TCS = 0;                  // make SSEE pin output
    SPI2CON = SPI_CONF;         // enable the peripheral

    SPI2BRG = SPI_BAUD_FAST; //SPI_BAUD;         // select clock speed
    
}// initSEE
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) DF_Init( void)
{
    DF_WriteEnable();
    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP
    DF_CS = 0;               // select the Serial EEPROM
    DF_WriteSPI2( SEE_WRSR);    // write enable command
    DF_WriteSPI2(0);    // write enable command
    DF_CS = 1;

    DF_WriteEnable();
    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP
    DF_CS = 0;               // select the Serial EEPROM
    DF_WriteSPI2( SEE_WRSR);    // write enable command
    DF_WriteSPI2(0);    // write enable command
    DF_CS = 1;

DFlash_CurrentSector = 0;
DFlash_CurrentPage = 0;
DFlash_CurrentBlock = 0;

}// initSEE
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
int __attribute__ ((section ("Sector_OSBoot"))) DF_ReadStatus( void)
{
    // Check the Serial EEPROM status register
    int i;	
    DF_CS = 0;                   // select the Serial EEPROM
    DF_WriteSPI2( SEE_STAT);       // send a READ STATUS COMMAND
    i = DF_WriteSPI2( 0);          // send/receive
    DF_CS = 1;                   // deselect terminate command
    return i;
} // DF_ReadStatus
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
int __attribute__ ((section ("Sector_OSBoot"))) DF_ReadW(unsigned int address)
{ // read a 32-bit value starting at an even address	
    int i,J;
	J = address<<1;
	//J = address;

    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP

    // perform a 16-bit read sequence (two byte sequential read)
    DF_CS = 0;                   // select the Serial EEPROM
    DF_WriteSPI2( SEE_READ);       // read command
    DF_WriteSPI2( J >>16);    // address MSB first
    DF_WriteSPI2( J >>8);    // address MSB first
    DF_WriteSPI2( J ); // address LSB (word aligned)
    
    i = DF_WriteSPI2( 0);          // send dummy, read msb
    i = (i<<8)+ DF_WriteSPI2( 0);  // send dummy, read lsb
    DF_CS = 1;
    return ( i);
}// readSEE
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
int __attribute__ ((section ("Sector_OSBoot"))) DF_ReadD( int address)
{ // read a 32-bit value starting at an even address	
    int i,J, K;
	J = address<<1;
	
    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP

    // perform a 16-bit read sequence (two byte sequential read)
    DF_CS = 0;                   // select the Serial EEPROM
    DF_WriteSPI2( SEE_READ);       // read command
    DF_WriteSPI2( J >>16);    // address MSB first
    DF_WriteSPI2( J >>8);    // address MSB first
    DF_WriteSPI2( J ); // address LSB (word aligned)
    
    i = DF_WriteSPI2( 0);          // send dummy, read msb
    i = (i<<8)+ DF_WriteSPI2( 0);  // send dummy, read lsb
    i = (i<<8)+ DF_WriteSPI2( 0);  // send dummy, read lsb
    i = (i<<8)+ DF_WriteSPI2( 0);  // send dummy, read lsb
    DF_CS = 1;

    return ( i);
}// readSEE
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) DF_SectorErase(int sector)
{
//#ifdef EEPROM_flash
	int K;
    while ( DF_ReadStatus() & 0x1);// check the WIP flag
    // Set the Write Enable Latch
    DF_WriteEnable();

    DF_CS = 0;                   // select the Serial EEPROM
    K = DF_WriteSPI2( 0X39);      // write command
    DF_WriteSPI2( sector);     // address MSB first
    //K = DF_WriteSPI2( 2);     // address MSB first
    K = DF_WriteSPI2( 0);     // address MSB first
    K = DF_WriteSPI2( 0); // address LSB (word aligned)

    DF_CS = 1;
    while ( DF_ReadStatus() & 0x1);// check the WIP flag 

    // Set the Write Enable Latch
    DF_WriteEnable();
    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP


    DF_CS = 0;                   // select the Serial EEPROM
    K = DF_WriteSPI2( 0XD8);      // write command ### NOTE 64K:0XD8 32K:0X52 4K:0X20 ###
    DF_WriteSPI2( sector);     // address MSB first
    //K = DF_WriteSPI2( 2);     // address MSB first
    K = DF_WriteSPI2( 0);     // address MSB first
    K = DF_WriteSPI2( 0); // address LSB (word aligned)
    while ( DF_ReadStatus() & 0x1);// check the WIP flag  

    DF_CS = 1;

	Delayms(2400);
    while ( DF_ReadStatus() & 0x1);// check the WIP flag 
	Delayms(400); 

//#endif	
}
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) DF_SectorErase_4K(int sector)
{
//#ifdef EEPROM_flash
	int K;
    while ( DF_ReadStatus() & 0x1);// check the WIP flag
    // Set the Write Enable Latch
    DF_WriteEnable();

    DF_CS = 0;                   // select the Serial EEPROM
    K = DF_WriteSPI2( 0X39);      // write command
    DF_WriteSPI2( sector>>16 );     // address MSB first
    K = DF_WriteSPI2( sector>>8 );     // address MSB first
    K = DF_WriteSPI2( sector ); // address LSB (word aligned)

    DF_CS = 1;
    while ( DF_ReadStatus() & 0x1);// check the WIP flag 

    // Set the Write Enable Latch
    DF_WriteEnable();
    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP


    DF_CS = 0;                   // select the Serial EEPROM
    K = DF_WriteSPI2( 0X20);      // write command ### NOTE 64K:0XD8 32K:0X52 4K:0X20 ###
    DF_WriteSPI2( sector>>16 );     // address MSB first
    K = DF_WriteSPI2( sector>>8 );     // address MSB first
    K = DF_WriteSPI2( sector ); // address LSB (word aligned)
    while ( DF_ReadStatus() & 0x1);// check the WIP flag  

    DF_CS = 1;

	Delayms(200);
    while ( DF_ReadStatus() & 0x1);// check the WIP flag 
	Delayms(200); 
//#endif	
}
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) DF_WriteW( int address, int data)
{ // write a 32-bit value starting at an even address
    int J, K;
	J = address<<1;
	
    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check the WIP flag
  
    // Set the Write Enable Latch
    DF_WriteEnable();

    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP

    // perform a 32-bit write sequence (4 byte page write)
    DF_CS = 0;                   // select the Serial EEPROM
    K = DF_WriteSPI2( SEE_WRITE);      // write command
    K = DF_WriteSPI2( J>>16);     // address MSB first
    K = DF_WriteSPI2( J>>8);     // address MSB first
    K = DF_WriteSPI2( J); // address LSB (word aligned)

    K = DF_WriteSPI2( data >>8);       // send msb
    K = DF_WriteSPI2( data);           // send lsb
    DF_CS = 1;
}// writeSEE
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) DF_WritePage( int address, unsigned short int *ptr )
{ // write a 32-bit value starting at an even address
    int J,K;
	J = address*256;

    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check the WIP flag
    // Set the Write Enable Latch
    DF_WriteEnable();

    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP

    // perform a 32-bit write sequence (4 byte page write)
    DF_CS = 0;                   // select the Serial EEPROM
    DF_WriteSPI2( SEE_WRITE);      // write command
    DF_WriteSPI2( J>>16);     // address MSB first
    DF_WriteSPI2( J>>8);     // address MSB first
    DF_WriteSPI2( J); // address LSB (word aligned)

	ptr = ptr + 0X2000;

	for (K=0; K<128;K++) //1 PAGE
	{
		//J = CORE_MEM[K+0X1000];
		J = *ptr;
		ptr = ptr + 1;
	    DF_WriteSPI2( J>>8 );       // send msb
	    DF_WriteSPI2( J );           // send lsb
    }
    
    //while ( DF_ReadStatus() & 0x1);// check the WIP flag  
    DF_CS = 1;
}// writeSEE
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------


//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) xxxDF_WritePage(unsigned int sector, unsigned short int *ptr )
{ // write a 32-bit value starting at an even address
    unsigned int J,K;
	//J = STARTaddr + address*256;
J = sector<<1;

    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check the WIP flag
    // Set the Write Enable Latch
    DF_WriteEnable();

    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP

    // perform a 32-bit write sequence (4 byte page write)
    DF_CS = 0;                   // select the Serial EEPROM
    DF_WriteSPI2( SEE_WRITE);      // write command
    DF_WriteSPI2( J>>16);     // address MSB first
    DF_WriteSPI2( J>>8);     // address MSB first
    DF_WriteSPI2( J); // address LSB (word aligned)


	for (K=0; K<128;K++) //1 PAGE
	{
		//J = CMEM[K];
		J = *ptr;
		ptr = ptr + 1;
	    DF_WriteSPI2( J>>8 );       // send msb
	    DF_WriteSPI2( J );           // send lsb
    }
    
    //while ( DF_ReadStatus() & 0x1);// check the WIP flag  
    DF_CS = 1;
	Delayms(20); 
//putsUART1(">DFLASH : write\r");//DEBUG
}// writeSEE
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------
void __attribute__ ((section ("Sector_OSBoot"))) xxxDF_ReadPage(unsigned int sector, unsigned short int *ptr )
{ // write a 32-bit value starting at an even address
    unsigned int i, J,K;
	//J = STARTaddr + address*256;
J = sector<<1;

    // wait until any work in progress is completed
    while ( DF_ReadStatus() & 0x1);// check WIP

    // perform a 32-bit write sequence (4 byte page write)
    DF_CS = 0;                   // select the Serial EEPROM
    DF_WriteSPI2( SEE_READ);      // write command
    DF_WriteSPI2( J>>16);     // address MSB first
    DF_WriteSPI2( J>>8);     // address MSB first
    DF_WriteSPI2( J); // address LSB (word aligned)

	for (K=0; K<128;K++) //1 PAGE
	{
	    i = DF_WriteSPI2( 0);          // send dummy, read msb
	    i = (i<<8)+ DF_WriteSPI2( 0);  // send dummy, read lsb
		*ptr = i;
		ptr = ptr + 1;

	    //DF_WriteSPI2( J>>8 );       // send msb
	    //DF_WriteSPI2( J );           // send lsb
    }
    
    //while ( DF_ReadStatus() & 0x1);// check the WIP flag  
    DF_CS = 1;
}// writeSEE
//#=================================================================================================
//#-------------------------------------------------------------------------------------------------



//#=================================================================================================
//#-------------------------------------------------------------------------------------------------


