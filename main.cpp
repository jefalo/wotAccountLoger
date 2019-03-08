#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <io.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
using namespace std;
#include "detours.h"
#pragma comment(lib, "detours.lib")
extern "C"
{
	struct s1 {
		char pad24[24];
		int f24;
	};
	struct s0 {
		int f0;
		struct s1* f4;
	};
	typedef struct s0* (__cdecl *uiUpdater)(int a1, int a2, int a3, void* a4);
}


uiUpdater packetSend = (uiUpdater)((DWORD)GetModuleHandleA("worldoftanks.exe") + 0x12DEA70);

ofstream* fout = NULL;

struct s0*  __cdecl packetSent(int a1, int a2, int a3, void* a4)
{
	fout = new ofstream("log.txt", std::ios::app);
	fout->write((char*)a1,a2);
    *fout <<endl;
	fout->close();
	return packetSend(a1, a2, a3, a4);
}

DWORD WINAPI ThreadDetour(LPVOID lpParam)
{
	char buf[50] = {};
	sprintf(buf, "%X", (DWORD)GetModuleHandleA("worldoftanks.exe") + 0x12DEA70);
	MessageBoxA(NULL, buf, "ThreadDetour", MB_OK);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)packetSend, packetSent);
	DetourTransactionCommit();
	while(true)
	{
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD nReason, VOID* pvReserved)
{
	HANDLE hThreadDetour;
	DWORD dwThreadId3 = 0;
	switch (nReason)
    {
        case DLL_PROCESS_ATTACH:
			hThreadDetour = CreateThread(NULL, 0, ThreadDetour, NULL, 0, &dwThreadId3);
            break;

        case DLL_PROCESS_DETACH:
			CloseHandle(hThreadDetour);
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // successful
}