#include <windows.h>
#include "resource.h"
#include <tchar.h>
#include <stdio.h>
#include "wrapper.h"

HINSTANCE hInst;
HWND dia1, dia2;
char name[20];
int LengthOfName, LengthOfX, LengthOfY, LengthOfVX, LengthOfVY, LengthOfMass, lengthOfLife;
double _sx = 0, _sy = 0, _vx = 0, _vy = 0, _mass = 0, _life = 0;
struct pt* root;

LPTSTR Slot = TEXT("\\\\.\\mailslot\\sample_mailslot");
void AddPlanetsToList(struct pt *Testplanet);
void AddPlanets();
DWORD WINAPI threadRead( void* data );

//Första dialogrutans funktioner... (DIALOG2)
INT_PTR CALLBACK DialogProc2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD dwBytesRead;
	planet_type loadplanets;
	HANDLE mailSlot, file;
	char buffer[sizeof(struct pt)];
	char temp[sizeof(struct pt)];
	mailSlot = mailslotConnect(Slot); 
	/*struct pt *newplanet = (struct pt*)malloc(sizeof(struct pt));
	DWORD bytesWritten;
	Sleep(2000);
	mailSlot = mailslotConnect(Slot); */


	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case btn_exit:
			//CloseWindow(dia1);
			//CloseWindow(dia2);
			DestroyWindow(dia1);
			DestroyWindow(dia2);
			//SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;

		case btn_createPlanet:
			ShowWindow(dia1, 1);
			break;

		case btn_start:
			//	//setWindowText();
			//	LPCSTR nrplanets;
			//	int nrofplanets = 0;
			//	nrplanets = (LPCSTR) nrofplanets;
			//	//SetWindowText(dia2, "HEJJJJJJJJ!!!!");
			//	SetDlgItemText(dia2,TXT_NrOfLocalPlanets, nrofplanets);
			//	//UpdateWindow(dia2);
			break;
		case btn_SendToServer:
			//threadCreate((LPTHREAD_START_ROUTINE)updatePlanets, root);
			//mailslotWrite (mailSlot, (void*)root, sizeof(struct pt));
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
			break;
		case btn_openFromFile:
			
			file = OpenFileDialog("load", GENERIC_READ, OPEN_EXISTING);
			if (file == INVALID_HANDLE_VALUE)
				return GetLastError();

			do
			{
				memcpy(temp, buffer, sizeof(struct pt));
				ReadFile(file, buffer, sizeof(struct pt), (LPDWORD)&dwBytesRead, NULL);
				if (strcmp(((struct pt*)temp)->name, ((struct pt*)buffer)->name))
					SendDlgItemMessage(dia2, list_localPlanets, LB_ADDSTRING, 0, (LPARAM)buffer);
			} while (dwBytesRead != 0);
			CloseHandle(file);
			break;

			
		case btn_SaveInFile:
			file = OpenFileDialog("save", GENERIC_WRITE, OPEN_EXISTING);
			if (file == INVALID_HANDLE_VALUE)
				return GetLastError();
			//ta data från listan och lägg i buffer??
			WriteFile(file, buffer, sizeof(struct pt), (LPDWORD)&dwBytesRead, NULL);

			CloseHandle(file);
			break;
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;

		case WM_DESTROY:
			PostQuitMessage(0);
			return TRUE;
	}

	return FALSE;
}

//Andra dialogrutans funktioner... (DIALOG1)
INT_PTR CALLBACK DialogProc1(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HANDLE mailSlot;
	mailSlot = mailslotConnect(Slot); 
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case btn_cancel:
			CloseWindow(hDlg);
			break;

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
	//öppnar DIALOG1
	dia1 = CreateDialogParam(hInstance, MAKEINTRESOURCE(DIALOG1), 0, DialogProc1, 0);

	//öppnar DIALOG2
	dia2 = CreateDialogParam(hInstance, MAKEINTRESOURCE(DIALOG2), 0, DialogProc2, 0);
	ShowWindow(dia2, nCmdShow);

	threadCreate(threadRead, (void*)GetCurrentProcessId());

	while((ret = GetMessage(&msg, 0, 0, 0)) != 0) 
	{
		if(ret == -1)
			return -1;

		if(!IsDialogMessage(dia2, &msg) && !IsDialogMessage(dia1, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
	}
	return 1;
}

void AddPlanets()
{
	HANDLE mailSlot;
	struct pt *newplanet = (struct pt*)malloc(sizeof(struct pt));
	DWORD bytesWritten;
	mailSlot = mailslotConnect(Slot);
	GetWindowText(GetDlgItem(dia1, txt_name), name, sizeof(name));
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
		strcpy_s(newplanet->name, sizeof(newplanet->name), name);
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
		SetDlgItemText(dia1, txt_name, "Something was missing!");	//Något fält är inte ifyllt

	strcpy_s(newplanet->name, sizeof(newplanet->name), name);
	newplanet->sx = _sx;										
	newplanet->sy = _sy;											
	newplanet->vx = _vy;											
	newplanet->vy = _vx;											
	newplanet->mass = _mass;											
	newplanet->life = _life;
	newplanet->next = NULL;
	sprintf_s(newplanet->pid,15, "%lu", GetCurrentProcessId());

	// add all planets to a local file and read from that one!
	//SendMessage(GetDlgItem(dia2, list_localPlanets), LB_INSERTSTRING, NULL, (LPARAM)newplanet);
	// Send to the file or to list of local planets
	//bytesWritten = mailslotWrite (mailSlot, (void*)newplanet, sizeof(struct pt));
	AddPlanetsToList(newplanet);
}

DWORD WINAPI threadRead( void* data ) // read if planet is dead
{
	char id[20];
	char theMessage[200];
	HANDLE mailSlot;
	LPTSTR Slot; 
	char slot[40];
	strcpy_s(slot, sizeof(slot), "\\\\.\\mailslot\\test");
	sprintf_s(id,sizeof(id), "%d", data);
	strcat_s(slot,sizeof(slot),id);
	Slot = slot;
	mailSlot = mailslotCreate(Slot);
	while (1)
	{
		int bytesread = mailslotRead(mailSlot, theMessage, 424);
		if (bytesread > 0)
			SendMessage(GetDlgItem(dia2, list_history), LB_INSERTSTRING, NULL, (LPARAM)theMessage);
	}
	mailslotClose (mailSlot);
}

void AddPlanetsToList(struct pt *Testplanet)
{
	struct pt* iterator;
	if(root == NULL)
	{
		root = Testplanet;
	}
	else
	{
		iterator = root;
		while(iterator->next != NULL)
		{
			iterator = iterator->next;
		}
		iterator->next = Testplanet;
	}
}