#include <Windows.h>
#include <vector>

class PlayerEntity
{
public:
	void* vTable;

	struct HeadCoordinates {
		float x; //0x0004
		float y;
		float z;
	}headCoordinates;

	char pad_0010[36]; //0x0010

	struct Coordinates {
		float x; //0x0034
		float y;
		float z;
	}coordinates;

	float Yaw; //0x0040
	float Pitch; //0x0044
	char pad_0048[32]; //0x0048
	int32_t JumpFlag; //0x0068
	char pad_006C[140]; //0x006C
	int32_t Health; //0x00F8
	int32_t Armor; //0x00FC
	char pad_0100[60]; //0x0100
	int32_t PistolClips; //0x013C
	char pad_0140[16]; //0x0140
	int32_t ARClips; //0x0150
	char pad_0154[4]; //0x0154
	int32_t Granades; //0x0158
	char pad_015C[68]; //0x015C
	int32_t ARClipsShot; //0x01A0
	char pad_01A4[96]; //0x01A4
	int32_t TeamNum; //0x0204
	char pad_0208[28]; //0x0208
	bool Shoot; //0x0224
	char Name[16]; //0x0225
}; //Size: 0x045E
