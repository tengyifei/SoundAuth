#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1
#pragma config FPBDIV = DIV_1, FCKSM = CSECME
#pragma config POSCMOD = HS, FNOSC = PRIPLL 
#pragma config FWDTEN = OFF
#pragma config OSCIOFNC = ON, FSOSCEN = ON
#pragma config CP = OFF, BWP = OFF, PWP = OFF

#define SYS_FREQ (80000000L)
#define SPL_RATE 6000
#define BUFF_LEN 60
#define THRESHOLD 220		//some sound amplitude threshold to blink the green light
#define WIDTH 70			//some +-10Hz frequency matching tolerance

#define PAGE_LEN 512

#define	GetSystemClock 			(80000000l)
#define	GetPeripheralClock		(GetSystemClock/(1 << OSCCONbits.PBDIV))
#define	GetInstructionClock		(GetSystemClock)

#define MEDIA_SOFT_DETECT

#include <GenericTypeDefs.h>
#include <plib.h>
#include <adc10.h>
#include <timer.h>
#include "sha1.h"
#include "FSConfig.h"
#include "SD-SPI.h"
//#include "SFlash.h"

unsigned int channel0;			// conversion result as read from result buffer
unsigned int 	spl=0,finished=1,magn=0,max=0,min=0,i=0,k=0,magn500=0,hist=0,
				maxfreqamp=0,nearestF=0,l=0,accu=0,len_freq=0;

unsigned int *recentsamp, *tempptr;		//keep track of recent BUFF_LEN samples
unsigned int recentptr=0;

unsigned int sampbuffA[BUFF_LEN];		//double buffering?....
unsigned int sampbuffB[BUFF_LEN];

double sinPart=0, cosPart=0;

float argsin[16][BUFF_LEN];				//precomputed fourier transform data
float argcos[16][BUFF_LEN];

int j,curr_freq;
int freq[160];
int ref_freq[32];						//data extracted from sound goes here
unsigned char c_ref_freq[16];

char inpmessage[1024*8];
char dbhdbuff[512];			//only stores header
char dbbuff[512];			//can only read 1 sector at a time
char msg[512];
char msg2[512];

int timeout=2500;

//unsigned char message[8]={0x00, 0x00, 0x00, 0x16, 0xBC, 0xAB, 0xED, 0x33};	//ID message
//unsigned char counter[4]={0x00, 0x00, 0x01, 0xCB};							//0x1CB times
unsigned char hashInp[12];				//Input data for hashing
unsigned char hashRes[16];				//Unfortunately, only 128 bits (16 bytes) are taken into account out of 160 bits

void DelayMs2(unsigned int msec);
BOOL SendDataBuffer(const char *buffer, UINT32 size);
BOOL GetDataBuffer(char *buffer, UINT32 max_size);
BOOL SendDataLong(UINT32 buffer);
UINT32 GetDataLong();
BOOL SendMessage(char* buffer);

const char defaultpass[9]={'P', 'A', 'S', 'S', 'W', 'O', 'R', 'D', '\0'};

BOOL consolemode = FALSE;

/*	Communication Protocol
Process:
1.	PIC receives a 'w' character from PC
2.	PIC sends dummy response
3.	Both enters communication cycle, sending and reading dummy response from each other non-stop
4.	PIC acts passively
5.	If PC wants to send command, it sends CPU_SEND_DATA instead of CPU_SEND_DUMMY, 
	followed by a 32-bit UINT representing length of data, and then data
6.	PIC processes the information, and send the response data through PIC_SEND_DATA, 
	followed by a 32-bit UINT representing length of data, and then response data
*/
#define PIC_SEND_DATA	0xDA1A1011
#define PIC_SEND_DUMMY	0xB1B1CAFE
#define CPU_SEND_DATA	0x672BF915
#define CPU_SEND_DUMMY	0xD2D2CAFE
#define PIC_EXIT		0x11FF11FF
#define CPU_EXIT		0x39273FFF

//start command of data (pure message does not start with 0xFF)
#define CPU_DOWNLOAD	0xFF583B99
#define CPU_INSERT		0xFF912E55
#define CPU_DELETE		0xFF348D12
#define CPU_EDIT		0xFF7142BF

