#include <Windows.h>
#include <string>

int main()
{
	LPCSTR GameWindowTitle = "AssaultCube";
	DWORD ammoDecreaseAddress = 0x4637E9;
	BYTE ammoDefaultOpCode[] = { 0xFF, 0x0E };
	BYTE ammoNewOpCode[] = { 0x90, 0x90 };
	bool isActive = false;

	HWND hWindow = NULL;
	do {
		hWindow = FindWindowA(NULL, GameWindowTitle);
	} while (hWindow == NULL);

	DWORD dwProcessId = NULL;
	GetWindowThreadProcessId(hWindow, &dwProcessId);

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	do {
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			isActive = !isActive;

			if (isActive)
				WriteProcessMemory(procHandle, (BYTE*)ammoDecreaseAddress, ammoNewOpCode, 2, NULL);
			else
				WriteProcessMemory(procHandle, (BYTE*)ammoDecreaseAddress, ammoDefaultOpCode, 2, NULL);
		}
	} while (!GetAsyncKeyState(VK_END));
}