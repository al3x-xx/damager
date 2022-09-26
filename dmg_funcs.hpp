#ifndef DMG_FUNCS_HPP
#define DMG_FUNCS_HPP

#include <chrono>

#include "misc_funcs.hpp"

class dmgFuncs : public miscFuncs {
public:
	dmgFuncs();
	~dmgFuncs();

	static inline std::unique_ptr<dmgFuncs> i;
private:
	static void __cdecl commandFunc(const char* buf);
public:
	[[nodiscard]] bool checkActivate() const noexcept;

	[[nodiscard]] const char* getModeStr(std::uint8_t mode) const noexcept;

	[[nodiscard]] bool remotePlayerIsListed(std::uint16_t id) const noexcept;
	[[nodiscard]] bool remotePlayerIsExist(std::uint16_t id, bool dead = true) const noexcept;

	[[nodiscard]] vector3 getLocalPlayerPos(bool invis = false) const noexcept;
	[[nodiscard]] vector3 getRemotePlayerPos(std::uint16_t id) const noexcept;

	[[nodiscard]] vector3 getRemotePlayerBonePos(std::uint16_t id, int boneId) const;

	[[nodiscard]] std::uint8_t getExtZoom(std::uint8_t gun) const;
	[[nodiscard]] std::uint8_t getWeaponState(std::uint8_t gun) const;

	[[nodiscard]] std::chrono::nanoseconds getDelay() const;

	[[nodiscard]] std::uint16_t getPlayerIdForDmg() const;
	void processDmg(std::uint16_t id) const;
private:
	[[nodiscard]] bool localPlayerInVehicle() const noexcept;
	[[nodiscard]] bool localPlayerIsPassenger() const noexcept;
	
	[[nodiscard]] std::uint8_t getCamMode(std::uint8_t gun) const;

	[[nodiscard]] vector3 calcVecAimPos(const vector3& targetPos) const noexcept;
	[[nodiscard]] float calcfAimZ(float aim) const noexcept;

	void processPlayerVehPassSync(std::uint16_t id) const;
	void processAimSync(std::uint16_t id, const vector3& targetPos) const;
	void processBulletSync(std::uint16_t id, const vector3& targetPos, bool isMiss) const;
	void processGiveDamage(std::uint16_t id) const;
	void processReduceAmmo() const noexcept;
	void processWeaponSync(std::uint16_t id, bool isMiss) const;
	void processPackets(std::uint16_t id, bool isMiss = false) const;

	[[nodiscard]] std::uint16_t processPlayerId(bool blackList = false) const;

	[[nodiscard]] bool checkSkin(std::uint16_t id) const;

	[[nodiscard]] bool remotePlayerInVehicle(std::uint16_t id) const noexcept;
};

#endif