#include <Windows.h>

DWORD dwClientStateAddress = 0x57D894;
DWORD dwEntityListAddress = 0x4A80244;
DWORD dwPlayerBaseAddress = 0xAA3154;
DWORD dwForceAttackAddress = 0x2EC2600;

DWORD dwClientStateStateOffset = 0x108;
DWORD dwMaxPlayerOffset = 0x310;
DWORD dwTeamOffset = 0xF0;
DWORD dwCrossHairOffset = 0xB2A4;
DWORD dwEntityLoopDistance = 0x10;

HMODULE hClientModule = NULL;
HMODULE hEngineModule = NULL;

DWORD* dwEntityList = NULL;
DWORD* dwPlayerBase = NULL;
DWORD* dwClientState = NULL;
DWORD* dwClientStateState = NULL;
DWORD* dwForceAttack = NULL;

int iMaxPlayers = 32;

bool isActive = false;

DWORD WINAPI Main_Thread(LPVOID lParam) {
	do {
		hClientModule = GetModuleHandleA("client.dll");
		hEngineModule = GetModuleHandleA("engine.dll");
	} while (hClientModule == NULL && hEngineModule == NULL);

	dwClientState = (DWORD*)((DWORD)hEngineModule + dwClientStateAddress);
	dwClientStateState = (DWORD*)(*dwClientState + dwClientStateStateOffset);
	dwForceAttack = (DWORD*)((DWORD)hClientModule + dwForceAttackAddress);

	while (true) {
		if (GetAsyncKeyState(VK_INSERT) & 1)
			isActive = !isActive;

		if (isActive && *dwClientStateState == 6) {
			if (*dwForceAttack == 5)
				*dwForceAttack = 4;

			dwEntityList = (DWORD*)((DWORD)hClientModule + dwEntityListAddress);
			dwPlayerBase = (DWORD*)((DWORD)hClientModule + dwPlayerBaseAddress);
			iMaxPlayers = *(int*)(dwClientState + dwMaxPlayerOffset);

			int playerTeam = *(int*)(*dwPlayerBase + dwTeamOffset);
			int crosshairEntityId = *(int*)(*dwPlayerBase + dwCrossHairOffset);

			if (crosshairEntityId != 0 && crosshairEntityId < iMaxPlayers) {
				DWORD crosshairEntity = *(DWORD*)((DWORD)dwEntityList + ((crosshairEntityId - 1) * dwEntityLoopDistance));
				if (crosshairEntity != NULL) {
					int crosshairEntityTeam = *(int*)(crosshairEntity + dwTeamOffset);

					if (playerTeam != crosshairEntityTeam && *dwForceAttack == 4)
						*dwForceAttack = 5;
				}
			}
		}
	}

	return S_OK;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0x1000, &Main_Thread, 0, 0, NULL);
	}

	return TRUE;
}