#define PIC_DOWNLOAD	0xFF71D1AA
#define PIC_DOWNLOAD_FIN 0xFFC1C2C3

/*	Communication Protocol	*/

//tree data only starts at 0x4000 sector (65536 user slots reserved), thus minimum 8MB card required
#define TREE_START_SECTOR	0x4000

/*	Data Storage
1st sector on SD card: database information
2-nth sector: user database
4 users per sector
Name(64bytes)+Data(8bytes)+Counter(4bytes)+Identification Info(4bytes)+deleted?(4bytes)+hashpresent?(4bytes	)
*/
typedef struct DBDataTag{
	char name[64];
	char message[8];
	char counter[4];
	UINT32 idinfo;		//0=>allow, 1=>disallow
	UINT32 deleted;		//0=>deleted, 1=>present
	UINT32 hashpresent;	//PRESENT_MAGIC=>present, others=>not present
}DBData, *PDBDATA;

#define PRESENT_MAGIC 0x93929166
#define DBMAGIC_VAL 0x7968574D
#define DBTAILMAGIC_VAL 0xE59B230C

typedef struct DBHeaderTag{
	UINT32 dbmagic;		//should always be 0x7968574D, used in checking for validity
	char pwhash[32];	//sha-1 hash of db password. size is not 20 but 32 for alignment
	UINT32 num_user;
	UINT32 num_user_deleted;
	UINT32 reserved1;
	UINT32 reserved2;
	UINT32 dbtailmagic;	//should always be 0xE59B230C, used in checking for validity
}DBHeader, *PDBHeader;
/*Data Storage*/

BOOL Dld_data(PDBHeader dbhd){
	UINT32 currSector=1;
	PDBDATA currdbbuff;
	int currdbpos;
	int i=0;
	for (i=0; i<dbhd->num_user; i++){
		if (i%4==0){
			MDD_SDSPI_SectorRead(currSector, dbbuff);
			currdbbuff=(PDBDATA)dbbuff;
			currdbpos=0;
			currSector++;
		}
		if (currdbbuff->hashpresent!=PRESENT_MAGIC){
			int k;
			for (k=0; k<8; k++)	hashInp[k]=currdbbuff->message[k];		//reverse the small-endian mode
			for (k=8; k<12; k++)hashInp[k]=currdbbuff->counter[k-8];		
			void* pNum=(void*)(hashInp);
			unsigned char* data=(unsigned char*)Sha1(pNum,96);		//resulting length 20 bytes
			for (k=0; k<16; k++) {hashRes[k]=data[k];}
			if (!SendDataLong(PIC_SEND_DATA)) return FALSE;
			sprintf(msg, "User %d not initialized, Hashing... %x%x%x", i, hashRes[0], hashRes[1], hashRes[2]);
			if (!SendMessage(msg)) return FALSE;
			if (GetDataLong()==0xFFFFFFFF) return FALSE;
			currdbbuff->hashpresent=PRESENT_MAGIC;
			MDD_SDSPI_SectorWrite(currSector-1, dbbuff, TRUE);
			MDD_SDSPI_SectorRead(TREE_START_SECTOR+((i-i%32)/32), msg2);			//read in hash table
			memcpy(msg2+(i%32)*16, hashRes, 16);
			MDD_SDSPI_SectorWrite(TREE_START_SECTOR+((i-i%32)/32), msg2, TRUE);		//write back hash table
		}
		UINT32 dld=PIC_DOWNLOAD;
		memcpy(msg, &dld, sizeof(dld));
		memcpy((msg+sizeof(dld)), currdbbuff, 128);
		if (!SendDataLong(PIC_SEND_DATA)) return FALSE;
		if (!SendDataBuffer(msg, sizeof(dld)+128)) return FALSE;
		if (GetDataLong()==0xFFFFFFFF) return FALSE;
		currdbpos+=128;
		currdbbuff=(PDBDATA)&dbbuff[currdbpos];
	}
	if (!SendDataLong(PIC_DOWNLOAD_FIN)) return FALSE;
	if (GetDataLong()==0xFFFFFFFF) return FALSE;
	if (!SendDataLong(PIC_SEND_DATA)) return FALSE;
	sprintf(msg, "Sent %d user information", dbhd->num_user);
	if (!SendMessage(msg)) return FALSE;
	return TRUE;
}

