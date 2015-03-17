#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "wrapper.h"

/* the server uses a timer to periodically update the presentation window */
/* here is the timer id and timer period defined                          */

#define UPDATE_FREQ     1	/* update frequency (in ms) for the timer */
#define G 6.67259e-11
#define DT 10
CRITICAL_SECTION Crit;
LPTSTR Slot = TEXT("\\\\.\\mailslot\\sample_mailslot");
struct pt* root;
void checkPlanets(struct pt* Testplanet);
void createPlanet(char*, double, double, double, double, double, int);
void* updatePlanets(void* planeten);
void removePlanets(struct pt* planeten);

LRESULT WINAPI MainWndProc( HWND, UINT, WPARAM, LPARAM );
DWORD WINAPI mailThread(LPVOID);
HDC hDC;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow ) {
	HWND hWnd;
	DWORD threadID;
	MSG msg;
	InitializeCriticalSection(&Crit);

	hWnd = windowCreate (hPrevInstance, hInstance, nCmdShow, "Himmel", MainWndProc, COLOR_WINDOW+1);

	windowRefreshTimer (hWnd, UPDATE_FREQ);

	threadID = threadCreate (mailThread, NULL); 

	while( GetMessage( &msg, NULL, 0, 0 ) ) 
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return msg.wParam;
}

DWORD WINAPI mailThread(LPVOID arg) 
{
	char buffer[1024];
	DWORD bytesRead;
	static int posY = 0;
	HANDLE mailbox;
	
	mailbox = mailslotCreate(Slot);


	for(;;) 
	{	
		bytesRead = mailslotRead (mailbox, buffer, sizeof(buffer));

		if(bytesRead!= 0) 
		{
			struct pt *planet = (struct pt*)malloc(sizeof(struct pt));	
			memcpy(planet, buffer, sizeof(struct pt));
			checkPlanets(planet);

			/* NOTE: It is appropriate to replace this code with something that match your needs here.*/
			posY++;  
			/* (hDC is used reference the previously created window) */							
			TextOut(hDC, 10, 50+posY%200, buffer, bytesRead);
		}
	}

	return 0;
}

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {

	struct pt* iterator;
	PAINTSTRUCT ps;
	static int posX = 100;
	int posY = 100;
	HANDLE context;
	static DWORD color = 0;

	switch( msg ) {
	case WM_CREATE:       
		hDC = GetDC(hWnd);  
		break;   
	case WM_TIMER:
		iterator = root;
		//EnterCriticalSection(&Crit);
		while(iterator != NULL)
		{
			SetPixel (hDC, iterator->sx, iterator->sy, (COLORREF) color);
			iterator = iterator->next;
		}
		//LeaveCriticalSection(&Crit);
		windowRefreshTimer (hWnd, UPDATE_FREQ);
		break;

	case WM_PAINT:
		context = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		ReleaseDC(hWnd, hDC); /* Some housekeeping */
		break;
	default:
		return( DefWindowProc( hWnd, msg, wParam, lParam )); 
	}
	return 0;
}

void checkPlanets(struct pt *Testplanet)
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
	threadCreate((LPTHREAD_START_ROUTINE)updatePlanets, Testplanet);
}

void* updatePlanets(void* planeten) // Ska uppdatera rutan och flytta planeternas pixlar
{
	double r, a1, totX, totY;
	char messageWhyDie[200];
	struct pt *planet = (struct pt*)planeten;
	struct pt* iterator;
	char slot[40];
	HANDLE messages;
	LPTSTR Slot; 
	strcpy_s(slot, sizeof(slot), "\\\\.\\mailslot\\test");
	strcat_s(slot,sizeof(slot),planet->pid);
	Slot = slot;
	iterator = root;
	messages = mailslotConnect(Slot);
	while(planet->life > 0) //För varje planet
	{
		EnterCriticalSection(&Crit);
		totX=0;
		totY=0;
		while (iterator != NULL )	//räkna mellan planeter
		{
			if(iterator != planet)
			{
				r = sqrt(pow((iterator->sx - planet->sx), 2)+ pow((iterator->sy - planet->sy), 2));	
				a1 = G * (iterator->mass / pow(r,2));
				totX += a1 * ((iterator->sx - planet->sx) / r);
				totY += a1 * ((iterator->sy - planet->sy) / r); 
			}

			iterator = iterator->next; 
		}
		//räkna ut ny position
		planet->vx = planet->vx + (totX * DT);				//vx_new
		planet->sx = planet->sx + (planet->vx * DT);		//sx_new

		planet->vy = planet->vy + (totY * DT);				//vx_new
		planet->sy = planet->sy + (planet->vy * DT);		//sx_new
		LeaveCriticalSection(&Crit);

		//döda om den är utanför
		if(planet->sx < 0 || planet->sx > 800 || planet->sy < 0 || planet->sy > 600)
		{
			strcpy_s(messageWhyDie, sizeof(messageWhyDie), planet->name);
			strcat_s(messageWhyDie, sizeof(messageWhyDie), " died because out of bounds!");
			mailslotWrite(messages, messageWhyDie, 200);
			removePlanets(planet);
			return NULL;
		}
		planet->life = planet->life - 1;		//minska liv med 1
		Sleep(UPDATE_FREQ);
	}
	//die because life < 1
	strcpy_s(messageWhyDie, sizeof(messageWhyDie), planet->name);
	strcat_s(messageWhyDie, sizeof(messageWhyDie), " died because out of lifes!");

	mailslotWrite(messages, messageWhyDie, 200);
	removePlanets(planet);
}

void removePlanets(struct pt* planeten)
{
	struct pt *planet = (struct pt*)planeten;
	struct pt* iterator;
	struct pt* swapper;
	EnterCriticalSection(&Crit);
	iterator = root;
	if(planet == root)
	{
		root = planet->next;
	}
	else
	{
		while(iterator->next != NULL)
		{
			if(planet == iterator->next)
			{
				if(iterator->next != NULL)
				{
					swapper = iterator->next->next;
					free(iterator->next);
					iterator->next = swapper;
				}
				else
				{
					free(iterator->next);
					iterator->next = NULL;
				}
			}
			else
				iterator = iterator->next;
		}
	}
	LeaveCriticalSection(&Crit);
}