//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class Exec_c : public TThread
{
private:
	HANDLE hChildProcess;
	void __fastcall HandleProcess(void);
	BOOL __fastcall RunChildProcess(HANDLE hStdout,HANDLE hStdErr);
protected:
	void __fastcall Execute();
public:
	AnsiString cmdInfo;
	int persent;
	AnsiString cmd;	// デコード
	__fastcall Exec_c(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