BOOL Ins_data(PDBHeader dbhd){
	PDBDATA object=(PDBDATA)(&inpmessage[4]);
	UINT32 calc_lrgoffset=(dbhd->num_user-(dbhd->num_user%4))/4+1;		//starting from the 1st page
	MDD_SDSPI_SectorRead(calc_lrgoffset, dbbuff);
	int calc_offset=(dbhd->num_user%4)*128;
	memcpy(&dbbuff[calc_offset], object, sizeof(DBData));
	MDD_SDSPI_SectorWrite(calc_lrgoffset, dbbuff, TRUE);
	dbhd->num_user++;
	MDD_SDSPI_SectorWrite(0, dbhdbuff, TRUE);							//update user count
	if (!SendDataLong(PIC_SEND_DATA)) return FALSE;
	sprintf(msg, "Inserted");
	if (!SendMessage(msg)) return FALSE;
	return TRUE;
}

BOOL Del_data(PDBHeader dbhd){
	UINT32* idx=(UINT32*)(&inpmessage[4]);							//obtain index of user
	if (!SendDataLong(PIC_SEND_DATA)) return FALSE;
	sprintf(msg, "Deleted User %d", *idx);
	if (!SendMessage(msg)) return FALSE;
	return TRUE;
}

BOOL Edit_data(PDBHeader dbhd){
	UINT32 idx=*((UINT32*)(&inpmessage[4]));							//obtain index of user
	PDBDATA object=(PDBDATA)(&inpmessage[8]);
	UINT32 calc_lrgoffset=idx/4+1;		//starting from the 1st page
	MDD_SDSPI_SectorRead(calc_lrgoffset, dbbuff);
	int calc_offset=(idx%4)*128;
	memcpy(&dbbuff[calc_offset], object, sizeof(DBData));
	object->hashpresent=0;
	MDD_SDSPI_SectorWrite(calc_lrgoffset, dbbuff, TRUE);
	if (!SendDataLong(PIC_SEND_DATA)) return FALSE;
	sprintf(msg, "Edited User %d", idx);
	if (!SendMessage(msg)) return FALSE;
	return TRUE;
}

void __ISR(_TIMER_1_VECTOR, ipl7) _Timer1Handler(void)			//stream checking with highest priority
{
	if (spl==1){			//a switch

		//while ( !BusyADC10() );			//assuming ADC has already finished(which is true)

	    // Reads the buffer that is not being populated (we don't want to read the active buffer)
		if(AD1CON2bits.BUFS==1)
		{
			channel0=ReadADC10(0);
		}
		else
		{
			channel0=ReadADC10(8);
		}

		AcquireADC10();		//start sampling manually

		recentsamp[recentptr]=channel0;
		recentptr++;
		if (recentptr==BUFF_LEN) {			//write to alternative buffer
			if (recentsamp==sampbuffA)
				recentsamp=sampbuffB;
			else
				recentsamp=sampbuffA;
			recentptr=0; finished=0;
		}
	}
	mT1ClearIntFlag();
}

void setupUART(){
    UARTConfigure(UART2, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART2, GetPeripheralClock, 115200);
    UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
}

