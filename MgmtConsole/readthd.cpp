//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "readthd.h"
#include "main.h"
#pragma package(smart_init)

ULONG itmcount=0;

__fastcall readthread::readthread(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}

void __fastcall readthread::precreate(){
	mainForm->ProgressBar1->Style=pbstMarquee;
}

void __fastcall readthread::DonwloadData(){
	PDBDATA dbdata=(PDBDATA)(&message[4]);		//bypass the instruction data(4-byte ulong)
	if (dbdata->isdeleted==0) {
		itmcount++;
	}else{
		TListItem *Item = mainForm->ListView1->Items->Add();
		Item->Caption=itmcount;
		itmcount++;
		Item->SubItems->Append(UnicodeString((wchar_t*)dbdata->name));
		__int64 mess=0;
		memcpy(&mess, &dbdata->message[0], 8);
		int cnt=0;
		memcpy(&cnt, &dbdata->counter[0], 4);
		UnicodeString smess=IntToStr(mess);
		UnicodeString scnt=IntToStr(cnt);
		Item->SubItems->Append(smess);
		Item->SubItems->Append(scnt);
		Item->SubItems->Append(IntToStr((int)dbdata->idinfo));
	}
}

void __fastcall readthread::postDownload(){
	downloading=false;
	downloaded=true;
	mainForm->Memo1->Lines->Add("Downloading Finished");
	mainForm->Button1->Enabled=true;
	mainForm->Button2->Enabled=true;
	mainForm->Button3->Enabled=true;
	mainForm->Button7->Enabled=true;
	mainForm->Button6->Enabled=true;
}

void __fastcall readthread::UpdateData()
{
	mainForm->Memo1->Lines->Add("PIC: "+UnicodeString(message));
}

void __fastcall readthread::ExitMgmt()
{
	mainForm->Memo1->Lines->Add("Exit Management Mode");
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

void __fastcall readthread::ExitMgmt2()
{
	mainForm->Memo1->Lines->Add("Operation timed out");
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

void __fastcall readthread::EnableFire(){
	mainForm->Button6->Enabled=true;
	mainForm->Button1->Enabled=downloaded;
	mainForm->Button2->Enabled=downloaded;
	mainForm->Button3->Enabled=downloaded;
	mainForm->Button7->Enabled=true;
}

//---------------------------------------------------------------------------
void __fastcall readthread::Execute()
{
	Sleep(500);
	Synchronize(&precreate);
	ULONG retlen=0;
	ULONG conn=0xDDDDDDDD;
	while (1)
	{
		cyclecnt++;
		if (cyclecnt>=0x70000000) {          //prevent overflow
            cyclecnt=0;
		}
		if (current_command==SEND_DATA) {
			conn=CPU_SEND_DATA;
			current_command=DO_NOTHING;
			unsigned long tmp_size=current_size;

			if (!WriteFileAsn(commHandle, &conn, 4, &dwWrite, NULL)){Synchronize(&ExitMgmt2);return;}
			if (!WriteFileAsn(commHandle, &tmp_size, 4, &dwWrite, NULL)){Synchronize(&ExitMgmt2);return;}
			if (!WriteFileAsn(commHandle, message, tmp_size, &dwWrite, NULL)){Synchronize(&ExitMgmt2);return;}
			if (!ReadFileAsn(commHandle, &retlen, 4, &dwRead, NULL)){Synchronize(&ExitMgmt2);return;}			//response

			if (retlen==PIC_EXIT) {
				Synchronize(&ExitMgmt);
				return;
			}else{
				ULONG code=*(ULONG*)(&message[0]);
				if (code!=PIC_DOWNLOAD && !downloading) {
                    Synchronize(&EnableFire);
				}
			}
			if (retlen==PIC_DOWNLOAD_FIN) {
				Synchronize(&postDownload);
				continue;
			}
			if (retlen!=PIC_SEND_DATA) {
            	//just dummy
			}else{
				if (retlen==PIC_SEND_DATA) {                    			//with message
					memset(message, 0,sizeof(message));
					if (!ReadFileAsn(commHandle, &retlen, 4, &dwRead, NULL)){Synchronize(&ExitMgmt2);return;}		//read message length
					if (!ReadFileAsn(commHandle, message, retlen, &dwRead, NULL)){Synchronize(&ExitMgmt2);return;}
					ULONG code=*(ULONG*)(&message[0]);
					switch (code) {
					case PIC_DOWNLOAD:
						Synchronize(&DonwloadData);
					break;
					default:
						Synchronize(&UpdateData);
					}
				}else{
					//just dummy
				}
			}
		}else{
			//do nothing, try and get PIC data if present
			conn=CPU_SEND_DUMMY;
			if (!WriteFileAsn(commHandle, &conn, 4, &dwWrite, NULL)){Synchronize(&ExitMgmt2);return;}
			if (!ReadFileAsn(commHandle, &retlen, 4, &dwRead, NULL)){Synchronize(&ExitMgmt2);return;}			//response
			if (retlen==PIC_EXIT) {
				Synchronize(&ExitMgmt);
				return;
			}
			if (retlen==PIC_DOWNLOAD_FIN) {
				Synchronize(&postDownload);
				continue;
			}
			if (retlen==PIC_SEND_DATA) {                    			//with message
				memset(message, 0,sizeof(message));
				if (!ReadFileAsn(commHandle, &retlen, 4, &dwRead, NULL)){Synchronize(&ExitMgmt2);return;}		//read message length
				if (!ReadFileAsn(commHandle, message, retlen, &dwRead, NULL)){Synchronize(&ExitMgmt2);return;}
				ULONG code=*(ULONG*)(&message[0]);
				switch (code) {
				case PIC_DOWNLOAD:
					Synchronize(&DonwloadData);
				break;
                default:
                    Synchronize(&UpdateData);
				}
			}else{
				//just dummy
            }
		}
	}
}
//---------------------------------------------------------------------------
