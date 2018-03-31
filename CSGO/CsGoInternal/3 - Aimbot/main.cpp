#define _CRT_SECURE_NO_WARNINGS
#define PI 3.1415927f

#include <Windows.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>

DWORD dwClientStateObjectAddress = 0x57D894;
DWORD dwClientStateOffset = 0x108;

DWORD dwEntityListOffset = 0x4A8387C;
DWORD dwPlayerBaseOffset = 0xAA6614;
DWORD dwInGamePlayerNumOffset = 0x4A6F3A4;

DWORD dwEntityOriginVect = 0x134;

DWORD dwClientModule = NULL;
DWORD dwEngineModule = NULL;
DWORD dwServerModule = NULL;
DWORD* dwClientStateObject = NULL;
DWORD* dwClientState = NULL;

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
};
MyPlayer_t* MyPlayer = NULL;

struct Entity_t {
	DWORD* dwEntityBase = NULL;
	int* iHealth = NULL;
	int* iTeam = NULL;
	Vector3_t* vecPosition = NULL;
	float fDistance;
};

float Calc3dDistance(Vector3_t* src, Vector3_t* dst) {
	return sqrt(
		pow(src->x - dst->x, 2) +
		pow(src->y - dst->y, 2) +
		pow(src->z - dst->z, 2)
	);
}

Entity_t GetClosestEnemy() {
	Entity_t closest;
	Entity_t tempEntity;
	int iInGamePlayersNum = *(int*)(dwServerModule + 0x9E5880);

	for (int i = 0; i < iInGamePlayersNum; i++) {
		tempEntity.dwEntityBase = (DWORD*)(dwClientModule + dwEntityListOffset + i * 0x10);
		tempEntity.iHealth = (int*)(*tempEntity.dwEntityBase + 0xFC);
		tempEntity.iTeam = (int*)(*tempEntity.dwEntityBase + 0xF0);
		tempEntity.vecPosition = (Vector3_t*)(*tempEntity.dwEntityBase + 0x134);

		if (*tempEntity.iTeam != *MyPlayer->iTeam && *tempEntity.iHealth > 1) {
			tempEntity.fDistance = Calc3dDistance(MyPlayer->vecPosition, tempEntity.vecPosition);
			
			if (closest.dwEntityBase == NULL || tempEntity.fDistance < closest.fDistance)
				closest = tempEntity;
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
		if (MyPlayer == NULL) {
			MyPlayer = new MyPlayer_t;
			MyPlayer->ReadInfo();
		}

		Entity_t closestEnemy = GetClosestEnemy();
		if (closestEnemy.dwEntityBase != NULL) {
			Vector3_t angles = CalculateAngle(MyPlayer->vecPosition, closestEnemy.vecPosition);
			*MyPlayer->vecAimAngle = angles;
		}
	} else {
		if (MyPlayer != NULL) {
			delete MyPlayer;
			MyPlayer = NULL;
		}
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