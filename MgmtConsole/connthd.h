//---------------------------------------------------------------------------

#ifndef connthdH
#define connthdH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class connthread : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall connthread(bool CreateSuspended);
};
class connthread2 : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall connthread2(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
