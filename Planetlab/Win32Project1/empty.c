#include <windows.h>
#include "resource.h"
#include <tchar.h>

HINSTANCE hInst;

HWND dia1, dia2;




//F�rsta dialogrutans funktioner... (DIALOG2)
INT_PTR CALLBACK DialogProc2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
	  case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case btn_exit:
					DestroyWindow(hDlg);
				  //SendMessage(hDlg, WM_CLOSE, 0, 0);
				  return TRUE;

			case btn_createPlanet:
				ShowWindow(dia1, 1);
                        return FALSE;

			case btn_start:
				//setWindowText();
				LPCSTR nrplanets;
				int nrofplanets = 0;
				nrplanets = (LPCSTR) nrofplanets;
				//SetWindowText(dia2, "HEJJJJJJJJ!!!!");
				SetDlgItemText(dia2,TXT_NrOfLocalPlanets, nrofplanets);
				//UpdateWindow(dia2);
				break;

		}
		break;

	  /*case WM_CLOSE:
		if(MessageBox(hDlg, TEXT("Close the program?"), TEXT("Close"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
		  DestroyWindow(hDlg);
		}
		return TRUE;

	  case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;*/
  }

  return FALSE;
}

//Andra dialogrutans funktioner... (DIALOG1)
INT_PTR CALLBACK DialogProc1(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDCANCEL:
						DestroyWindow(hDlg);
						return TRUE;

				case btn_createPlanet:
					ShowWindow(dia1, 1);
							return FALSE;
				case btn_addPlanet:
					break;
			}
			break;

	  }

  return FALSE;
}



int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow ) 
{
	MSG msg;
	BOOL ret;
	//MessageBox(NULL, "It works man?\n", "A cool Mbox", 0);
	//InitCommonControls();
	//�ppnar DIALOG1
	dia1 = CreateDialogParam(hInstance, MAKEINTRESOURCE(DIALOG1), 0, DialogProc1, 0);
	hInst = hInstance;

	
	//�ppnar DIALOG2
	dia2 = CreateDialogParam(hInstance, MAKEINTRESOURCE(DIALOG2), 0, DialogProc2, 0);
	hInst = hInstance;
	ShowWindow(dia2, nCmdShow);
	

  while((ret = GetMessage(&msg, 0, 0, 0)) != 0) 
  {
    if(ret == -1)
      return -1;

    if(!IsDialogMessage(dia2, &msg)) 
	{
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }





	
	return 1;
}
