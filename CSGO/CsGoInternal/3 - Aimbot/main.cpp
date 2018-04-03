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

class MyPlayerClass {
public:
	char pad_0000[240]; //0x0000
	int32_t iTeamNum; //0x00F0
	char pad_00F4[8]; //0x00F4
	int32_t iHealth; //0x00FC
	char pad_0100[4]; //0x0100
	Vector3_t vecView; //0x0104
	char pad_0110[36]; //0x0110
	Vector3_t vecPosition; //0x0134
};
MyPlayerClass* MyPlayer;

class BoneClass
{
public:
	char pad_0000[12]; //0x0000
	float x; //0x000C
	char pad_0010[12]; //0x0010
	float y; //0x001C
	char pad_0020[12]; //0x0020
	float z; //0x002C
}; //Size: 0x0030

class BoneList
{
public:
	BoneClass Bone0[9]; //0x0000
}; //Size: 0x01B0

class EntityClass
{
public:
	char pad_0000[233]; //0x0000
	bool bDormant; //0x00E9
	char pad_00EA[6]; //0x00EA
	int32_t iTeamNum; //0x00F0
	char pad_00F4[8]; //0x00F4
	int32_t iHealth; //0x00FC
	char pad_0100[4]; //0x0100
	Vector3_t vecView; //0x0104
	char pad_0110[36]; //0x0110
	Vector3_t vecPosition; //0x0134
	char pad_0140[9560]; //0x0140
	BoneList* pBonesArray; //0x2698
	char pad_269C[2676]; //0x269C
}; //Size: 0x3110

class EntityPaddingClass {
public:
	EntityClass* entity;
	char pad[12]; //0x0004
};

class EntList {
public:
	EntityPaddingClass entities[20];
};
EntList* entList = NULL;

float Calc3dDistance(const Vector3_t* src, const Vector3_t* dst) {
	return sqrt(
		pow(src->x - dst->x, 2) +
		pow(src->y - dst->y, 2) +
		pow(src->z - dst->z, 2)
	);
}

EntityClass* GetClosestEnemy() {
	EntityClass* closest = NULL;
	float closestfDistance;
	int iInGamePlayersNum = *(int*)(dwServerModule + 0x9E5880);

	for (int i = 0; i < iInGamePlayersNum; i++) {
		if (entList->entities[i].entity->bDormant == false && entList->entities[i].entity->iTeamNum != MyPlayer->iTeamNum && entList->entities[i].entity->iHealth > 1) {
			float fDistance = Calc3dDistance(&MyPlayer->vecPosition, &entList->entities[i].entity->vecPosition);
			
			if (closest == NULL || fDistance < closestfDistance) {
				closestfDistance = fDistance;
				closest = entList->entities[i].entity;
			}
		}
	}

	return closest;
}

Vector3_t CalculateAngle2(const Vector3_t* src, const BoneClass* dst) {
	Vector3_t angles;
	Vector3_t e = { dst->x - src->x, dst->y - src->y, dst->z - src->z };
	float eh = sqrt(e.x * e.x + e.y * e.y);

	angles.x = atan2(-e.z, eh) * 180 / PI;
	angles.y = atan2(e.y, e.x) * 180 / PI;
	angles.z = 0;

	return angles;
}

void Aimbot() {
	if (GetAsyncKeyState(VK_INSERT) & 1)
		isTriggerOn = !isTriggerOn;

	if (isTriggerOn && *dwClientState == inGame) {
		if (MyPlayer == NULL)
			MyPlayer = (MyPlayerClass*)(*(DWORD*)(dwClientModule + dwPlayerBaseOffset));

		if (entList == NULL)
			entList = (EntList*)(dwClientModule + dwEntityListOffset);

		EntityClass* closestEnemy = GetClosestEnemy();
		if (closestEnemy != NULL) {
			Vector3_t myPlayerViewPosition = { MyPlayer->vecPosition.x, MyPlayer->vecPosition.y, MyPlayer->vecPosition.z + MyPlayer->vecView.z };
			Vector3_t* myPlayerViewAngles = (Vector3_t*)(*dwClientStateObject + 0x4D10);
			*myPlayerViewAngles = CalculateAngle2(&myPlayerViewPosition, &closestEnemy->pBonesArray->Bone0[8]);
		}
	}
}

DWORD WINAPI Main_Thread(LPVOID lParam) {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

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