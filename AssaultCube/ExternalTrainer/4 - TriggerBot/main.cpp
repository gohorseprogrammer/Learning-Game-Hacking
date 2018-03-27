#include <Windows.h>
#include <string>
#include "PlayerEntity.h"

typedef int(_cdecl* _getCrossHairEntity)();
_getCrossHairEntity getCrosshairEntity;// (_getCrossHairEntity)0x4607C0;

int main()
{
	LPCSTR GameWindowTitle = "AssaultCube";
	bool isActive = false;

	HWND hWindow = NULL;
	do {
		hWindow = FindWindowA(NULL, GameWindowTitle);
	} while (hWindow == NULL);

	DWORD dwProcessId = NULL;
	GetWindowThreadProcessId(hWindow, &dwProcessId);

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	do {
		if (GetAsyncKeyState(VK_INSERT) & 1)
			isActive = !isActive;

		if (isActive) {
			ReadProcessMemory(procHandle, (BYTE*)0x4607C0, &getCrosshairEntity, sizeof(getCrosshairEntity), NULL);
			PlayerEntity* crossEntity = (PlayerEntity*)getCrosshairEntity();
			PlayerEntity* localPlayer;// = (PlayerEntity*) *(DWORD*)0x50F4F4;

			ReadProcessMemory(procHandle, (PBYTE)0x50F4F4, &localPlayer, sizeof(localPlayer), NULL);

			if (crossEntity != nullptr && crossEntity->TeamNum != localPlayer->TeamNum) {
				//localPlayer->Shoot = true;
				WriteProcessMemory(procHandle, &(localPlayer->Shoot), (LPCVOID)true, sizeof(bool), NULL);
				Sleep(10);
				//localPlayer->Shoot = false;
				WriteProcessMemory(procHandle, &(localPlayer->Shoot), (LPCVOID)false, sizeof(bool), NULL);
			}
		}

	} while (!GetAsyncKeyState(VK_END));
}