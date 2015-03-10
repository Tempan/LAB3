#include <windows.h>
#include "resource.h"
#include <tchar.h>
#include <stdio.h>
#include "wrapper.h"

HINSTANCE hInst;
HWND dia1, dia2;
CHAR buff[1024];
char name[20];
int LengthOfName, LengthOfX, LengthOfY, LengthOfVX, LengthOfVY, LengthOfMass, lengthOfLife;
double _sx = 0, _sy = 0, _vx = 0, _vy = 0, _mass = 0, _life = 0;

HINSTANCE hInst;
LPTSTR Slot = TEXT("\\\\.\\mailslot\\mailslot_fromForm");
HWND dia1, dia2;
HANDLE mailSlot;
void AddPlanets();

//Första dialogrutans funktioner... (DIALOG2)
INT_PTR CALLBACK DialogProc2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*struct pt *newplanet = (struct pt*)malloc(sizeof(struct pt));
	DWORD bytesWritten;
	Sleep(2000);
	mailSlot = mailslotConnect(Slot); */

	//FILE *fp;
	//char buff[255];
	//fp = fopen_s(&fp, "/tmp/test.txt", "r");
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case btn_exit:
			DestroyWindow(dia1);
			DestroyWindow(dia2);
			//SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;

		case btn_createPlanet:
			ShowWindow(dia1, 1);
			return FALSE;

			//case btn_start:
			//	//setWindowText();
			//	LPCSTR nrplanets;
			//	int nrofplanets = 0;
			//	nrplanets = (LPCSTR) nrofplanets;
			//	//SetWindowText(dia2, "HEJJJJJJJJ!!!!");
			//	SetDlgItemText(dia2,TXT_NrOfLocalPlanets, nrofplanets);
			//	//UpdateWindow(dia2);
			//	break;
		case btn_SendToServer:
			//Send planets to server

			/*gets_s(newplanet->name,sizeof(newplanet->name));
			newplanet->sx = _sx;										
			newplanet->sy = _sy;											
			newplanet->vx = _vy;											
			newplanet->vy = _vx;											
			newplanet->mass = _mass;											
			newplanet->life = _life;
			newplanet->next = NULL;

			bytesWritten = mailslotWrite (mailSlot, (void*)newplanet, sizeof(struct pt));*/
			;
		case btn_SaveInFile:

			;
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
			AddPlanets();
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
	//öppnar DIALOG1
	dia1 = CreateDialogParam(hInstance, MAKEINTRESOURCE(DIALOG1), 0, DialogProc1, 0);
	hInst = hInstance;


	//öppnar DIALOG2
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

void AddPlanets()
{
	struct pt *newplanet = (struct pt*)malloc(sizeof(struct pt));
	DWORD bytesWritten;
	mailSlot = mailslotConnect(Slot);
	LengthOfName = GetWindowTextLength(GetDlgItem(dia1, txt_name));
	LengthOfX = GetWindowTextLength(GetDlgItem(dia1, txt_posx));
	LengthOfY = GetWindowTextLength(GetDlgItem(dia1, txt_posY));
	LengthOfVX = GetWindowTextLength(GetDlgItem(dia1, txt_VX));
	LengthOfVY = GetWindowTextLength(GetDlgItem(dia1, txt_VY));
	LengthOfMass = GetWindowTextLength(GetDlgItem(dia1, txt_mass));
	lengthOfLife = GetWindowTextLength(GetDlgItem(dia1, txt_life));

	if (LengthOfName > 0 && LengthOfX > 0 && LengthOfY > 0 && 
		LengthOfVX > 0 && LengthOfVY > 0 && LengthOfMass > 0 && lengthOfLife > 0  )
	{
		char *buf;

		buf = (char*)GlobalAlloc(GPTR, LengthOfName + 1);
		GetDlgItemText(dia1, txt_name, buf, LengthOfName + 1);
		strcpy_s(name, sizeof(buf), buf);

		buf = (char*)GlobalAlloc(GPTR, LengthOfX + 1);
		GetDlgItemText(dia1, txt_posx, buf, LengthOfX + 1);
		_sx = atof(buf);

		buf = (char*)GlobalAlloc(GPTR, LengthOfY + 1);
		GetDlgItemText(dia1, txt_posY, buf, LengthOfY + 1);
		_sy = atof(buf);

		buf = (char*)GlobalAlloc(GPTR, LengthOfVX + 1);
		GetDlgItemText(dia1, txt_VX, buf, LengthOfVX + 1);
		_vx = atof(buf);

		buf = (char*)GlobalAlloc(GPTR, LengthOfVY + 1);
		GetDlgItemText(dia1, txt_VY, buf, LengthOfVY + 1);
		_vy = atof(buf);

		buf = (char*)GlobalAlloc(GPTR, LengthOfMass + 1);
		GetDlgItemText(dia1, txt_mass, buf, LengthOfMass + 1);
		_mass = atof(buf);

		buf = (char*)GlobalAlloc(GPTR, lengthOfLife + 1);
		GetDlgItemText(dia1, txt_life, buf, lengthOfLife + 1);
		_life = atof(buf);

		GlobalFree((HANDLE)buf);
	}
	else 
		SetDlgItemText(dia1, txt_name, "Something was missing!");//Något fält är inte ifyllt

	strcpy_s(newplanet->name, sizeof(newplanet->name), name);
	newplanet->sx = _sx;										
	newplanet->sy = _sy;											
	newplanet->vx = _vy;											
	newplanet->vy = _vx;											
	newplanet->mass = _mass;											
	newplanet->life = _life;
	newplanet->next = NULL;
	sprintf_s(newplanet->pid,15, "%lu", GetCurrentProcessId());
	bytesWritten = mailslotWrite (mailSlot, (void*)newplanet, sizeof(struct pt));
}