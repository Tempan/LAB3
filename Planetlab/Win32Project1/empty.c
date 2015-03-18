#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "resource.h"
#include <tchar.h>
#include <stdio.h>
#include "wrapper.h"

HINSTANCE hInst;
DWORD selectedItem;
HWND dia1, dia2;
char name[20];
struct pt* root;
struct pt* serverRoot;
int amount = 0;
int i = 0;
int test;
int LengthOfName, LengthOfX, LengthOfY, LengthOfVX, LengthOfVY, LengthOfMass, lengthOfLife;
double _sx = 0, _sy = 0, _vx = 0, _vy = 0, _mass = 0, _life = 0;
char buffer[sizeof(struct pt)];
LPTSTR Slot = TEXT("\\\\.\\mailslot\\sample_mailslot");

void AddPlanetsToLocalList(struct pt *);
void AddPlanets();
void sendToServer();
void readFromFile(struct pt*);
void writeToFile();
void RemovePlanetFromLocalList(struct pt* Testplanet);
void RemovePlanetFromServerList(struct pt* Testplanet);
void ClearListbox(int list);
void AddToListbox(int list, char* message);
void AddPlanetsToServerList(struct pt *Testplanet);


DWORD WINAPI threadRead( void* data );

//Första dialogrutans funktioner... (DIALOG2)
INT_PTR CALLBACK DialogProc2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	struct pt *newplanet = (struct pt*)malloc(sizeof(struct pt));
	struct pt* iterator;
	DWORD dwBytesRead;
	planet_type loadplanets;
	HANDLE mailSlot, file, wfile;
	int count, i, test;
	char temp[sizeof(struct pt)];
	mailSlot = mailslotConnect(Slot);

	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case btn_exit:
			CloseWindow(dia1);
			CloseWindow(dia2);
			return TRUE;

		case btn_createPlanet:
			ShowWindow(dia1, 1);
			break;

		case btn_start:

			//SendDlgItemMessage(dia2, list_history, LB_GETSELITEMS, 0, (LPARAM)Testplanet);
			//	//setWindowText();
			//	LPCSTR nrplanets;
			//	int nrofplanets = 0;
			//	nrplanets = (LPCSTR) nrofplanets;
			//	//SetWindowText(dia2, "HEJJJJJJJJ!!!!");
			//	SetDlgItemText(dia2,TXT_NrOfLocalPlanets, nrofplanets);
			//	//UpdateWindow(dia2);
			break;
		case btn_SendToServer:

			test = SendDlgItemMessage(hDlg, list_localPlanets, LB_GETCOUNT, NULL, NULL);
			for (i = 0; i < test; i++)		//loop list
			{
				if(SendDlgItemMessage(hDlg, list_localPlanets, LB_GETSEL, i, NULL))				//Get Selected values from list
				{
					//byter aldrig buffern....
					SendDlgItemMessage(hDlg, list_localPlanets, LB_GETTEXT, i, (LPARAM)buffer);	//GET text from selected items
					iterator = root;						//SET iterator to root
					while(iterator != NULL)	
					{
						if(!strcmp(iterator->name, buffer))			//StringCompare if buffer = planets name
						{
							memcpy(newplanet, iterator, sizeof(struct pt));
							sendToServer(newplanet);					//send to server
						}
						iterator = iterator->next;
					}
				}

			}

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
				{
					readFromFile((struct pt*)buffer);
				}
			} while (dwBytesRead != 0);

			CloseHandle(file);
			break;


		case btn_SaveInFile: // Saving the Listbox Items to File 

			file = OpenFileDialog("save", GENERIC_WRITE, CREATE_NEW);
			if (file == INVALID_HANDLE_VALUE)
				return GetLastError();

			writeToFile(file);
			CloseHandle(file);
			break;

		}
		break;

	case WM_CLOSE:
		CloseWindow(hDlg);
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
			CloseWindow(hDlg);
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

//read everything from DIA1
//Check if everything is correctly done so we can add the planet to the list
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

	AddPlanetsToLocalList(newplanet);
}

void AddPlanetsToServerList(struct pt *Testplanet)
{
	struct pt * newplanet= (struct pt*)malloc(sizeof(struct pt));
	memcpy(newplanet, Testplanet, sizeof(struct pt));

	if(serverRoot == NULL)
	{
		serverRoot = newplanet;
	}
	else
	{
		struct pt* iterator = serverRoot;
		while(iterator->next != NULL)
		{
			iterator = iterator->next;
		}
		iterator->next = newplanet;
	}
	SendDlgItemMessage(dia2, list_localPlanets, LB_ADDSTRING, 0, (LPARAM)Testplanet->name);
	amount--;
	SetDlgItemInt(dia2,TXT_NrOfLocalPlanets, amount, FALSE);

}

//Add planet to list linked to root
void AddPlanetsToLocalList(struct pt *Testplanet)
{
	struct pt * newplanet= (struct pt*)malloc(sizeof(struct pt));
	memcpy(newplanet, Testplanet, sizeof(struct pt));

	if(root == NULL)
	{
		root = newplanet;
	}
	else
	{
		struct pt* iterator = root;
		while(iterator->next != NULL)
		{
			iterator = iterator->next;
		}
		iterator->next = newplanet;
	}
	SendDlgItemMessage(dia2, list_localPlanets, LB_ADDSTRING, 0, (LPARAM)Testplanet->name);
	amount++;
	SetDlgItemInt(dia2,TXT_NrOfLocalPlanets, amount, FALSE);

}