void setupADC(){
	// configure and enable the ADC
	CloseADC10();	// ensure the ADC is off before setting the configuration

	// define setup parameters for OpenADC10
	// 				Turn module on | ouput in integer | trigger mode auto | enable autosample 
	#define PARAM1  ADC_FORMAT_INTG | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_OFF 

	// define setup parameters for OpenADC10
	// 				ADC ref external    | disable offset test    | disable scan mode | perform 8 samples | use dual buffers | use alternate mode
	#define PARAM2  ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_8 | ADC_ALT_BUF_ON | ADC_ALT_INPUT_OFF 

	// define setup parameters for OpenADC10
	//				  use ADC PB clock| set sample time | auto
	#define PARAM3  ADC_CONV_CLK_PB | ADC_SAMPLE_TIME_2 | ADC_CLK_AUTO 

	// define setup parameters for OpenADC10
    // AN2 as analog inputs
	#define PARAM4	ENABLE_AN2_ANA

	// define setup parameters for OpenADC10
	// do not assign channels to scan
	#define PARAM5	SKIP_SCAN_ALL
	
	// use ground as neg ref for A | use AN4 for input A      |  use ground as neg ref for A | use AN5 for input B

	// configure to sample AN2
	SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN2); 		// configure to sample AN2
	OpenADC10( PARAM1, PARAM2, PARAM3, PARAM4, PARAM5 ); 						// configure ADC using the parameters defined above
	
	AD1CON3bits.ADCS=0x01; //Note the 65 NS minimum TAD from datasheet, don't use FRM

	EnableADC10(); // Enable the ADC
}

void setupTimer(){
	OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, 0xFFFF);

	WritePeriod1(51);			//6KHz sampling rate
 
	// set up the core timer interrupt with a prioirty of 2 and zero sub-priority
	ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_7);
 
	// enable device multi-vector interrupts
	INTEnableSystemMultiVectoredInt();
}

void tryEnterMgmt(){
	/*if (consolemode){
		timeout=500;
		if (GetDataLong()==0xFFFFFFFF) {consolemode=FALSE;timeout=2500;goto out2;}
		if (!SendMessage("Access Denied")) consolemode=FALSE;
		timeout=2500;
	}
out2:*/
	if(UARTReceivedDataIsAvailable(UART2)){
		spl=0;									//disable sampling first
		UINT8 pw='w';
		UINT8 pw2='c';
		UINT8 en=UARTGetDataByte(UART2);
		UINT32 tmpi=0;
		UINT32 tmpo=PIC_SEND_DUMMY;
		if (en==pw){		//enter cycle


			SendDataLong(PIC_SEND_DUMMY);		//response to PC

			tmpi=GetDataLong();
			if (tmpi==0xFFFFFFFF) return;
			if (tmpi==CPU_SEND_DUMMY){
				if (!SendDataLong(PIC_SEND_DATA)) return;
				if (MDD_SDSPI_MediaDetect()==TRUE){
					if (!SendMessage("Management Mode - MMC Present")) return;
				}else{
					if (!SendMessage("Management Mode - MMC Absent")) return;
					GetDataLong();
					if (!SendDataLong(PIC_EXIT)) return;			//confirm exit
					return;
				}
			}else{
				GetDataBuffer(inpmessage, GetDataLong());	//read message from PC
				if (!SendDataLong(PIC_SEND_DATA)) return;
				if (!SendMessage("Got it...but not supposed to...")) return;
			}

			if (GetDataLong()==0xFFFFFFFF) return;
			sprintf(msg, "Card Size: %x", MDD_SDSPI_ReadCapacity()*MDD_SDSPI_ReadSectorSize());
			if (!SendDataLong(PIC_SEND_DATA)) return;
			if (!SendMessage(msg)) return;

			if (MDD_SDSPI_SectorRead(0, dbhdbuff)==FALSE){
				if (GetDataLong()==0xFFFFFFFF) return;
				if (!SendMessage("Error: Cannot read card")) return;
				if (GetDataLong()==0xFFFFFFFF) return;
				if (!SendDataLong(PIC_EXIT)) return;			//confirm exit
				return;
			}

			PDBHeader dbhd=(PDBHeader)dbhdbuff;
			if (dbhd->dbmagic!=DBMAGIC_VAL || dbhd->dbtailmagic!=DBTAILMAGIC_VAL){
				if (GetDataLong()==0xFFFFFFFF) return;
				if (!SendDataLong(PIC_SEND_DATA)) return;
				if (!SendMessage("First Time Initialization")) return;
				dbhd->num_user=0;
				dbhd->num_user_deleted=0;
				dbhd->dbmagic=DBMAGIC_VAL;
				dbhd->dbtailmagic=DBTAILMAGIC_VAL;
				MDD_SDSPI_SectorWrite(0, (BYTE*)dbhd, TRUE);
			}else{
				if (GetDataLong()==0xFFFFFFFF) return;
				if (!SendDataLong(PIC_SEND_DATA)) return;
				sprintf(msg, "Number of users: %d", dbhd->num_user);
				if (!SendMessage(msg)) return;
			}

			while(1){
				tmpi=GetDataLong();				//get opcode from PC
				if (tmpi==0xFFFFFFFF) return;
				if (tmpi==CPU_SEND_DUMMY){
					if (!SendDataLong(PIC_SEND_DUMMY)) return;
				}else{
					tmpi=GetDataLong();
					if (tmpi==0xFFFFFFFF) return;
					if (!GetDataBuffer(inpmessage, tmpi)) return;			//read message from PC
					if (tmpi==4){
						UINT32 code=(*(unsigned long*)(&(inpmessage[0])));
						if (code==CPU_EXIT){				//cpu requests exit
							if (!SendDataLong(PIC_EXIT)) return;			//confirm exit
							break;
						}
					}
					UINT32 code=(*(unsigned long*)(&(inpmessage[0])));

					switch (code){
						case CPU_DOWNLOAD:
							if (Dld_data(dbhd)==FALSE) return;
						break;
						case CPU_INSERT:
							if (Ins_data(dbhd)==FALSE) return;
						break;
						case CPU_DELETE:
							if (Del_data(dbhd)==FALSE) return;
						break;
						case CPU_EDIT:
							if (Edit_data(dbhd)==FALSE) return;
						break;
						default:
							if (!SendDataLong(PIC_SEND_DATA)) return;
							sprintf(msg, "Got %d Data", tmpi);
							if (!SendMessage(msg)) return;
						break;
					}

				}
			}
		}
		if (en==pw2){
			SendDataLong(PIC_SEND_DUMMY);		//response to PC
			consolemode=TRUE;
			DelayMs2(500);
		}
		spl=1;
	}
}

