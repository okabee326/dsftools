//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "Unit2.h"
#include "Unit1.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------

// 注意：異なるスレッドが所有する VCL のメソッド/関数/プロパティを
//		 別のスレッドの中から扱う場合，排他処理の問題が発生します。
//
//		 メインスレッドの所有するオブジェクトに対しては Synchronize
//		 メソッドを使う事ができます。他のオブジェクトを参照するため
//		 のメソッドをスレッドクラスに追加し，Synchronize メソッドの
//		 引数として渡します。
//
//		 たとえば UpdateCaption を以下のように定義し、
//
//		void __fastcall Exec::UpdateCaption()
//		{
//		  Form1->Caption = "スレッドから書き換えました";
//		}
//
//		 Execute メソッドの中で Synchronize メソッドに渡すことでメイ
//		 ンスレッドが所有する Form1 の Caption プロパティを安全に変
//		 更できます。
//
//		Synchronize(UpdateCaption);
//
//---------------------------------------------------------------------------

__fastcall Exec::Exec(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall Exec::Execute()
{
	FILE *fp;
	char fullpath[_MAX_PATH];
	char buf[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[_MAX_FNAME];
	char ext[_MAX_EXT];
	char *p;
	int i;
	int r1_busy = false;
	int r2_busy = false;
	int r3_busy = false;
	int r4_busy = false;
	int r5_busy = false;
	int r6_busy = false;
	threadExec_c1 = NULL;
	threadExec_c2 = NULL;
	threadExec_c3 = NULL;
	threadExec_c4 = NULL;
	threadExec_c5 = NULL;
	threadExec_c6 = NULL;
	int sum_cpu;

	int end;

	if (cmd_usr != "") {
		cmd = cmd_usr;
		HandleProcess();
		Synchronize(FinishProcess_usr);
		return;
	}
	cmdIndex = 0;
	sum_cpu  = 0;

	do {
		switch (cmdIndex) {
			case 0:		// デコード
				cmdIndex = 0;
				if (cmd_d != "") {
					cmdInfo = "Decoding...";
					persent = -1;
					cmd = cmd_d;
					break;
				}
			case 1:		// wav2raw
				cmdIndex = 1;
				if (cmd_r != "") {
					cmdInfo = "wav2raw...";
					persent = -1;
					cmd = cmd_r;
					break;
				}
			case 2:		// upconv
				cmdIndex = 2;
				cmdInfo = "";
				persent = 0;

				for (end = false;end == false;) {
					// r1
					if (r1_busy == false) {
						if (cmd_u1 != "" && sum_cpu < thread) {
							threadExec_c1 = new Exec_c(true);
							threadExec_c1->FreeOnTerminate = false;
							threadExec_c1->cmd = cmd_u1;
							r1_busy = true;
							threadExec_c1->Start();
							cmd_u1 = "";
							sum_cpu++;
						}
					} else {
						if (threadExec_c1->Finished == false) {
							cmdInfo_c1 = threadExec_c1->cmdInfo;
							persent_c1 = threadExec_c1->persent;
							Synchronize(UpdateProgress_c1);
							Sleep(500);
						} else {
							// スレッドが終了
							delete threadExec_c1;
							r1_busy = false;
							sum_cpu--;
						}
					}
					// r2
					if (r2_busy == false) {
						if (cmd_u2 != "" && sum_cpu < thread) {
							threadExec_c2 = new Exec_c(true);
							threadExec_c2->FreeOnTerminate = false;
							threadExec_c2->cmd = cmd_u2;
							r2_busy = true;
							threadExec_c2->Start();
							cmd_u2 = "";
							sum_cpu++;
						}
					} else {
						if (threadExec_c2->Finished == false) {
							cmdInfo_c2 = threadExec_c2->cmdInfo;
							persent_c2 = threadExec_c2->persent;
							Synchronize(UpdateProgress_c2);
							Sleep(500);
						} else {
							// スレッドが終了
							delete threadExec_c2;
							r2_busy = false;
							sum_cpu--;
						}
					}
					// r3
					if (r3_busy == false) {
						if (cmd_u3 != "" && sum_cpu < thread) {
							threadExec_c3 = new Exec_c(true);
							threadExec_c3->FreeOnTerminate = false;
							threadExec_c3->cmd = cmd_u3;
							r3_busy = true;
							threadExec_c3->Start();
							cmd_u3 = "";
							sum_cpu++;
						}
					} else {
						if (threadExec_c3->Finished == false) {
							cmdInfo_c3 = threadExec_c3->cmdInfo;
							persent_c3 = threadExec_c3->persent;
							Synchronize(UpdateProgress_c2);
							Sleep(500);
						} else {
							// スレッドが終了
							delete threadExec_c3;
							r3_busy = false;
							sum_cpu--;
						}
					}
					// r4
					if (r4_busy == false) {
						if (cmd_u4 != "" && sum_cpu < thread) {
							threadExec_c4 = new Exec_c(true);
							threadExec_c4->FreeOnTerminate = false;
							threadExec_c4->cmd = cmd_u4;
							r4_busy = true;
							threadExec_c4->Start();
							cmd_u4 = "";
						}
					} else {
						if (threadExec_c4->Finished == false) {
							cmdInfo_c4 = threadExec_c4->cmdInfo;
							persent_c4 = threadExec_c4->persent;
							Synchronize(UpdateProgress_c2);
							Sleep(500);
						} else {
							// スレッドが終了
							delete threadExec_c4;
							r4_busy = false;
							sum_cpu--;
						}
					}
					// r5
					if (r5_busy == false) {
						if (cmd_u5 != "" && sum_cpu < thread) {
							threadExec_c5 = new Exec_c(true);
							threadExec_c5->FreeOnTerminate = false;
							threadExec_c5->cmd = cmd_u5;
							r5_busy = true;
							threadExec_c5->Start();
							cmd_u5 = "";
						}
					} else {
						if (threadExec_c5->Finished == false) {
							cmdInfo_c5 = threadExec_c5->cmdInfo;
							persent_c5 = threadExec_c5->persent;
							Synchronize(UpdateProgress_c2);
							Sleep(500);
						} else {
							// スレッドが終了
							delete threadExec_c5;
							r5_busy = false;
							sum_cpu--;
						}
					}
					// r6
					if (r6_busy == false) {
						if (cmd_u6 != "" && sum_cpu < thread) {
							threadExec_c6 = new Exec_c(true);
							threadExec_c6->FreeOnTerminate = false;
							threadExec_c6->cmd = cmd_u6;
							r6_busy = true;
							threadExec_c6->Start();
							cmd_u6 = "";
						}
					} else {
						if (threadExec_c6->Finished == false) {
							cmdInfo_c6 = threadExec_c6->cmdInfo;
							persent_c6 = threadExec_c6->persent;
							Synchronize(UpdateProgress_c2);
							Sleep(500);
						} else {
							// スレッドが終了
							delete threadExec_c6;
							r6_busy = false;
							sum_cpu--;
						}
					}
					if (r1_busy == false && r2_busy == false && r3_busy == false && r4_busy == false && r5_busy == false && r6_busy == false
						&& cmd_u1 == "" && cmd_u2 == "" && cmd_u3 == "" && cmd_u4 == "" && cmd_u5 == "" && cmd_u6 == "") {
						end = true;
						break;
					}
					if (Terminated) {
						break;
					}
					// エラーがあれば err ファイルが作成される
					if (FileExists(errFile)) {
						break;
					}
				}
			case 3:
				cmdIndex = 3;
				if (r1_busy == true) {
					threadExec_c1->Terminate();
					for (;;) {
						if (threadExec_c1->Finished == true) {
							break;
						}
					}
				}
				if (r2_busy == true) {
					threadExec_c2->Terminate();
					for (;;) {
						if (threadExec_c2->Finished == true) {
							break;
						}
					}
				}
				if (r3_busy == true) {
					threadExec_c3->Terminate();
					for (;;) {
						if (threadExec_c3->Finished == true) {
							break;
						}
					}
				}
				if (r4_busy == true) {
					threadExec_c4->Terminate();
					for (;;) {
						if (threadExec_c4->Finished == true) {
							break;
						}
					}
				}
				if (r5_busy == true) {
					threadExec_c5->Terminate();
					for (;;) {
						if (threadExec_c5->Finished == true) {
							break;
						}
					}
				}
				if (r6_busy == true) {
					threadExec_c6->Terminate();
					for (;;) {
						if (threadExec_c6->Finished == true) {
							break;
						}
					}
				}
				if (Terminated) {
					break;
				}
				// エラーがあれば err ファイルが作成される
				if (FileExists(errFile)) {
                    cmdIndex = 100;
					unlink(toFile.c_str());
					break;
				}
			case 4:
				cmdIndex = 4;
				fileIndex = 0;
				cmdInfo_c2 = "";
				persent_c2 = 0;
				Synchronize(UpdateProgress_c2);

			case 5:		// multConv
				if (cmd_mc != "") {
					cmdIndex = 5;
					cmdInfo.printf("mult conv...");
					persent = -1;
					cmd = cmd_mc;
					break;
				}
			case 6:		// raw2wav
				cmdIndex = 6;
				cmdInfo.printf("raw2wav...");
				persent = -1;

				// 処理すべきファイルがある
				if (cmd_w != "") {
					cmd = cmd_w;
					break;
				}
			case 7:		// エンコード
				cmdIndex = 7;
				if (cmd_e != "") {
					cmdInfo = "Encoding...";
					persent = -1;
					cmd = cmd_e;
					break;
				}
			case 8:
				cmdIndex = 8;
				if (cmd_snd != "") {
					cmdInfo = "W64...";
					persent = -1;
					cmd = cmd_snd;
					break;
				}
			default:
				// 終わり
				cmdIndex = 100;
				break;
		}
		if (cmdIndex < 100 && Terminated == false) {
			Synchronize(UpdateProgress);
			HandleProcess();
			cmdIndex++;
			// エラーがあれば err ファイルが作成される
			if (FileExists(errFile)) {
				cmdIndex = 100;
				unlink(toFile.c_str());
			}
		}
	} while (cmdIndex < 100 && Terminated == false);

	// 後始末
	_splitpath(toFile.c_str(),drive,dir,file,ext);
	_makepath(fullpath,drive,dir,file,"files");
	fp = fopen(fullpath,"r");
	if (fp != NULL) {
		while (fgets(buf,_MAX_PATH,fp) != NULL) {
			p = strrchr(buf,'\n');
			if (p) {
				*p = '\0';
			}
			unlink(buf);
		}
		fclose(fp);
		unlink(fullpath);
	}
	if (Terminated || FileExists(errFile)) {
		unlink(toFile.c_str());
	}

	Synchronize(FinishProcess);
}
//---------------------------------------------------------------------------
void __fastcall Exec::HandleProcess()
{
	HANDLE hStdOutChild,hStdOutReadTmp,hStdOutRead;
	HANDLE hStdErrChild;
	SECURITY_ATTRIBUTES sa;
	CHAR lpBuffer[256];
	DWORD nBytesRead;
	DWORD nCharsWritten;
	BOOL ret;
	AnsiString s,ss;

	hChildProcess = NULL;
	ZeroMemory(&sa,sizeof(SECURITY_ATTRIBUTES));
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	// Read,Write用のパイプを作成する
	CreatePipe(&hStdOutReadTmp,&hStdOutChild,&sa,0);
	// 標準エラー出力を複製する
	DuplicateHandle(GetCurrentProcess(),hStdOutChild,
					GetCurrentProcess(),&hStdErrChild,0,
					TRUE,DUPLICATE_SAME_ACCESS);

	// GUIアプリが使う標準出力から読み込むためのハンドルを作成する
	DuplicateHandle(GetCurrentProcess(),hStdOutReadTmp,
					GetCurrentProcess(),
					&hStdOutRead, // Address of new handle.
					0,FALSE, // Make it uninheritable.
					DUPLICATE_SAME_ACCESS);

	CloseHandle(hStdOutReadTmp);
	ret = RunChildProcess(hStdOutChild,hStdErrChild);
	CloseHandle(hStdOutChild);
	CloseHandle(hStdErrChild);
	if (ret == TRUE) {
		for (;;) {
			if (!ReadFile(hStdOutRead,lpBuffer,sizeof(lpBuffer),&nBytesRead,NULL)) {
				break; // pipe done
			}
			if (nBytesRead == 0) {
				continue;
			}
			if (Terminated == true) {
				TerminateProcess(hChildProcess,0);
				break;
			}
			lpBuffer[nBytesRead] = '\0';
			if (cmd_usr != "") {
				cmdInfo_usr = (AnsiString)lpBuffer;
				Synchronize(UpdateProgress_usr);
			} else {
				s = (AnsiString)lpBuffer;
				if (sscanf(lpBuffer,"%d",&persent) == 1) {
					Synchronize(UpdateProgress);
				}
			}
		}
		if (Terminated != true) {
			WaitForSingleObject(hChildProcess,INFINITE);
		}
		CloseHandle(hChildProcess);
	}
	hChildProcess = NULL;
	CloseHandle(hStdOutRead);
}
//---------------------------------------------------------------------------
BOOL __fastcall Exec::RunChildProcess(HANDLE hStdOut,HANDLE hStdErr)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL ret;

	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = hStdOut;
	si.hStdError  = hStdErr;

	ret = TRUE;
	if (!CreateProcess(NULL,cmd.c_str(),NULL,NULL,TRUE,CREATE_NEW_CONSOLE | IDLE_PRIORITY_CLASS,NULL,NULL,&si,&pi)) {
		ret = FALSE;
	}
	hChildProcess = pi.hProcess;

#if 0
	if (!CloseHandle(pi.hThread)) {
		ret = FALSE;
	}
#endif
	return ret;
}
//---------------------------------------------------------------------------
void __fastcall Exec::UpdateProgress(void)
{
	int param;
	param = (int)persent;
	Form1->UpdateProgress1(cmdInfo,param);
}
//---------------------------------------------------------------------------
void __fastcall Exec::UpdateProgress_c1(void)
{
	int param;

	AnsiString s;
	s.printf("upconv");
	s += "(";
	s += cmdInfo_c1;
	s += ")";
	param = (int)persent_c1;
	Form1->UpdateProgress1(s,param);
}
//---------------------------------------------------------------------------
void __fastcall Exec::UpdateProgress_c2(void)
{
}
//---------------------------------------------------------------------------
void __fastcall Exec::FinishProcess(void)
{
	Form1->EndExec(0);
}
//---------------------------------------------------------------------------
void __fastcall Exec::UpdateProgress_usr(void)
{
	Form1->UpdateProgress_usr(cmdInfo_usr);
}
//---------------------------------------------------------------------------
void __fastcall Exec::FinishProcess_usr(void)
{
	Form1->EndExec_usr(0);
}

