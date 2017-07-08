//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include <string.h>
#include "Unit3.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

// ���ӁF�قȂ�X���b�h�����L���� VCL �̃��\�b�h/�֐�/�v���p�e�B��ʂ�
// ���b�h���L�̃I�u�W�F�N�g�ɑ΂��Ă� Synchronize ���g�p�ł��܂��B
//
//		Synchronize(&UpdateCaption);
//
// �Ⴆ�� UpdateCaption ���ȉ��̂悤�ɒ�`��
//
//		void __fastcall TThread_c::UpdateCaption()
//		{
//		  Form1->Caption = "�X���b�h���珑�������܂���";
//		}
//---------------------------------------------------------------------------

__fastcall Exec_c::Exec_c(bool CreateSuspended)
	: TThread(CreateSuspended)
{

}
//---------------------------------------------------------------------------
void __fastcall Exec_c::Execute()
{
	if (Terminated == true) {
		return;
	}
	cmdInfo = "";
	persent = 0;
	HandleProcess();
}
//---------------------------------------------------------------------------
void __fastcall Exec_c::HandleProcess()
{
	HANDLE hStdOutChild,hStdOutReadTmp,hStdOutRead;
	HANDLE hStdErrChild;
	SECURITY_ATTRIBUTES sa;
	CHAR lpBuffer[256];
	DWORD nBytesRead;
	DWORD nCharsWritten;
	BOOL ret;
	AnsiString s,ss;
	int per;

	hChildProcess = NULL;
	ZeroMemory(&sa,sizeof(SECURITY_ATTRIBUTES));
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	// Read,Write�p�̃p�C�v���쐬����
	CreatePipe(&hStdOutReadTmp,&hStdOutChild,&sa,0);
	// �W���G���[�o�͂𕡐�����
	DuplicateHandle(GetCurrentProcess(),hStdOutChild,
					GetCurrentProcess(),&hStdErrChild,0,
					TRUE,DUPLICATE_SAME_ACCESS);

	// GUI�A�v�����g���W���o�͂���ǂݍ��ނ��߂̃n���h�����쐬����
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
			s = (AnsiString)lpBuffer;
			if (sscanf(lpBuffer,"%d",&per) == 1) {
				persent = per;
			} else {
				ss = s.TrimLeft();
				s = ss.TrimRight();
				if (s == "ABE") {
					cmdInfo = s;
				} else if (s == "SRC") {
					cmdInfo = "Resampling";
				} else if (s == "HFC Auto") {
					cmdInfo = s;
				} else if (s == "LPF") {
					cmdInfo = s;
				} else if (s == "NR") {
					cmdInfo = s;
				} else if (s == "HFA1" || s == "HFA2" || s == "HFA3") {
					cmdInfo = s;
				} else if (s == "SRC(DS)") {
					cmdInfo = "Down Sampling";
				} else if (s == "Post ABE") {
					cmdInfo = s;
				}
				if (s == "End") {
					break;
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
BOOL __fastcall Exec_c::RunChildProcess(HANDLE hStdOut,HANDLE hStdErr)
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

	return ret;
}
