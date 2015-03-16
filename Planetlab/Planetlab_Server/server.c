#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "wrapper.h"

#define UPDATE_FREQ     1	/* update frequency (in ms) for the timer */
#define G 6.67259e-11
#define DT 10
CRITICAL_SECTION Crit;
LPTSTR Slot = TEXT("\\\\.\\mailslot\\sample_mailslot");
/* (the server uses a mailslot for incoming client requests) */
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

	/* Create the window, 3 last parameters important */
	/* The tile of the window, the callback function */
	/* and the backgrond color */

	hWnd = windowCreate (hPrevInstance, hInstance, nCmdShow, "Himmel", MainWndProc, COLOR_WINDOW+1);

	/* start the timer for the periodic update of the window    */
	/* (this is a one-shot timer, which means that it has to be */
	/* re-set after each time-out) */
	/* NOTE: When this timer expires a message will be sent to  */
	/*       our callback function (MainWndProc).               */

	windowRefreshTimer (hWnd, UPDATE_FREQ);


	/* create a thread that can handle incoming client requests */
	/* (the thread starts executing in the function mailThread) */

	/* NOTE: See online help for details, you need to know how  */ 
	/*       this function does and what its parameters mean.   */
	/* We have no parameters to pass, hence NULL				*/


	threadID = threadCreate (mailThread, NULL); 


	/* (the message processing loop that all windows applications must have) */
	/* NOTE: just leave it as it is. */
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
	/* create a mailslot that clients can use to pass requests through   */
	/* (the clients use the name below to get contact with the mailslot) */
	/* NOTE: The name of a mailslot must start with "\\\\.\\mailslot\\"  */

	mailbox = mailslotCreate(Slot);


	for(;;) 
	{				
		/* (ordinary file manipulating functions are used to read from mailslots) 
		in this example the server receives strings from the client side and   
		displays them in the presentation window                               
		NOTE: binary data can also be sent and received, e.g. planet structures*/

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
		EnterCriticalSection(&Crit);
		while(iterator != NULL)
		{
			SetPixel (hDC, iterator->sx, iterator->sy, (COLORREF) color);
			iterator = iterator->next;
		}
		LeaveCriticalSection(&Crit);
		windowRefreshTimer (hWnd, UPDATE_FREQ);
		break;

	case WM_PAINT:
		/* NOTE: The code for this message can be removed. It's just */
		/*       for showing something in the window.                */
		context = BeginPaint( hWnd, &ps );
		/* (you can safely remove the following line of code) */
		//TextOut( context, 10, 10, "Hello, World!", 13 ); /* 13 is the string length */
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
	EnterCriticalSection(&Crit);
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
	LeaveCriticalSection(&Crit);
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
	messages = mailslotConnect(Slot);
	while(planet->life > 0) //F�r varje planet
	{
		EnterCriticalSection(&Crit);
		totX=0;
		totY=0;
		iterator = root;
		while (iterator != NULL )	//r�kna mellan planeter
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
		//r�kna ut ny position
		planet->vx = planet->vx + (totX * DT);				//vx_new
		planet->sx = planet->sx + (planet->vx * DT);		//sx_new

		planet->vy = planet->vy + (totY * DT);				//vx_new
		planet->sy = planet->sy + (planet->vy * DT);		//sx_new
		LeaveCriticalSection(&Crit);
		//d�da om den �r utanf�r
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
	if(planet = root)
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