//Hör till remove from list_living planets
void RemovePlanetFromServerList(struct pt* Testplanet)
{
	planet_type *tempo = serverRoot;
	planet_type *swapper = NULL;
	if(Testplanet == serverRoot)
	{
		serverRoot = serverRoot->next;
	}
	else
	{
		while (tempo->next != NULL)
		{
			if(!strcmp(Testplanet->name, tempo->next->name))//(Testplanet == tempo->next)
			{
				if(tempo->next->next != NULL)
				{
					swapper = tempo->next->next;
					free(tempo->next);
					tempo->next = swapper;
					break;
				}
				else 
				{
					tempo->next = NULL;
					break;
				}
			}
			tempo = tempo->next;
		}
	}
}

void RemovePlanetFromLocalList(struct pt* Testplanet)
{
	planet_type *tempo = root;
	planet_type *swapper = NULL;
	if(!strcmp(Testplanet->name, root->name))
	{
		root = root->next;
	}
	else
	{
		while (tempo->next != NULL)
		{
			if(!strcmp(Testplanet->name, tempo->next->name))//(Testplanet == tempo->next)
			{
				if(tempo->next->next != NULL)
				{
					swapper = tempo->next->next;
					free(tempo->next);
					tempo->next = swapper;
					break;
				}
				else 
				{
					tempo->next = NULL;
					break;
				}
			}
			tempo = tempo->next;
		}
	}
}

//Sent all the planets to the server
void sendToServer(struct pt *planetToServer)
{
	HANDLE mailSlot;
	struct pt planetToSend = *planetToServer;
	struct pt* iterator = root;
	mailSlot = mailslotConnect(Slot); 
	planetToSend.next = NULL;
	mailslotWrite (mailSlot, (void*)&planetToSend, sizeof(struct pt));
	SendDlgItemMessage(dia2, list_livingPlanets, LB_ADDSTRING, 0, (LPARAM)planetToSend.name);
	AddPlanetsToServerList(&planetToSend);
	RemovePlanetFromLocalList(planetToServer);
	ClearListbox(list_localPlanets);
	iterator = root;
	while(iterator != NULL)
	{
		AddToListbox(list_localPlanets, iterator->name);
		iterator = iterator->next;
	}
}

void readFromFile(struct pt* Testplanet)
{
	Testplanet->next = NULL;
	sprintf_s(Testplanet->pid,15, "%lu ", GetCurrentProcessId());
	AddPlanetsToLocalList(Testplanet);
}

void writeToFile(HANDLE wfile)
{
	struct pt* iterator;
	DWORD dwBytesRead;
	int count, i, test;

	count = SendDlgItemMessage(dia2, list_localPlanets,LB_GETCOUNT, NULL, NULL);
	for (i = 0; i < count; i++)
	{
		test = SendDlgItemMessage(dia2, list_localPlanets,LB_GETSEL, i, NULL);
		if(test)
		{
			SendDlgItemMessage(dia2, list_localPlanets,LB_GETTEXT,(WPARAM) i, (LPARAM)buffer);
			iterator = root;
			while(iterator != NULL)
			{
				if(!strcmp(buffer, iterator->name))
				{
					WriteFile(wfile, iterator, sizeof(struct pt), (LPDWORD)&dwBytesRead, NULL);
				}
				iterator = iterator->next;
			}

		}
	}
}

// read if planet is dead
DWORD WINAPI threadRead( void* data )
{
	char id[20];
	char nameFromServer[20];
	char theMessage[200];
	struct pt* iterator;
	struct pt* it;
	HANDLE mailSlot;
	LPTSTR Slot; 
	char slot[40];
	strcpy_s(slot, sizeof(slot), "\\\\.\\mailslot\\test");
	sprintf_s(id,sizeof(id), "%d", data);
	strcat_s(slot,sizeof(slot),id);
	Slot = slot;
	mailSlot = mailslotCreate(Slot);
	Sleep(2000);		// make sure mailslotConnect is done before starting the loop!!
	while (1)
	{
		int bytesread = mailslotRead(mailSlot, theMessage, 424);
		if (bytesread > 0)
		{
			SendMessage(GetDlgItem(dia2, list_history), LB_INSERTSTRING, NULL, (LPARAM)theMessage);

			// Update planets in server root
			iterator = serverRoot;

			while (iterator != NULL)
			{
				int planetNameLength = strlen(iterator->name);
				strncpy(nameFromServer, buffer, planetNameLength);
				nameFromServer[planetNameLength] = '\0';

				if (strcmp(nameFromServer, iterator->name) == 0)
				{
					RemovePlanetFromServerList(iterator);

					// Update listbox
					ClearListbox(list_livingPlanets);
					it = serverRoot;
					while (it != NULL)
					{
						AddToListbox(list_livingPlanets, it->name);
						it = it->next;
					}
					break;

				}
				iterator = iterator->next;
			}
		}
	}
	mailslotClose (mailSlot);
}


//Hör till remove from list_living planets
void ClearListbox(int list)
{
	HWND handleClearList = GetDlgItem(dia2, list);
	SendMessage(handleClearList, LB_RESETCONTENT, NULL, NULL);
}

//Hör till remove from list_living planets
void AddToListbox(int list, char* message)
{
	HWND handleAddToList = GetDlgItem(dia2, list);
	SendMessage(handleAddToList, LB_ADDSTRING, NULL, (LPARAM)message);
}
