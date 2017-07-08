//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "Unit3.h"
//---------------------------------------------------------------------------
class Exec : public TThread
{
private:
	HANDLE hChildProcess;
	AnsiString cmd;
	AnsiString cmdInfo;
	AnsiString cmdInfo_c1;
	AnsiString cmdInfo_c2;
	AnsiString cmdInfo_c3;
	AnsiString cmdInfo_c4;
	AnsiString cmdInfo_c5;
	AnsiString cmdInfo_c6;
	AnsiString cmdInfo_usr;
	AnsiString wkFile;
	Exec_c* threadExec_c1;
	Exec_c* threadExec_c2;
	Exec_c* threadExec_c3;
	Exec_c* threadExec_c4;
	Exec_c* threadExec_c5;
	Exec_c* threadExec_c6;
	int fileIndex;
	int r1_index;
	int r2_index;
	int cmdIndex;
	int persent;
	int persent_c1;
	int persent_c2;
	int persent_c3;
	int persent_c4;
	int persent_c5;
	int persent_c6;
	void __fastcall HandleProcess(void);
	BOOL __fastcall RunChildProcess(HANDLE hStdout,HANDLE hStdErr);
	void __fastcall UpdateProgress(void);
	void __fastcall UpdateProgress_c1(void);
	void __fastcall UpdateProgress_c2(void);
	void __fastcall FinishProcess(void);
	void __fastcall UpdateProgress_usr(void);
	void __fastcall FinishProcess_usr(void);
protected:
	void __fastcall Execute();
public:
	double progress;
	AnsiString cmd_d;	// デコード
	AnsiString cmd_r;	// wav2raw
	AnsiString cmd_u1;	// upconv
	AnsiString cmd_u2;	// upconv
	AnsiString cmd_u3;	// upconv
	AnsiString cmd_u4;	// upconv;
	AnsiString cmd_u5;	// upconv;
	AnsiString cmd_u6;	// upconv;
	AnsiString cmd_w;	// raw2wav
	AnsiString cmd_e;	// エンコード
	AnsiString cmd_snd;	// sndfile convert
	AnsiString cmd_usr;	// ユーザーコマンド
	AnsiString cmd_mc;	// マルチチャンネル
	AnsiString toFile;	// 出力ファイル名
	AnsiString errFile;	// エラーファイル
	int thread;
	__fastcall Exec(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
 