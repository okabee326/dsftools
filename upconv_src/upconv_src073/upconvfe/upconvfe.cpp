//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("Unit1.cpp", Form1);
USEFORM("Unit4.cpp", Form4);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		HANDLE mutex;
		HWND handle;
		mutex = OpenMutex(MUTEX_ALL_ACCESS,False,"UPCONVFE_MUTEX");
		if (mutex != NULL) {
			handle = FindWindow("TForm1","Upconv Frontend 0.5.x");
			if (IsIconic(handle)) {
				PostMessage(handle,WM_SYSCOMMAND,SC_RESTORE,0);
			// SendMessage(handle,WM_LBUTTONDOWN,WPARAM(0),0);
			// SendMessage(handle,WM_LBUTTONUP,WPARAM(0),0);
			} else {
			// MessageBox(NULL,"window","a",0);
				SetForegroundWindow(handle);
			}
			return 0;
		}
		CreateMutex(NULL,False,"UPCONVFE_MUTEX");
		Application->Initialize();
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->CreateForm(__classid(TForm4), &Form4);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
