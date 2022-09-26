#include "misc_funcs.hpp"

#include <d3d9.h>

#include <unordered_map>
#include <charconv>

#include "lemon/mem.hpp"

std::string miscFuncs::dobavitZapyatuyuIindex(const std::vector<std::string>& vec) const {
	std::string ret;
	for (const auto [i, buf] : vec | std::ranges::views::enumerate) {
		ret += (i ? ", " : "") + buf + "[" + std::to_string(i + 1) + "]";
	}
	return ret;
}

void miscFuncs::setKeyState(std::uint8_t key, bool state) noexcept {
	keyState_[key] = state;
}

std::string miscFuncs::getKeyNameById(std::uint32_t id) const {
	char buf[16]{};
	GetKeyNameTextA(MapVirtualKeyA(id, MAPVK_VK_TO_VSC) << 16, buf, sizeof(buf));
	return buf;
}

bool miscFuncs::checkIdLimit(int id) const noexcept {
	return id < sampFuncs::playerLimit && id >= 0;
}

std::uint32_t miscFuncs::calcNameColor(std::uint32_t color) const noexcept {
	return D3DCOLOR_XRGB(color >> 24 & 0xFF, color >> 16 & 0xFF, color >> 8 & 0xFF);
}

const POINT& miscFuncs::getScreenRes() const noexcept {
	return *reinterpret_cast<POINT*>(0xC9C040);
}

vector3 miscFuncs::calcScreenCoords(const vector3& vecWorld) const noexcept {
	const auto m{ reinterpret_cast<float*>(0xB6FA2C) };
	vector3 vecScreen{
		vecWorld.z * m[8] + vecWorld.y * m[4] + vecWorld.x * m[0] + m[12],
		vecWorld.z * m[9] + vecWorld.y * m[5] + vecWorld.x * m[1] + m[13],
		vecWorld.z * m[10] + vecWorld.y * m[6] + vecWorld.x * m[2] + m[14]
	};

	const auto r{ 1.f / vecScreen.z };

	vecScreen.x *= r * *reinterpret_cast<std::uint32_t*>(0xC17044);
	vecScreen.y *= r * *reinterpret_cast<std::uint32_t*>(0xC17048);

	return vecScreen;
}

float miscFuncs::getWeaponDamage(std::uint8_t gun) const {
	static const std::unordered_map<std::uint8_t, float> weaponDamage{
		{ 0, 5.280000f }, { 1, 5.280000f }, { 2, 5.280000f }, { 3, 5.280000f }, { 4, 5.280000f }, { 5, 5.280000f }, { 6, 5.280000f }, { 7, 5.280000f }, { 8, 5.280000f }, { 9, 27.060001f }, { 10, 5.280000f }, { 11, 5.280000f }, { 12, 5.280000f }, { 13, 5.280000f }, { 14, 5.280000f }, { 15, 5.280000f }, { 18, 0.495000f },
		{ 22, 8.250000f }, { 23, 13.200001f }, { 24, 46.200001f }, { 25, 49.500004f }, { 26, 49.500004f }, { 27, 39.600002f }, { 28, 6.60000f }, { 29, 8.250000f }, { 30, 9.900001f }, { 31, 9.900001f }, { 32, 6.60000f }, { 33, 24.750002f }, { 34, 41.25000f },
		{ 37, 0.495000f },
		{ 38, 46.200001f },
		{ 41, 0.330000f }, { 42, 0.330000f }, { 46, 6.600000f }
	};
	const auto res{ weaponDamage.find(gun) };
	return res != weaponDamage.end() ? res->second : 0.f;
}

std::uint8_t miscFuncs::getWeaponSlot(std::uint8_t gun) const {
	static const std::unordered_map<std::uint8_t, std::uint8_t> weaponSlot{
		{ 0, 0 }, { 1, 0 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 1 }, { 9, 1 }, { 10, 10 }, { 11, 10 }, { 12, 10 }, { 13, 10 }, { 14, 10 }, { 15, 10 }, { 16, 8 }, { 17, 8 }, { 18, 8 },
		{ 22, 2 }, { 23, 2 }, { 24, 2 }, { 25, 3 }, { 26, 3 }, { 27, 3 }, { 28, 4 }, { 29, 4 }, { 30, 5 }, { 31, 5 }, { 32, 4 }, { 33, 6 }, { 34, 6 },
		{ 37, 7 },
		{ 38, 7 },
		{ 41, 9 }, { 42, 9 }, { 46, 11 }
	};
	const auto res{ weaponSlot.find(gun) };
	return res != weaponSlot.end() ? res->second : 0;
}

std::uint8_t miscFuncs::getWeaponId(const char* str) const {
	static const std::unordered_map<std::string_view, std::uint8_t> weaponId{
		{ "Fist", 0 }, { "Brass Knuckles", 1 }, { "Golf Club", 2 }, { "Nitestick", 3 }, { "Knife", 4 }, { "Baseball Bat", 5 }, { "Shovel", 6 }, { "Pool Cue", 7 }, { "Katana", 8 }, { "Chainsaw", 9 }, { "Dildo 1", 10 }, { "Dildo 2", 11 }, { "Vibe 1", 12 }, { "Vibe 2", 13 }, { "Flowers", 14 }, { "Cane", 15 }, { "Molotov Cocktail", 18 }, 
		{ "Pistol", 22 }, { "Silenced Pistol", 23 }, { "Desert Eagle", 24 }, { "Shotgun", 25 }, { "Sawn-Off Shotgun", 26 }, { "SPAZ12", 27 }, { "Micro UZI", 28 }, { "MP5", 29 }, { "AK47", 30 }, { "M4", 31 }, { "Tech9", 32 }, { "Country Rifle", 33 }, { "Sniper Rifle", 34 },
		{ "Flame Thrower", 37 },
		{ "Minigun", 38 },
		{ "Spray Can", 41 }, { "Fire Extinguisher", 42 }, { "Parachute", 46 }
	};
	const auto res{ weaponId.find(str) };
	return res != weaponId.end() ? res->second : 0;
}

