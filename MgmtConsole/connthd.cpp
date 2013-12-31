//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "connthd.h"
#include "main.h"
#include "readthd.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall connthread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

extern readthread* rdthd;

__fastcall connthread::connthread(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall connthread::Execute()
{
	char charconn='w';
	unsigned long response=0;

	int a=mainForm->ComboBox1->ItemIndex;
	UnicodeString aa=mainForm->ComboBox1->Items->operator [](a);

	if (aa[aa.Length()-1]=='\n') {
        aa=aa.SubString(0, aa.Length()-2);
	}

	UnicodeString portn=L"\\\\.\\"+aa;

	if (commHandle!=NULL) {
		CloseHandle(commHandle);
		commHandle=NULL;
	}

	if (theOverlapped->hEvent!=NULL) {
		CloseHandle(theOverlapped->hEvent);
	}
	theOverlapped->hEvent=CreateEvent(NULL, false, false, NULL);

	commHandle=
	CreateFile(
		portn.c_str(),
		(GENERIC_READ | GENERIC_WRITE),
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
	);
	if (commHandle!=INVALID_HANDLE_VALUE && commHandle!=NULL) {
		memset(&dcb, 0,sizeof(DCB));
		dcb.DCBlength = sizeof(DCB);
		GetCommState(commHandle, &dcb);
		dcb.BaudRate = CBR_115200;    //  baud rate
		dcb.ByteSize = 8;             //  data size, xmit and rcv
		dcb.Parity   = NOPARITY;      //  parity bit
		dcb.StopBits = ONESTOPBIT;    //  stop bit
		SetCommState(commHandle, &dcb);
		SetupComm(commHandle, 8192, 8192);
		if (!WriteFileAsn(commHandle, &charconn, 1, &dwWrite, NULL)){
			ShowMessage("Connection Failed...");mainForm->Button4->Enabled=true;return;
		}
		ReadFileAsn(commHandle, &response, 4, &dwRead, NULL);
		if (response==PIC_SEND_DUMMY) {		//success?
			rdthd=new readthread(true);
			rdthd->Start();               	//spawn data exchange cycle
		}else{
			ShowMessage("Connection Failed...");mainForm->Button4->Enabled=true;return;
		}
	}else{
		ShowMessage("Connection Failed...");mainForm->Button4->Enabled=true;return;
	}

	Sleep(1400);

	mainForm->Button5->Enabled=true;
	mainForm->ProgressBar1->State=pbsNormal;
	mainForm->Button6->Enabled=true;
	mainForm->Button7->Enabled=true;

	mainForm->Button7Click(mainForm);
}
//---------------------------------------------------------------------------

__fastcall connthread2::connthread2(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall connthread2::Execute()
{
	char charconn='c';
	unsigned long response=0;

	int a=mainForm->ComboBox1->ItemIndex;
	UnicodeString aa=mainForm->ComboBox1->Items->operator [](a);

	if (aa[aa.Length()-1]=='\n') {
        aa=aa.SubString(0, aa.Length()-2);
	}

	UnicodeString portn=L"\\\\.\\"+aa;

	if (commHandle!=NULL) {
		CloseHandle(commHandle);
		commHandle=NULL;
	}

	if (theOverlapped->hEvent!=NULL) {
		CloseHandle(theOverlapped->hEvent);
	}
	theOverlapped->hEvent=CreateEvent(NULL, false, false, NULL);

	commHandle=
	CreateFile(
		portn.c_str(),
		(GENERIC_READ | GENERIC_WRITE),
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
	);
	if (commHandle!=INVALID_HANDLE_VALUE && commHandle!=NULL) {
		memset(&dcb, 0,sizeof(DCB));
		dcb.DCBlength = sizeof(DCB);
		GetCommState(commHandle, &dcb);
		dcb.BaudRate = CBR_115200;    //  baud rate
		dcb.ByteSize = 8;             //  data size, xmit and rcv
		dcb.Parity   = NOPARITY;      //  parity bit
		dcb.StopBits = ONESTOPBIT;    //  stop bit
		SetCommState(commHandle, &dcb);
		SetupComm(commHandle, 8192, 8192);
		if (!WriteFileAsn(commHandle, &charconn, 1, &dwWrite, NULL)){
			ShowMessage("Connection Failed...");mainForm->Button4->Enabled=true;return;
		}
		ReadFileAsn(commHandle, &response, 4, &dwRead, NULL);
		if (response==PIC_SEND_DUMMY) {		//success?
			mainForm->Memo1->Lines->Add("Connected");
		}else{
			ShowMessage("Connection Failed...");mainForm->Button4->Enabled=true;return;
		}
	}else{
		ShowMessage("Connection Failed...");mainForm->Button4->Enabled=true;return;
	}

	Sleep(100);
	mainForm->Button5->Enabled=true;
	mainForm->ProgressBar1->State=pbsNormal;
	while (1){
		if (shoulddie) {
			break;
		}
		DWORD retlen=0, tmp=111;
		ReadFileAsn(commHandle, &retlen, 4, NULL, NULL, &shoulddie);
		WriteFileAsn(commHandle, &tmp, 4, NULL, NULL, &shoulddie);
		ReadFileAsn(commHandle, &retlen, 4, NULL, NULL, &shoulddie);
		if (shoulddie) {
			break;
		}
		memset(message, 0, sizeof(message));
		while (!ReadFileAsn(commHandle, message, retlen, NULL, NULL)){
			if (shoulddie) {
				break;
			}
		}
		mainForm->Memo1->Lines->Add(message);
	}
	mainForm->Memo1->Lines->Add("Terminated");
}
