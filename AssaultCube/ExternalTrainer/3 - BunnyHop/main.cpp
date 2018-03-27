#include <Windows.h>
#include <string>

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
			keybd_event(VK_SPACE, 0, 0, 0);
			Sleep(500);
			keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
		}

	} while (!GetAsyncKeyState(VK_END));
}