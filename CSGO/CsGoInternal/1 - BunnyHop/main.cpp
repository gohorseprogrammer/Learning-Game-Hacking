#include <Windows.h>

DWORD dwPlayerBaseAddress = 0xAA3154;
DWORD dwFlagsOffset = 0x100;
DWORD dwForceJumpOffset = 0x4F174D0;

HMODULE hClientModule = NULL;
DWORD dwPlayerBase = NULL;
DWORD dwFlags = NULL;

bool isActive = false;

DWORD WINAPI Main_Thread(LPVOID lParam) {
	hClientModule = GetModuleHandleA("client.dll");

	do {
		if (GetAsyncKeyState(VK_INSERT) & 1)
			isActive = !isActive;

		if (isActive) {
			dwPlayerBase = *(DWORD*)((DWORD)hClientModule + dwPlayerBaseAddress);
			dwFlags = *(DWORD*)(dwPlayerBase + dwFlagsOffset);

			if (dwFlags == 257)
				*(DWORD*)((DWORD)hClientModule + dwForceJumpOffset) = 5;
			else
				*(DWORD*)((DWORD)hClientModule + dwForceJumpOffset) = 4;

			Sleep(100);
		}

	} while (!GetAsyncKeyState(VK_ESCAPE));

	return S_OK;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0x1000, &Main_Thread, 0, 0, NULL);
	}

	return TRUE;
}