/*********************************************
* client.c
*
* Author: mle@mdh.se
* Desc: lab-skeleton for the client side of an
* client-server application
* Date: 990309
*
* Revised 011018 by Dag Nyström & Juka Mäki-Turja
* NOTE: the server must be started BEFORE the
* client.
*********************************************/

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "wrapper.h"
LPTSTR Slot = TEXT("\\\\.\\mailslot\\sample_mailslot");
#define MESSAGE "Hej på dig!"
void main(void) 
{
	struct pt *newplanet = (struct pt*)malloc(sizeof(struct pt));
	DWORD WINAPI threadRead( LPVOID lpParam );
	double _sx, _sy, _vx, _vy, _mass, _life;
	HANDLE mailSlot;
	DWORD bytesWritten;
	Sleep(2000);
	mailSlot = mailslotConnect(Slot); 

	/* NOTE: replace code below for sending planet data to the server. */
	threadCreate(threadRead, 0);
	Sleep(2002);
	
	/* send a friendly greeting to the server
	NOTE: The messages sent to the server need not to be of equal size.
	Messages can be of different sizes as long as they don't exceed the
	maximum message size that the mailslot can handle (defined upon creation).*/

	/*printf("Name of planet");
	gets_s(newplanet->name,sizeof(newplanet->name));
	printf("x-position");
	fgets(message, sizeof (message), stdin);
	sscanf_s(message, "%lf", &_sx);
	printf("y-position");
	fgets(message, sizeof (message), stdin);
	sscanf_s(message, "%lf", &_sy);
	printf("x-velocity");
	fgets(message, sizeof (message), stdin);
	sscanf_s(message, "%lf", &_vx);
	printf("y-velocity");
	fgets(message, sizeof (message), stdin);
	sscanf_s(message, "%lf", &_vy);
	printf("planet mass");
	fgets(message, sizeof (message), stdin);
	sscanf_s(message, "%lf", &_mass);
	printf("planet life");
	fgets(message, sizeof (message), stdin);
	sscanf_s(message, "%lf", &_life);*/

	/*newplanet->sx = _sx;											
	newplanet->sy = _sy;											
	newplanet->vx = _vy;											
	newplanet->vy = _vx;											
	newplanet->mass = _mass;											
	newplanet->life = _life;
	newplanet->next = NULL;*/

	strcpy_s(newplanet->name, sizeof(newplanet->name), "Orvar");
	newplanet->sx = 300;	
	newplanet->sy = 300;
	newplanet->vx = 0;
	newplanet->vy = 0;
	newplanet->mass = 100000000;											
	newplanet->life = 37737383;
	newplanet->next = NULL;

	bytesWritten = mailslotWrite (mailSlot, (void*)newplanet, sizeof(struct pt));

	if (bytesWritten!=-1)
		printf("data sent to server (bytes = %d), (name = %s) \n", bytesWritten, newplanet->name);
	else
		printf("failed sending data to server\n");
	
	strcpy_s(newplanet->name, sizeof(newplanet->name), "Bertill");
	newplanet->sx = 200;											
	newplanet->sy = 300;											
	newplanet->vx = 0;											
	newplanet->vy = 0.008;											
	newplanet->mass = 1000;											
	newplanet->life = 37737388;
	newplanet->next = NULL;

	bytesWritten = mailslotWrite (mailSlot, (void*)newplanet, sizeof(struct pt));

	if (bytesWritten!=-1)
		printf("data sent to server (bytes = %d), (name = %s) \n", bytesWritten, newplanet->name);
	else
		printf("failed sending data to server\n");
	while(1)
	{
	}
	mailslotClose(mailSlot);
	return;
}

DWORD WINAPI threadRead( LPVOID lpParam ) // read if planet is dead
{
	char theMessage[200];
	HANDLE mailSlot;
	mailSlot = mailslotCreate("\\\\.\\mailslot\\test");
	while (1)
	{
		int bytesread = mailslotRead(mailSlot, theMessage, 424);
		if (bytesread > 0)
			printf("%.*s \n", bytesread, theMessage);
	}
	mailslotClose (mailSlot);
}