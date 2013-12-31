//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "readthd.h"
#include "connthd.h"
#include "newUserDialog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TmainForm *mainForm;
HANDLE commHandle=NULL;
DWORD dwWrite=0;
DWORD dwRead=0;
DCB dcb;
readthread* rdthd=NULL;
connthread* cothd=NULL;
connthread2* cothd2=NULL;
char message[8192];
long current_command;
unsigned long current_size;
long cyclecnt=0;

OVERLAPPED overlappedst;
LPOVERLAPPED theOverlapped;

bool downloaded=false;
bool downloading=false;
bool shoulddie=false;

DBData dbdatastr;
bool edit=false;

//---------------------------------------------------------------------------
__fastcall TmainForm::TmainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::FormCreate(TObject *Sender)
{
	HKEY hKey;
	int rtn;
	rtn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"Hardware\\DeviceMap\\SerialComm", NULL, KEY_READ, &hKey);
	if( rtn == ERROR_SUCCESS)
	{
		int  i=0;
		wchar_t  portName[256],commName[256];
		DWORD  dwLong,dwSize;
		while(1)
		{
			dwSize =  sizeof(portName);
			dwLong  =  dwSize;
			rtn = RegEnumValue( hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize );
			if( rtn == ERROR_NO_MORE_ITEMS )
				break;
			this->ComboBox1->Items->Add(commName);
			i++;
		}
		RegCloseKey(hKey);
	}
	if (ComboBox1->Items->Count>0) {
		ComboBox1->ItemIndex=0;
	}
	theOverlapped=&overlappedst;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::Button5Click(TObject *Sender)
{
	ListView1->Items->Clear();
	Button5->Enabled=false;
	unsigned long senddata=CPU_EXIT;
    memcpy(message, &senddata, 4);
	current_size=4;
	current_command=SEND_DATA;
	Button4->Enabled=true;
	ProgressBar1->State=pbsPaused;
	ProgressBar1->Position=0;
	Button6->Enabled=false;
	Button7->Enabled=false;
	if ((cothd2!=NULL)) {
		try{
			shoulddie=true;
			cothd2->WaitFor();
			cothd2->Terminate();
			delete cothd2;
			cothd2=NULL;
		}catch(...){
		}
	}
	if ((rdthd!=NULL)) {
		try{
			rdthd->WaitFor();
			rdthd->Terminate();
			delete rdthd;
			rdthd=NULL;
		}catch(...){
		}
	}
	GroupBox1->Visible=true;
	Memo1->Top=160;
	Memo1->Left=519;
	Memo1->Height=386;
	Memo1->Width=253;
	mainForm->Button4->Enabled=true;
	mainForm->ProgressBar1->State=pbsPaused;
	mainForm->ProgressBar1->Position=0;
	mainForm->Button6->Enabled=false;
	mainForm->Button5->Enabled=false;
	mainForm->Button7->Enabled=false;
	mainForm->Button1->Enabled=false;
	mainForm->Button2->Enabled=false;
	mainForm->Button3->Enabled=false;
	downloaded=false;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::Timer2Timer(TObject *Sender)
{
	Label4->Caption=cyclecnt;
	cyclecnt=0;
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::Button6Click(TObject *Sender)
{
	Button6->Enabled=false;
	Button7->Enabled=false;
	Button1->Enabled=false;
	Button2->Enabled=false;
	Button3->Enabled=false;
	char *msg="Hello!PADAAAAAAAHello!PADAAAAAAAHello!PADAAAAAAAHello!PADAAAAAAAHello!PADAAAAAAAHello!PADAAAAAAAHello!PADAAAAAAAHello!PADAAAAAAA\0";
	memcpy(message, msg, 129);
	for (int i = 0; i < 10; i++) {
		strcat(message, msg);
	}
	current_size=128*11+1;
	current_command=SEND_DATA;
}
//---------------------------------------------------------------------------

bool WriteFileAsn(HANDLE commHandle, void* data, long num, DWORD* reserved, void* reserved2, volatile bool* shoulddie){
	int timeout=2500;
	if (shoulddie!=NULL) {
        timeout=900;
	}
	WriteFile(commHandle, data, num, NULL, theOverlapped);
back1:
	if (WaitForSingleObject(theOverlapped->hEvent, timeout)==WAIT_TIMEOUT){
		if (shoulddie!=NULL && !(*shoulddie)) {
			goto back1;
		}
		return false;
	}else{
		return true;
    }
}

bool ReadFileAsn(HANDLE commHandle, void* data, long num, DWORD* reserved, void* reserved2, volatile bool* shoulddie){
	int timeout=2500;
	if (shoulddie!=NULL) {
        timeout=900;
	}
	ReadFile(commHandle, data, num, NULL, theOverlapped);
back2:
	if (WaitForSingleObject(theOverlapped->hEvent, timeout)==WAIT_TIMEOUT){
		if (shoulddie!=NULL && !(*shoulddie)) {
            goto back2;
		}
		return false;
	}else{
		return true;
    }
}

void __fastcall TmainForm::Button7Click(TObject *Sender)
{
	itmcount=0;
	ListView1->Items->Clear();
	Button7->Enabled=false;
	Button6->Enabled=false;
	Button1->Enabled=false;
	Button2->Enabled=false;
	Button3->Enabled=false;
	Sleep(100);
	downloading=true;
	ULONG msg=CPU_DOWNLOAD;
	memcpy(message, &msg, 4);
	current_size=8;
	current_command=SEND_DATA;
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::Button9Click(TObject *Sender)
{
	Memo1->Lines->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::Button1Click(TObject *Sender)
{
	edit=false;
	newUserDlg=new TnewUserDlg(this);
	newUserDlg->Show();
	this->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::oneTimeTimer2Timer(TObject *Sender)
{
	if (edit) {
		Button7->Enabled=false;
		Button6->Enabled=false;
		Button1->Enabled=false;
		Button2->Enabled=false;
		Button3->Enabled=false;
		oneTimeTimer2->Enabled=false;
		Memo1->Lines->Add("Editing User");
		ULONG msg=CPU_EDIT;
		ULONG idx=ListView1->Selected->Caption.ToInt();
		memset(message, 0, sizeof(message));
		memcpy(message, &msg, 4);
		memcpy(message+4, &idx, 4);
		memcpy(message+8, &dbdatastr, sizeof(DBData));
		current_size=132;
		current_command=SEND_DATA;

		mainForm->ListView1->Items->Delete(idx);
		TListItem *Item = mainForm->ListView1->Items->Insert(idx);
		Item->Caption=idx;
		Item->SubItems->Append(UnicodeString((wchar_t*)dbdatastr.name));
		__int64 mess=0;
		memcpy(&mess, &dbdatastr.message[0], 8);
		int cnt=0;
		memcpy(&cnt, &dbdatastr.counter[0], 4);
		UnicodeString smess=IntToStr(mess);
		UnicodeString scnt=IntToStr(cnt);
		Item->SubItems->Append(smess);
		Item->SubItems->Append(scnt);
		Item->SubItems->Append(IntToStr((int)dbdatastr.idinfo));
	}else{
		Button7->Enabled=false;
		Button6->Enabled=false;
		Button1->Enabled=false;
		Button2->Enabled=false;
		Button3->Enabled=false;
		oneTimeTimer2->Enabled=false;
		Memo1->Lines->Add("Creating New User");
		ULONG msg=CPU_INSERT;
		memset(message, 0, sizeof(message));
		memcpy(message, &msg, 4);
		memcpy(message+4, &dbdatastr, sizeof(DBData));
		current_size=128;
		current_command=SEND_DATA;

		TListItem *Item = mainForm->ListView1->Items->Add();
		Item->Caption=itmcount;
		itmcount++;
		Item->SubItems->Append(UnicodeString((wchar_t*)dbdatastr.name));
		__int64 mess=0;
		memcpy(&mess, &dbdatastr.message[0], 8);
		int cnt=0;
		memcpy(&cnt, &dbdatastr.counter[0], 4);
		UnicodeString smess=IntToStr(mess);
		UnicodeString scnt=IntToStr(cnt);
		Item->SubItems->Append(smess);
		Item->SubItems->Append(scnt);
		Item->SubItems->Append(IntToStr((int)dbdatastr.idinfo));
	}
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::Button2Click(TObject *Sender)
{
	if (ListView1->Selected==NULL) {
		ShowMessage("Please select a user!");
		return;
	}
	ULONG msg=CPU_DELETE;
	ULONG idx=ListView1->Selected->Caption.ToInt();
	memcpy(message, &msg, 4);
	memcpy(&message[4], &idx, 4);
	current_size=8;
	current_command=SEND_DATA;
	Button2->Enabled=false;
}
//---------------------------------------------------------------------------


void __fastcall TmainForm::Button4MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	PABar1->Popup(Left+GroupBox2->Left+Button4->Left+X+10, Top+GroupBox2->Top+Button4->Top+Y+30);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::A1Click(TObject *Sender)
{
	GroupBox1->Visible=true;
	Memo1->Top=160;
	Memo1->Left=519;
	Memo1->Height=386;
	Memo1->Width=253;
	ListView1->Items->Clear();
	if (mainForm->ComboBox1->ItemIndex==-1) {
		ShowMessage("Please Select a port!");
	}else{
		Button4->Enabled=false;
		Memo1->Lines->Clear();
		Memo1->Lines->Add("Connecting...");
		cothd=new connthread(false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::AsConsole1Click(TObject *Sender)
{
	shoulddie=false;
	GroupBox1->Visible=false;
	Memo1->Top=8;
	Memo1->Left=8;
	Memo1->Height=564;
	Memo1->Width=505;
	ListView1->Items->Clear();
	if (mainForm->ComboBox1->ItemIndex==-1) {
		ShowMessage("Please Select a port!");
	}else{
		Button4->Enabled=false;
		Memo1->Lines->Clear();
		Memo1->Lines->Add("Connecting...");
		cothd2=new connthread2(false);
	}
}
//---------------------------------------------------------------------------


void __fastcall TmainForm::Button3Click(TObject *Sender)
{
	edit=true;
	newUserDlg=new TnewUserDlg(this);
	newUserDlg->Show();
	this->Enabled=false;
}
//---------------------------------------------------------------------------