const char* miscFuncs::getWeaponName(std::uint8_t gun) const {
	static const std::unordered_map<std::uint8_t, const char*> weaponName{
		{ 0, "Fist" }, { 1, "Brass Knuckles" }, { 2, "Golf Club" }, { 3, "Nitestick" }, { 4, "Knife" }, { 5, "Baseball Bat" }, { 6, "Shovel" }, { 7, "Pool Cue" }, { 8, "Katana" }, { 9, "Chainsaw" }, { 10, "Dildo 1" }, { 11, "Dildo 2" }, { 12, "Vibe 1" }, { 13, "Vibe 2" }, { 14, "Flowers" }, { 15, "Cane" }, { 18, "Molotov Cocktail" },
		{ 22, "Pistol" }, { 23, "Silenced Pistol" }, { 24, "Desert Eagle" }, { 25, "Shotgun" }, { 26, "Sawn-Off Shotgun" }, { 27, "SPAZ12" }, { 28, "Micro UZI" }, { 29, "MP5" }, { 30, "AK47" }, { 31, "M4" }, { 32, "Tech9" }, { 33, "Country Rifle" }, { 34, "Sniper Rifle" },
		{ 37, "Flame Thrower" },
		{ 38, "Minigun" },
		{ 41, "Spray Can" }, { 42, "Fire Extinguisher" }, { 46, "Parachute" }
	};
	const auto res{ weaponName.find(gun) };
	return res != weaponName.end() ? res->second : "";
}

bool miscFuncs::isFist(std::uint8_t gun) const {
	return std::ranges::contains(std::ranges::views::iota(0, 16), gun) || gun == 18 || gun == 37 || gun == 41 || gun == 42 || gun == 46;
}

bool miscFuncs::isWeapon(std::uint8_t gun) const {
	return std::ranges::contains(std::ranges::views::iota(22, 35), gun) || gun == 38;
}

bool miscFuncs::isWeaponForfAimZ(std::uint8_t gun) const {
	static const std::uint8_t id[]{ 23, 24, 25, 27, 29, 30, 31, 33, 37, 38, 41, 42 };
	return std::ranges::contains(id, gun);
}

void miscFuncs::toggleCursorState(bool state) const {
	lemon::mem::safe_copy(reinterpret_cast<void*>(0x53F417), state ? "\x90\x90\x90\x90\x90" : "\xE8\xB4\x7A\x20\x00", 5);
	lemon::mem::safe_copy(reinterpret_cast<void*>(0x53F41F), state ? "\x33\xC0\x0F\x84" : "\x85\xC0\x0F\x8C", 4);
	lemon::mem::safe_copy(reinterpret_cast<void*>(0x6194A0), state ? "\xC3" : "\xE9", 1);

	(*reinterpret_cast<IDirect3DDevice9**>(0xC97C28))->ShowCursor(state);

	*reinterpret_cast<float*>(0xB73424) = 0.f;
	*reinterpret_cast<float*>(0xB73428) = 0.f;

	reinterpret_cast<void(__cdecl*)()>(0x541BD0)();
	reinterpret_cast<void(__cdecl*)()>(0x541DD0)();
}

std::optional<int> miscFuncs::strToInt(const std::string_view& str) const noexcept {
	int value_;
	return std::from_chars(str.data(), str.data() + str.size(), value_).ec == std::errc{} ? value_ : std::optional<int>{};
}

std::tuple<float, float> miscFuncs::calcQuaternion(float angle) const noexcept {
	return { std::cosf(angle / 2.f), std::sinf(angle / 2.f) };
}

float miscFuncs::getVecLen(const vector3& vec) const noexcept {
	return std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float miscFuncs::getDistance(const vector3& vec, const vector3& vec2) const noexcept {
	return std::sqrtf((vec.x - vec2.x) * (vec.x - vec2.x) + (vec.y - vec2.y) * (vec.y - vec2.y) + (vec.z - vec2.z) * (vec.z - vec2.z));
}

float miscFuncs::getDistance(float x, float y, float x2, float y2) const noexcept {
	return std::sqrtf((x - x2) * (x - x2) + (y - y2) * (y - y2));
}

char miscFuncs::getWeaponSkill(std::uint8_t gun) const { // thx AdCKuY_DpO4uLa
	return reinterpret_cast<char(__thiscall*)(void*, std::uint8_t)>(0x5E3B60)(sampFuncs::i->getLocalSampPed()->pGTA_Ped, gun);
}

bool miscFuncs::isLineOfSightClear(const vector3* orig, const vector3* target, bool build, bool veh, bool obj) const {
	return reinterpret_cast<bool(__cdecl*)(const vector3*, const vector3*, bool, bool, bool, bool, bool, bool, bool)>(0x56A490)(orig, target, build, veh, false, obj, false, false, false);
}