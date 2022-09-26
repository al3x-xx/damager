#ifndef DMG_FUNCS_HPP
#define DMG_FUNCS_HPP

#include "misc_funcs.hpp"

class dmgFuncs : public miscFuncs {
	dmgFuncs();
	~dmgFuncs();

	static void __cdecl commandFunc(char* buf);
public:
	static dmgFuncs& i();

	bool checkActivate() const;

	[[nodiscard]] const char* getModeStr(std::uint8_t mode) const;

	bool remotePlayerIsListed(std::uint16_t id) const;
	bool remotePlayerIsExist(std::uint16_t id, bool dead = true) const;

	vector getLocalPlayerPos(bool wInvis = false) const;
	vector getRemotePlayerPos(std::uint16_t id) const;

	vector getRemotePlayerBonePos(std::uint16_t id, int boneID) const;

	[[nodiscard]] std::uint8_t getExtZoom(std::uint8_t gun) const;
	[[nodiscard]] std::uint8_t getWeaponState(std::uint8_t gun) const;

	int getDelay() const;

	std::uint16_t getPlayerIdForDMG() const;
	void processDMG(std::uint16_t id) const;
private:
	bool localPlayerInVehicle() const;
	bool localPlayerIsPassenger() const;
	
	std::uint8_t getCamMode(std::uint8_t gun) const;

	vector calcVecAimPos(const vector& targetPos) const;
	float calcfAimZ(float aim) const;

	void processPlayerVehPassSync(std::uint16_t id) const;
	void processAimSync(std::uint16_t id, const vector& targetPos) const;
	void processBulletSync(std::uint16_t id, const vector& targetPos, bool isMiss) const;
	void processGiveDamage(std::uint16_t id) const;
	void processReduceAmmo() const;
	void processWeaponSync(std::uint16_t id, bool isMiss) const;
	void processPackets(std::uint16_t id, bool isMiss = false) const;

	std::uint16_t processPlayerId(bool wBlackList = false) const;

	bool checkSkin(std::uint16_t id) const;

	bool remotePlayerInVehicle(std::uint16_t id) const;
};

#endif // DMG_FUNCS_HPP