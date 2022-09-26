#ifndef SAMP_FUNCS_HPP
#define SAMP_FUNCS_HPP

#include <memory>
#include <string>

#include "BitStream.h"

#include "structs.hpp"
#include "packets.hpp"

class sampFuncs {
public:
	sampFuncs() noexcept;
	~sampFuncs();

	static inline std::unique_ptr<sampFuncs> i;

	static constexpr auto playerLimit{ 1004 };
private:
	enum sampVer_ : std::uint8_t {
		r1,
		r3_1,
		unk
	};
public:
	[[nodiscard]] static std::tuple<std::uint32_t, sampVer_> getSampBaseAndVer() noexcept;
	[[nodiscard]] static bool sampState() noexcept;

	[[nodiscard]] std::uint32_t getSendPacketAddr() const noexcept;

	using oSend = bool(__thiscall*)(void*, BitStream*, PacketPriority, PacketReliability, char);
	void setOrigSendPacket(oSend orig) noexcept;

	bool sendPacket(BitStream* bs) const;
	bool sendRpc(RPCEnumeration id, BitStream* bs) const;

	void addChatMessage(int color, const char* message) const;
	void registerChatCommand(const char* cmd, cmdProc commandFunc) const;
	void unregisterChatCommand(cmdProc commandFunc) const noexcept;
	void updateScoreboardData() const;
	void disableNameTags(bool state) const;

	[[nodiscard]] const stInputInfo* getInput() const noexcept;

	[[nodiscard]] bool gameStateIsConnected() const noexcept;
	[[nodiscard]] const char* getServerIp() const noexcept;

	[[nodiscard]] int remotePlayerIsListed(std::uint16_t id) const noexcept;

	[[nodiscard]] const stSAMPPed* getLocalSampPed() const noexcept;
	[[nodiscard]] stOnFootData getLocalOnFootData() const noexcept;
	[[nodiscard]] stInCarData getLocalInCarData() const noexcept;
	[[nodiscard]] stPassengerData getLocalPassengerData() const noexcept;
	[[nodiscard]] std::uint8_t getCurrentWeapon() const noexcept;

	[[nodiscard]] const std::string& getRemotePlayerName(std::uint16_t id) const noexcept;
	[[nodiscard]] std::uint32_t getRemotePlayerNameColor(std::uint16_t id) const noexcept;
	[[nodiscard]] int getRemotePlayerScore(std::uint16_t id) const noexcept;
	[[nodiscard]] int getRemotePlayerPing(std::uint16_t id) const noexcept;

	[[nodiscard]] std::uint32_t getRemotePlayer(std::uint16_t id) const noexcept;
	[[nodiscard]] std::uint32_t getRemotePlayerData(std::uint16_t id) const noexcept;

	[[nodiscard]] const stSAMPPed* getRemotePlayerSampPed(std::uint16_t id) const noexcept;
	[[nodiscard]] float getRemotePlayerArmour(std::uint16_t id) const noexcept;
	[[nodiscard]] float getRemotePlayerHealth(std::uint16_t id) const noexcept;
	[[nodiscard]] int getRemotePlayerAfkState(std::uint16_t id) const noexcept;
private:
	std::uint32_t base_;
	sampVer_ ver_;

	oSend sendPacket_;

	std::uint32_t samp_;
	void* pChat_;
	stInputInfo* pInput_;

	const char* serverIp_;
	int* gameState_;
	void** pRakClient_;

	std::uint32_t* pRemotePlayer_;
	int* isListed_;

	stSAMPPed* pLocalSampPed_;
	stOnFootData* onFootData_;
	stInCarData* inCardata_;
	stPassengerData* passengerData_;
	std::uint8_t* currentWeapon_;
};

#endif