BOOL search_dbase_l(){
	for (l=0; l<16; l++){
		if (c_ref_freq[l]!=hashRes[l]) {return FALSE;}
	}
	return TRUE;
}

int search_dbase(char* user_name, UINT32* user_info, UINT32* user_cnt){
	int i,j;
	if (!MDD_SDSPI_MediaDetect()) return -999;
	MDD_SDSPI_SectorRead(0, dbhdbuff);
	PDBHeader dbhd=(PDBHeader)dbhdbuff;
	if (dbhd->dbmagic!=DBMAGIC_VAL || dbhd->dbtailmagic!=DBTAILMAGIC_VAL) return -1;
	int curr_usr=0;
	for (j=0; j<=dbhd->num_user/32; j++){
		MDD_SDSPI_SectorRead(TREE_START_SECTOR+j, msg2);
		int page_usr=dbhd->num_user-curr_usr;
		if (page_usr>32) page_usr=32;
		for (i=0; i<page_usr; i++){
			memcpy(hashRes, msg2+i*16, 16);
			if (search_dbase_l()) {
				UINT32 calc_lrgoffset=curr_usr/4+1;
				MDD_SDSPI_SectorRead(calc_lrgoffset, msg2);
				memcpy(user_name, (char*)(((PDBDATA)&msg2[128*(curr_usr%4)])->name), 64);
				memcpy(user_info, &(((PDBDATA)&msg2[128*(curr_usr%4)])->idinfo), sizeof(UINT32));
				memcpy(user_cnt, &(((PDBDATA)&msg2[128*(curr_usr%4)])->counter), sizeof(UINT32));
				return curr_usr;
			}
			curr_usr++;
		}
	}
	return -1;
}

