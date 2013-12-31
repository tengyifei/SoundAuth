//---------------------------------------------------------------------------

#ifndef readthdH
#define readthdH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class readthread : public TThread
{
private:
protected:
	void __fastcall Execute();
	void __fastcall UpdateData();
	void __fastcall ExitMgmt();
	void __fastcall ExitMgmt2();
	void __fastcall EnableFire();
	void __fastcall precreate();
	void __fastcall DonwloadData();
	void __fastcall postDownload();
public:
	__fastcall readthread(bool CreateSuspended);
};

extern ULONG itmcount;

//---------------------------------------------------------------------------
#endif
