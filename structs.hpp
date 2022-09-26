#ifndef STRUCTS_HPP
#define STRUCTS_HPP

struct vector3 {
	vector3(float x = 0.f, float y = 0.f, float z = 0.f) noexcept : x{ x }, y{ y }, z{ z } {}
	float x, y, z;
};

#pragma pack(push, 1)

// blast.hk

struct stOnFootData {
	std::uint16_t sLeftRightKeys;
	std::uint16_t sUpDownKeys;
	std::uint16_t sKeys;
	float fPosition[3];
	float fQuaternion[4];
	std::uint8_t byteHealth;
	std::uint8_t byteArmor;
	std::uint8_t byteCurrentWeapon;
	std::uint8_t byteSpecialAction;
	float fMoveSpeed[3];
	float fSurfingOffsets[3];
	std::uint16_t sSurfingVehicleID;
	short sCurrentAnimationID;
	short sAnimFlags;
};

struct stInCarData {
	std::uint16_t sVehicleID;
	std::uint16_t sLeftRightKeys;
	std::uint16_t sUpDownKeys;
	std::uint16_t sKeys;
	float fQuaternion[4];
	float fPosition[3];
	float fMoveSpeed[3];
	float fVehicleHealth;
	std::uint8_t bytePlayerHealth;
	std::uint8_t byteArmor;
	std::uint8_t byteCurrentWeapon;
	std::uint8_t byteSiren;
	std::uint8_t byteLandingGearState;
	std::uint16_t sTrailerID;
	union {
		std::uint16_t HydraThrustAngle[2];
		float fTrainSpeed;
	};
};

struct stPassengerData {
	std::uint16_t sVehicleID;
	std::uint8_t byteSeatID;
	std::uint8_t byteCurrentWeapon;
	std::uint8_t byteHealth;
	std::uint8_t byteArmor;
	std::uint16_t sLeftRightKeys;
	std::uint16_t sUpDownKeys;
	std::uint16_t sKeys;
	float fPosition[3];
};

struct stBulletData {
	std::uint8_t byteType;
	std::uint16_t sTargetID;
	float fOrigin[3];
	float fTarget[3];
	float fCenter[3];
	std::uint8_t byteWeaponID;
};

struct stAimData {
	std::uint8_t byteCamMode;
	float vecAimf1[3];
	float vecAimPos[3];
	float fAimZ;
	std::uint8_t byteCamExtZoom : 6;
	std::uint8_t byteWeaponState : 2;
	std::uint8_t bUnk;
};

struct stWeaponsData {
	std::uint16_t playerTarget;
	std::uint16_t actorTarget;
	std::uint8_t slot;
	std::uint8_t weapon;
	std::uint16_t ammo;
};

struct stGiveTakeDamage {
	std::uint16_t id;
	float damage;
	std::uint32_t weapon;
	std::uint32_t bodypart;
};

using cmdProc = void(__cdecl*)(const char*);

struct stInputInfo {
private:
	char skip[12];
public:
	cmdProc pCMDs[144];
	char szCMDNames[144][33];
private:
	char skip2[4];
public:
	int iInputEnabled;
};

struct object_base {
private:
	char skip[20];
public:
	float* matrix;
private:
	char skip2[10];
public:
	std::uint16_t model_alt_id;
private:
	char skip3[30];
};

struct weapon_ {
private:
	char skip[8];
public:
	int ammo_clip;
	int ammo;
private:
	char skip2[12];
};

struct vehicle_info {
private:
	char skip[1120];
public:
	void* passengers[9];
};

struct actor_info {
	object_base base;
private:
	char skip[1066];
public:
	std::uint8_t pedFlags[16];
private:
	char skip2[180];
public:
	std::uint32_t state;
private:
	char skip3[88];
public:
	vehicle_info* vehicle;
private:
	char skip4[16];
public:
	weapon_ weapon[13];
};

struct stSAMPPed {
private:
	char skip[676];
public:
	actor_info* pGTA_Ped;
};

#pragma pack(pop)

#endif