void incr_user_cnt(int id){
	UINT32 calc_lrgoffset=id/4+1;
	MDD_SDSPI_SectorRead(calc_lrgoffset, msg2);
	int calc_offset=(id%4)*128;
	(*((UINT32*)(((PDBDATA)(&msg2[calc_offset]))->counter)))++;
	MDD_SDSPI_SectorWrite(calc_lrgoffset, msg2, TRUE);
	PDBDATA currdbbuff=(PDBDATA)(&msg2[calc_offset]);
	int k;
	for (k=0; k<8; k++)	hashInp[k]=currdbbuff->message[k];		//reverse the small-endian mode
	for (k=8; k<12; k++)hashInp[k]=currdbbuff->counter[k-8];		
	void* pNum=(void*)(hashInp);
	unsigned char* data=(unsigned char*)Sha1(pNum,96);		//resulting length 20 bytes
	for (k=0; k<16; k++) {hashRes[k]=data[k];}
	MDD_SDSPI_SectorRead(TREE_START_SECTOR+((id-id%32)/32), msg2);			//read in hash table
	memcpy(msg2+(id%32)*16, hashRes, 16);
	MDD_SDSPI_SectorWrite(TREE_START_SECTOR+((id-id%32)/32), msg2, TRUE);		//write back hash table
}

void setupMMC(){
	//enable a pull-up for the card detect, just in case the SD-Card isn't attached
    //  then lets have a pull-up to make sure we don't think it is there.
	MDD_SDSPI_InitIO();
	MDD_SDSPI_MediaInitialize();
}

void unicode_to_ascii(char* target){
	int i=0;
	int j=0;
	while (target[i]!=0){
		target[j]=target[i];
		j++;
		i+=2;
	}
	target[j]=0;
}

int main(void)
{
	// Configure the device for maximum performance but do not change the PBDIV
	// Given the options, this function will change the flash wait states and
	// enable prefetch cache but will not change the PBDIV. The PBDIV value
	// is already set via the pragma FPBDIV option above..
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
	INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);

	PORTSetPinsDigitalOut(IOPORT_D, BIT_0 | BIT_1 | BIT_2);

	PORTSetPinsDigitalIn(IOPORT_F, BIT_7);
	PORTSetPinsDigitalOut(IOPORT_B, BIT_1);
	PORTSetPinsDigitalOut(IOPORT_F, BIT_6 | BIT_8);

	PORTClearBits(IOPORT_D, BIT_0 | BIT_1 | BIT_2);

	//initialize precomputated stuff
	for (i=0; i<16; i++)
		for (k = 0; k < BUFF_LEN; k++) {
			argsin[i][k] = sin((float)2.0*(float)((float)(500.0+(float)i*100.0)*(float)BUFF_LEN/(float)SPL_RATE)*(float)3.141592653589*(float)k/(float)BUFF_LEN);
			argcos[i][k] = cos((float)2.0*(float)((float)(500.0+(float)i*100.0)*(float)BUFF_LEN/(float)SPL_RATE)*(float)3.141592653589*(float)k/(float)BUFF_LEN);
		}

	recentsamp=sampbuffA;

	setupMMC();

	mPORTDSetBits(BIT_0);
				DelayMs2(500);
				mPORTDClearBits(BIT_0);

	//setupHash();
	setupUART();

