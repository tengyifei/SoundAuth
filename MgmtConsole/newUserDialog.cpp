//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "newUserDialog.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TnewUserDlg *newUserDlg;
//---------------------------------------------------------------------------
__fastcall TnewUserDlg::TnewUserDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TnewUserDlg::Button2Click(TObject *Sender)
{
	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TnewUserDlg::Button1Click(TObject *Sender)
{
	memcpy(dbdatastr.name, Edit1->Text.c_str(), 64);
	__int64 mess=StrToInt64(Edit2->Text);
	memcpy(dbdatastr.message, &mess, 8);
	int cnt=StrToInt(Edit3->Text);
	memcpy(dbdatastr.counter, &cnt, 4);
	int ifo=StrToInt(Edit4->Text);
	dbdatastr.idinfo=ifo;
	dbdatastr.isdeleted=1;
	mainForm->oneTimeTimer2->Enabled=true;
	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TnewUserDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	mainForm->Enabled=true;
}
//---------------------------------------------------------------------------
