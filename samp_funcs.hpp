#ifndef SAMP_FUNCS_HPP
#define SAMP_FUNCS_HPP

#include <string>

#include "BitStream.h"

#include "structs.hpp"
#include "packets.hpp"

using oSend = bool(__thiscall*)(void*, BitStream*, PacketPriority, PacketReliability, char);

enum sampVer : std::uint8_t {
	SAMP_VER_R1,
	SAMP_VER_R3_1,
	SAMP_VER_UNK
};

enum limits : std::uint16_t {
	SAMP_MAX_PLAYERS = 1004
};

class sampFuncs {
	sampFuncs();
	~sampFuncs();
public:
	[[nodiscard]] static sampFuncs& i();

	[[nodiscard]] static std::tuple<std::uint32_t, sampVer> getSampBaseAndVer();
	[[nodiscard]] static bool sampInit();

	[[nodiscard]] std::uint32_t getSendPacketAddr() const;
	void setOrigSendPacket(oSend orig);

	bool sendPacket(BitStream* bs) const;
	bool sendRPC(RPCEnumeration id, BitStream* bs) const;

	void addChatMessage(int color, const char* message) const;
	void registerChatCommand(const char* cmd, cmdProc commandFunc) const;
	void unregisterChatCommand(cmdProc commandFunc);
	void updateScoreboardData() const;
	void disableNameTags(bool state) const;

	[[nodiscard]] const stInputInfo* getInput() const;

	[[nodiscard]] bool gameStateIsConnected() const;
	[[nodiscard]] const char* getServerIP() const;

	[[nodiscard]] int remotePlayerIsListed(std::uint16_t id) const;

	[[nodiscard]] stSAMPPed* getLocalSampPed() const;
	[[nodiscard]] stOnFootData getLocalOnFootData() const;
	[[nodiscard]] stInCarData getLocalInCarData() const;
	[[nodiscard]] stPassengerData getLocalPassengerData() const;
	[[nodiscard]] std::uint8_t getCurrentWeapon() const;

	[[nodiscard]] const std::string& getRemotePlayerName(std::uint16_t id) const;
	[[nodiscard]] std::uint32_t getRemotePlayerNameColor(std::uint16_t id) const;
	[[nodiscard]] int getRemotePlayerScore(std::uint16_t id) const;
	[[nodiscard]] int getRemotePlayerPing(std::uint16_t id) const;

	[[nodiscard]] std::uint32_t getRemotePlayer(std::uint16_t id) const;
	[[nodiscard]] std::uint32_t getRemotePlayerData(std::uint16_t id) const;

	[[nodiscard]] stSAMPPed* getRemotePlayerSampPed(std::uint16_t id) const;
	[[nodiscard]] float getRemotePlayerArmour(std::uint16_t id) const;
	[[nodiscard]] float getRemotePlayerHealth(std::uint16_t id) const;
	[[nodiscard]] int getRemotePlayerAFKState(std::uint16_t id) const;
private:
	std::uint32_t base_;
	sampVer ver_;

	oSend sendPacket_;

	std::uint32_t samp_;
	void* pChat_;
	stInputInfo* pInput_;

	void** pRakClient_;
	const char* serverIP_;
	int* gameState_;

	std::uint32_t* pRemotePlayer_;
	int* isListed_;

	stSAMPPed* pLocalSampPed_;
	stOnFootData* onFootData_;
	stInCarData* inCardata_;
	stPassengerData* passengerData_;
	std::uint8_t* currentWeapon_;
};

#endif