mPORTDSetBits(BIT_0);
				DelayMs2(500);
				mPORTDClearBits(BIT_0);

	setupADC();
	AcquireADC10();
	while (!BusyADC10());			//wait for first value to come
	setupTimer();

	while (1){
		while (1){
				spl=1;								//start sampling
			while (finished==1);				//until finished writing one loop
			finished=1;
	
			if (recentsamp==sampbuffA)		//interrupt writing buffA, we use buffB then
				tempptr=sampbuffB;
			else
				tempptr=sampbuffA;
	
			max=0; min=0xFFFFFFFF;
			for (i=0; i<BUFF_LEN; i++){
				if (tempptr[i]>max) max=tempptr[i];
				if (tempptr[i]<min) min=tempptr[i];
			}
			magn=max-min;
	
			if (magn>THRESHOLD){			//loudness notification
				mPORTDSetBits(BIT_1);
			}else{
				mPORTDClearBits(BIT_1);
			}
	
			//Discrete fourier transform
			freq[hist]=-1;
			maxfreqamp=0;
			for (j=0; j<16; j++){
				sinPart=0; cosPart=0;
				for (k = 0; k < BUFF_LEN; k++) {
					sinPart += (float)tempptr[k] * (float)(-1.0) * argsin[j][k];
					cosPart += (float)tempptr[k] * argcos[j][k];
				}
		
				magn500=abs((int)((float)BUFF_LEN *  (float)2.0 * sqrt(sinPart*sinPart + cosPart*cosPart) / (float)BUFF_LEN));
		
				if ((float)magn500>((float)magn*WIDTH/10.0) && (magn>THRESHOLD)){
					if (magn500>maxfreqamp){			//get most prominent frequency
						maxfreqamp=magn500;
						freq[hist]=j;
					}
				}
			}

			if (freq[hist]==-1) nearestF=0; else nearestF++;
	
			if (nearestF>=160){
				nearestF=0;
				spl=0;						//stop sampling and start analysis
				break;
			}

			if ((hist==0)||(hist==80)){
				tryEnterMgmt();
			}

			hist++;
			hist%=160;		//keep track of recent 160 samples
		}

		l=0;curr_freq=-1;accu=0;
		hist++;		//rewind hist ptr
		hist%=160;
		curr_freq=freq[hist];
		while (l<32){
			{						//long enough to be identified as part of a frequency series?
				accu=hist;
				len_freq=0;
				while ((curr_freq==freq[accu]) && (len_freq<=4)){accu++;accu%=160;len_freq++;}
				if (len_freq>=3){			//yes
					ref_freq[l]=freq[hist];
					hist+=(len_freq>4)?5:len_freq;
					l++;
				}else{						//not long enough
					hist++;
				}
			}
			hist%=160;
			curr_freq=freq[hist];
		}

		for (l=0; l<16; l++){		//squeeze the results
			ref_freq[l]=(ref_freq[l*2]<<4)+ref_freq[l*2+1];
			c_ref_freq[l]=ref_freq[l];
		}

		int correct=1;
		char user_name[64];
		UINT32 user_info;
		UINT32 user_cnt;
		memset(user_name, 0, sizeof(user_name));
 
		correct=search_dbase(user_name, &user_info, &user_cnt);

		if (correct>=0){
			if (user_info!=0){
				mPORTDSetBits(BIT_2);
				DelayMs2(1500);
				mPORTDClearBits(BIT_2);
				incr_user_cnt(correct);
			}else{
				mPORTDSetBits(BIT_0);
				DelayMs2(1500);
				mPORTDClearBits(BIT_0);
			}
			if (consolemode){
				char str[128];
				memset(str, 0, sizeof(str));
				unicode_to_ascii(user_name);
				if (user_info!=0){
					sprintf(str, "User Identified: %s, Info: %d, Count: %d => Access Granted", user_name, user_info, user_cnt);
				}else{
					sprintf(str, "User Identified: %s, Info: %d => Access Denied", user_name, user_info);
				}
				timeout=800;
				SendDataLong(0x55555555);
				if (GetDataLong()==0xFFFFFFFF) {consolemode=FALSE;timeout=3000;goto out;}
				if (!SendMessage(str)) consolemode=FALSE;
				timeout=3000;
			}
		}else{
			mPORTDSetBits(BIT_0);
			DelayMs2(500);
			mPORTDClearBits(BIT_0);
			if (consolemode){
				timeout=800;
				SendDataLong(0x55555555);
				if (GetDataLong()==0xFFFFFFFF) {consolemode=FALSE;timeout=3000;goto out;}
				if (!SendMessage("User Not Identified => Access Denied")) consolemode=FALSE;
				timeout=3000;
			}
		}
out:
		hist=0;nearestF=0;finished=1;recentsamp=sampbuffA;recentptr=0;
	}
	return 0;
}

void DelayMs2(unsigned int msec)
{
	unsigned int tWait, tStart;
		
    tWait=(SYS_FREQ/2000)*msec;
    tStart=ReadCoreTimer();
    while((ReadCoreTimer()-tStart)<tWait);		// wait for the time to pass
}

BOOL SendMessage(char* buffer){
	return SendDataBuffer(buffer, strlen(buffer));
}

