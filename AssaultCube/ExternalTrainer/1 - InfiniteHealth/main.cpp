#include <Windows.h>
#include <string>

int main()
{
	LPCSTR GameWindowTitle = "AssaultCube";
	DWORD healthBaseAddress = 0x50F4F4;
	BYTE healthOffset = 0xF8;
	DWORD healthDynamicAddress = NULL;
	DWORD oldHealth = 100;
	DWORD newHealth = 1337;
	bool isActive = false;

	HWND hWindow = NULL;
	do {
		hWindow = FindWindowA(NULL, GameWindowTitle);
	} while (hWindow == NULL);

	DWORD dwProcessId = NULL;
	GetWindowThreadProcessId(hWindow, &dwProcessId);

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	DWORD temp;
	ReadProcessMemory(procHandle, (BYTE*)healthBaseAddress, &temp, sizeof(temp), NULL);
	healthDynamicAddress = temp + healthOffset;

	do {
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			isActive = !isActive;

			if (isActive) {
				ReadProcessMemory(procHandle, (BYTE*)healthDynamicAddress, &oldHealth, sizeof(oldHealth), NULL);
			}
			else {
				WriteProcessMemory(procHandle, (BYTE*)healthDynamicAddress, &oldHealth, sizeof(oldHealth), NULL);
			}
		}

		if (isActive)
			WriteProcessMemory(procHandle, (BYTE*)healthDynamicAddress, &newHealth, sizeof(newHealth), NULL);
	} while (!GetAsyncKeyState(VK_END));
}