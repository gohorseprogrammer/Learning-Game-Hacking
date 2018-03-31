#define _CRT_SECURE_NO_WARNINGS
#define PI 3.1415927f

#include <Windows.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>

DWORD dwClientStateObjectAddress = 0x57D894;
DWORD dwClientStateOffset = 0x108;
DWORD dwClientStateMaxPlayerOffset = 0x310;

DWORD dwEntityListOffset = 0x4A80244;
DWORD dwPlayerBaseOffset = 0xAA3154;
DWORD dwInGamePlayerNumOffset = 0x4A6F3A4;

DWORD dwEntityOriginVect = 0x134;

DWORD dwClientModule = NULL;
DWORD dwEngineModule = NULL;
DWORD dwServerModule = NULL;
DWORD* dwClientStateObject = NULL;
DWORD* dwClientState = NULL;
int iInGamePlayersNum = NULL;

int inGame = 6;
bool isTriggerOn = false;

struct Vector3_t {
	float x;
	float y;
	float z;
};

struct MyPlayer_t {
	DWORD* dwPlayerBase;
	int* iHealth;
	int* iTeam;
	Vector3_t* vecPosition;
	Vector3_t* vecAimAngle;

	void ReadInfo() {
		dwPlayerBase = (DWORD*)(dwClientModule + dwPlayerBaseOffset);
		iHealth = (int*)(*dwPlayerBase + 0xFC);
		iTeam = (int*)(*dwPlayerBase + 0xF0);
		vecPosition = (Vector3_t*)(*dwPlayerBase + 0x134);
		vecAimAngle = (Vector3_t*)(*dwClientStateObject + 0x4D10);
	}
}MyPlayer;

struct Entity_t {
	MyPlayer_t* p_MyPlayer;
	DWORD* dwEntityBase;
	int* iHealth;
	int* iTeam;
	Vector3_t* vecPosition;
	float fDistance;

	void ReadInfo(int entityIndex) {
		p_MyPlayer = &MyPlayer;
		dwEntityBase = (DWORD*)(dwClientModule + dwEntityListOffset + entityIndex * 0x10);
		iHealth = (int*)(*dwEntityBase + 0xFC);
		iTeam = (int*)(*dwEntityBase + 0xF0);
		vecPosition = (Vector3_t*)(*dwEntityBase + 0x134);
	}

	void Calc3dDistance() {
		fDistance = sqrt(
			pow(p_MyPlayer->vecPosition->x - vecPosition->x, 2) +
			pow(p_MyPlayer->vecPosition->y - vecPosition->y, 2) +
			pow(p_MyPlayer->vecPosition->z - vecPosition->z, 2)
		);
	}
};

Entity_t* Entities = NULL;
Entity_t* GetClosestEnemy() {
	Entity_t* closest = NULL;

	for (int i = 0; i < iInGamePlayersNum; i++) {
		if (*Entities[i].iTeam != *MyPlayer.iTeam && *Entities[i].iHealth > 1) {
			Entities[i].Calc3dDistance();
			
			if (closest == NULL) {
				closest = &Entities[i];
			} else {
				if (Entities[i].fDistance < closest->fDistance)
					closest = &Entities[i];
			}
		}
	}

	return closest;
}

Vector3_t CalculateAngle(Vector3_t* src, Vector3_t* dst) {
	Vector3_t angles;
	
	Vector3_t delta = { (src->x - dst->x), (src->y - dst->y), (src->z - dst->z) };
	float distance = sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);

	angles.x = -((atan2(dst->z - src->z, distance)) * 180.0f / PI);
	angles.y = (-(float)atan2(dst->x - src->x, dst->y - src->y)) / PI * 180.0f + 90.0f; 
	angles.z = 0.0f;

	return angles;
}

void Aimbot() {
	if (GetAsyncKeyState(VK_INSERT) & 1)
		isTriggerOn = !isTriggerOn;

	if (isTriggerOn && *dwClientState == inGame) {
		iInGamePlayersNum = *(int*)(dwServerModule + 0x9E16F0);
		MyPlayer.ReadInfo();

		Entities = new Entity_t[iInGamePlayersNum];

		for (int i = 0; i < iInGamePlayersNum; i++)
			Entities[i].ReadInfo(i);

		Entity_t* closestEnemy = GetClosestEnemy();
		if (closestEnemy != NULL) {
 			Vector3_t angles = CalculateAngle(MyPlayer.vecPosition, closestEnemy->vecPosition);
			*MyPlayer.vecAimAngle = angles;
		}

		delete[] Entities;
	}
}

DWORD WINAPI Main_Thread(LPVOID lParam) {
	do {
		dwClientModule = (DWORD)GetModuleHandleA("client.dll");
		dwEngineModule = (DWORD)GetModuleHandleA("engine.dll");
		dwServerModule = (DWORD)GetModuleHandleA("server.dll");
	} while (dwClientModule == NULL || dwClientModule == 0
			&& dwEngineModule == NULL || dwEngineModule == 0
			&& dwServerModule == NULL || dwServerModule == 0);

	dwClientStateObject = (DWORD*)(dwEngineModule + dwClientStateObjectAddress);
	dwClientState = (DWORD*)(*dwClientStateObject + dwClientStateOffset);

	while (!GetAsyncKeyState(VK_ESCAPE) & 1)
		Aimbot();

	FreeLibraryAndExitThread((HMODULE)lParam, 0);

	return S_OK;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0x1000, &Main_Thread, 0, 0, NULL);
	}

	return TRUE;
}