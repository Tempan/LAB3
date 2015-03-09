#include <windows.h>
#include "resource.h"
#include <tchar.h>

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow ) 
{
	HWND hDlg;
	MSG msg;
	BOOL ret;

	InitCommonControls();
	hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
	ShowWindow(hDlg, nCmdShow);

	while((ret = GetMessage(&msg, 0, 0, 0)) != 0) 
	{
		if(ret == -1)
			return -1;

		if(!IsDialogMessage(hDlg, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}





	MessageBox(NULL, "It works man?\n", "A cool Mbox", 0);
	return 1;
}
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}