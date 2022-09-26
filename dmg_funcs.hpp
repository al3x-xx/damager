#ifndef DMG_FUNCS_HPP
#define DMG_FUNCS_HPP

#include <chrono>

#include "misc_funcs.hpp"

class dmgFuncs : public miscFuncs {
	dmgFuncs();
	~dmgFuncs();

	static void __cdecl commandFunc(char* buf);
public:
	static dmgFuncs& i();

	[[nodiscard]] bool checkActivate() const;

	[[nodiscard]] const char* getModeStr(std::uint8_t mode) const;

	[[nodiscard]] bool remotePlayerIsListed(std::uint16_t id) const;
	[[nodiscard]] bool remotePlayerIsExist(std::uint16_t id, bool dead = true) const;

	[[nodiscard]] vector3 getLocalPlayerPos(bool wInvis = false) const;
	[[nodiscard]] vector3 getRemotePlayerPos(std::uint16_t id) const;

	[[nodiscard]] vector3 getRemotePlayerBonePos(std::uint16_t id, int boneID) const;

	[[nodiscard]] std::uint8_t getExtZoom(std::uint8_t gun) const;
	[[nodiscard]] std::uint8_t getWeaponState(std::uint8_t gun) const;

	[[nodiscard]] std::chrono::nanoseconds getDelay() const;

	[[nodiscard]] std::uint16_t getPlayerIdForDMG() const;
	void processDMG(std::uint16_t id) const;
private:
	[[nodiscard]] bool localPlayerInVehicle() const;
	[[nodiscard]] bool localPlayerIsPassenger() const;
	
	[[nodiscard]] std::uint8_t getCamMode(std::uint8_t gun) const;

	[[nodiscard]] vector3 calcVecAimPos(const vector3& targetPos) const;
	[[nodiscard]] float calcfAimZ(float aim) const;

	void processPlayerVehPassSync(std::uint16_t id) const;
	void processAimSync(std::uint16_t id, const vector3& targetPos) const;
	void processBulletSync(std::uint16_t id, const vector3& targetPos, bool isMiss) const;
	void processGiveDamage(std::uint16_t id) const;
	void processReduceAmmo() const;
	void processWeaponSync(std::uint16_t id, bool isMiss) const;
	void processPackets(std::uint16_t id, bool isMiss = false) const;

	[[nodiscard]] std::uint16_t processPlayerId(bool wBlackList = false) const;

	[[nodiscard]] bool checkSkin(std::uint16_t id) const;

	[[nodiscard]] bool remotePlayerInVehicle(std::uint16_t id) const;
};

#endif // DMG_FUNCS_HPP