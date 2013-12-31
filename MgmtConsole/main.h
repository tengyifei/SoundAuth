//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Mask.hpp>
#include <ExtCtrls.hpp>
#include <IdBaseComponent.hpp>
#include <IdSASL.hpp>
#include <IdSASL_CRAM_SHA1.hpp>
#include <IdSASL_CRAMBase.hpp>
#include <IdSASLUserPass.hpp>
#include <IdAntiFreeze.hpp>
#include <IdAntiFreezeBase.hpp>
#include <ButtonGroup.hpp>
#include <ActnList.hpp>
#include <CategoryButtons.hpp>
#include <ActnPopup.hpp>
#include <Menus.hpp>
#include <PlatformDefaultStyleActnCtrls.hpp>
//---------------------------------------------------------------------------
class TmainForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TListView *ListView1;
	TLabel *Label1;
	TLabel *Label2;
	TComboBox *ComboBox1;
	TEdit *Edit1;
	TButton *Button1;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	TMemo *Memo1;
	TButton *Button5;
	TProgressBar *ProgressBar1;
	TTimer *Timer2;
	TLabel *Label3;
	TLabel *Label4;
	TButton *Button6;
	TButton *Button7;
	TTimer *oneTimeTimer;
	TButton *Button8;
	TButton *Button9;
	TTimer *oneTimeTimer2;
	TActionList *ActionList1;
	TPopupActionBar *PABar1;
	TMenuItem *A1;
	TMenuItem *AsConsole1;
	TAction *Action1;
	TAction *Action2;
	TAction *Action3;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Timer2Timer(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);
	void __fastcall Button9Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall oneTimeTimer2Timer(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button4MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall A1Click(TObject *Sender);
	void __fastcall AsConsole1Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TmainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
#define SEND_DATA 0x13323444
#define DO_NOTHING 0xF9C1F9C1
//---------------------------------------------------------------------------

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
#define CPU_DOWNLOAD		0xFF583B99
#define CPU_INSERT			0xFF912E55
#define CPU_DELETE			0xFF348D12
#define CPU_EDIT		0xFF7142BF

#define PIC_DOWNLOAD		0xFF71D1AA
#define PIC_DOWNLOAD_FIN	0xFFC1C2C3

/*	Communication Protocol	*/

/*	Data Storage
1st sector on SD card: database information
2-nth sector: user database
4 users per sector
Name(64bytes)+Data(8bytes)+Counter(4bytes)+Identification Info(4bytes)+deleted?(4bytes)
*/
typedef struct DBHeaderTag{
	UINT32 dbmagic;		//should always be 0x7968574D, used in checking for validity
	char pwhash[32];	//sha-1 hash of db password. size is not 20 but 32 for alignment
	UINT32 num_user;
	UINT32 num_user_deleted;
	UINT32 reserved1;
	UINT32 reserved2;
	UINT32 dbtailmagic;	//should always be 0xE59B230C, used in checking for validity
}DBHeader, *PDBHeader;

typedef struct DBDataTag{
	char name[64];
	char message[8];
	char counter[4];
	UINT32 idinfo;
	UINT32 isdeleted;		//0=>deleted, 1=>present
}DBData, *PDBDATA;
/*Data Storage*/

extern PACKAGE TmainForm *mainForm;
extern HANDLE commHandle;
extern DWORD dwWrite;
extern DWORD dwRead;
extern char message[8192];
extern long current_command;
extern unsigned long current_size;
extern long cyclecnt;

extern DCB dcb;
extern LPOVERLAPPED theOverlapped;

extern bool downloaded;
extern bool downloading;

extern DBData dbdatastr;
extern bool shoulddie;

bool WriteFileAsn(HANDLE commHandle, void* data, long num, DWORD* reserved, void* reserved2, volatile bool* shoulddie=NULL);
bool ReadFileAsn(HANDLE commHandle, void* data, long num, DWORD* reserved, void* reserved2, volatile bool* shoulddie=NULL);

#endif
