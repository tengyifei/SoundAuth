//---------------------------------------------------------------------------

#ifndef newUserDialogH
#define newUserDialogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TnewUserDlg : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TButton *Button2;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
	__fastcall TnewUserDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TnewUserDlg *newUserDlg;
//---------------------------------------------------------------------------
#endif