// *****************************************************************************
// void UARTTxBuffer(char *buffer, UINT32 size)
// *****************************************************************************
BOOL SendDataBuffer(const char *buffer, UINT32 size)
{
	unsigned int tWait, tStart;
	SendDataLong(size);

    while(size)
    {
		tWait=(SYS_FREQ/2000)*2500;
    	tStart=ReadCoreTimer();
        while(!UARTTransmitterIsReady(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

        UARTSendDataByte(UART2, *buffer);

        buffer++;
        size--;
    }
	tWait=(SYS_FREQ/2000)*2500;
    tStart=ReadCoreTimer();
    while(!UARTTransmissionHasCompleted(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

	return TRUE;
}
// *****************************************************************************
// UINT32 GetDataBuffer(char *buffer, UINT32 max_size)
// *****************************************************************************
BOOL GetDataBuffer(char *buffer, UINT32 max_size)
{
	unsigned int tWait, tStart;
    UINT32 num_char;

    num_char = 0;

    while(num_char < max_size)
    {
        UINT8 character;

		tWait=(SYS_FREQ/2000)*2500;
    	tStart=ReadCoreTimer();
        while(!UARTReceivedDataIsAvailable(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

        character = UARTGetDataByte(UART2);

        *buffer = character;

        buffer++;
        num_char++;
    }

    return TRUE;
}

UINT32 GetDataLong()
{
	UINT32 tmp=0;
	UINT32 a,b,c,d;
	unsigned int tWait, tStart;

	tWait=(SYS_FREQ/2000)*timeout; tStart=ReadCoreTimer();
	while(!UARTReceivedDataIsAvailable(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return 0xFFFFFFFF;
	a = UARTGetDataByte(UART2);

	tWait=(SYS_FREQ/2000)*timeout; tStart=ReadCoreTimer();
	while(!UARTReceivedDataIsAvailable(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return 0xFFFFFFFF;
	b = UARTGetDataByte(UART2);

	tWait=(SYS_FREQ/2000)*timeout; tStart=ReadCoreTimer();
	while(!UARTReceivedDataIsAvailable(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return 0xFFFFFFFF;
	c = UARTGetDataByte(UART2);

	tWait=(SYS_FREQ/2000)*timeout; tStart=ReadCoreTimer();
	while(!UARTReceivedDataIsAvailable(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return 0xFFFFFFFF;
	d = UARTGetDataByte(UART2);

	tmp=a+(b<<8)+(c<<16)+(d<<24);
	timeout=2500;
	return tmp;
}

BOOL SendDataLong(UINT32 buffer)
{
	unsigned int tWait, tStart;
	UINT32 tmp=buffer;

	tWait=(SYS_FREQ/2000)*2500;tStart=ReadCoreTimer();
	while(!UARTTransmitterIsReady(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

    UARTSendDataByte(UART2, (tmp&0x000000FF));

	tWait=(SYS_FREQ/2000)*2500;tStart=ReadCoreTimer();
    while(!UARTTransmissionHasCompleted(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

	tWait=(SYS_FREQ/2000)*2500;tStart=ReadCoreTimer();
	while(!UARTTransmitterIsReady(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

    UARTSendDataByte(UART2, (tmp&0x0000FF00)>>8);

	tWait=(SYS_FREQ/2000)*2500;tStart=ReadCoreTimer();
    while(!UARTTransmissionHasCompleted(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

	tWait=(SYS_FREQ/2000)*2500;tStart=ReadCoreTimer();
	while(!UARTTransmitterIsReady(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

    UARTSendDataByte(UART2, (tmp&0x00FF0000)>>16);

	tWait=(SYS_FREQ/2000)*2500;tStart=ReadCoreTimer();
    while(!UARTTransmissionHasCompleted(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

	tWait=(SYS_FREQ/2000)*2500;tStart=ReadCoreTimer();
    while(!UARTTransmitterIsReady(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

    UARTSendDataByte(UART2, (tmp&0xFF000000)>>24);

	tWait=(SYS_FREQ/2000)*2500;tStart=ReadCoreTimer();
    while(!UARTTransmissionHasCompleted(UART2)) if ((ReadCoreTimer()-tStart)>tWait) return FALSE;

	return TRUE;
}
