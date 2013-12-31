#ifndef newUserDlgH
#define newUserDlgH
//----------------------------------------------------------------------------
#include <OKCANCL1.h>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <StdCtrls.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <SysUtils.hpp>
#include <Windows.hpp>
#include <System.hpp>
//----------------------------------------------------------------------------
class TNewUserDlg : public TOKBottomDlg
{
__published:
private:
public:
	virtual __fastcall TNewUserDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TNewUserDlg *NewUserDlg;
//----------------------------------------------------------------------------
#endif    
