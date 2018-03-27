#include <Windows.h>

DWORD dwClientStateObjectAddress = 0x57D894;
DWORD dwClientStateOffset = 0x108;
DWORD dwEntityListOffset = 0x4A80244;
DWORD dwPlayerBaseOffset = 0xAA3154;
DWORD dwForceAttackOffset = 0x2EC2600;

DWORD dwClientModule = NULL;
DWORD dwEngineModule = NULL;
DWORD* dwClientState = NULL;
DWORD* dwForceAttack = NULL;

int inGame = 6;
bool isTriggerOn = false;

struct PlayerEntity_t {
	DWORD dwPlayerEntity;
	int iTeam;
	int iHealth;
	int crosshairEntityId;

	void ReadInfo() {
		dwPlayerEntity = *(DWORD*)(dwClientModule + dwPlayerBaseOffset);
		iTeam = *(int*)(dwPlayerEntity + 0xF0);
		iHealth = *(int*)(dwPlayerEntity + 0xFC);
		crosshairEntityId = *(int*)(dwPlayerEntity + 0xB2A4);
	}
}Player;

struct Entity_t {
	DWORD dwEntity;
	int iTeam;
	int iHealth;

	void ReadInfo(int entityIndex) {
		dwEntity = *(DWORD*)(dwClientModule + dwEntityListOffset + ((entityIndex - 1) * 0x10));
		iTeam = *(int*)(dwEntity + 0xF0);
		iHealth = *(int*)(dwEntity + 0xFC);
	}
}Entity;

void Triggerbot() {
	if (GetAsyncKeyState(VK_INSERT) & 1)
		isTriggerOn = !isTriggerOn;

	if (isTriggerOn && *dwClientState == inGame) {
		Player.ReadInfo();

		if (Player.crosshairEntityId > 0 && Player.crosshairEntityId < 32) {
			Entity.ReadInfo(Player.crosshairEntityId);

			if (Player.iTeam != Entity.iTeam) {
				*dwForceAttack = 5;
				Sleep(25);
				*dwForceAttack = 4;
				Sleep(25);
			}
		}
	}
}

DWORD WINAPI Main_Thread(LPVOID lParam) {
	do {
		dwClientModule = (DWORD)GetModuleHandleA("client.dll");
		dwEngineModule = (DWORD)GetModuleHandleA("engine.dll");
	} while (dwClientModule == NULL && dwEngineModule == NULL);

	DWORD dwClientStateObject = *(DWORD*)(dwEngineModule + dwClientStateObjectAddress);
	dwClientState = (DWORD*)(dwClientStateObject + dwClientStateOffset);
	dwForceAttack = (DWORD*)(dwClientModule + dwForceAttackOffset);

	while (!GetAsyncKeyState(VK_ESCAPE))
		Triggerbot();

	return S_OK;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0x1000, &Main_Thread, 0, 0, NULL);
	}

	return TRUE;
}