#include "samp_funcs.hpp"

#include <ranges>

#include "lemon/mem.hpp"

namespace offsets {
	// r1, r3-1
	
	const std::uint32_t pSamp[]{ 0x21A0F8, 0x26E8DC };
	const std::uint32_t pChat[]{ 0x21A0E4, 0x26E8C8 };
	const std::uint32_t pInput[]{ 0x21A0E8, 0x26E8CC };

	// stSamp {
	const std::uint8_t serverIp[]{ 0x20, 0x30 };
	const std::uint16_t gameState[]{ 0x3BD, 0x3CD };
	const std::uint16_t pRakClient[]{ 0x3C9, 0x2C };
	const std::uint16_t pPools[]{ 0x3CD, 0x3DE };
	// }

	// stSAMPPools {
	const std::uint8_t pPlayerPools[]{ 0x18, 0x8 };
	// }
	
	// stPlayerPool {
	const std::uint16_t pLocalPlayer[]{ 0x22, 0x2F3A };
	const std::uint8_t pRemotePlayer[]{ 0x2E, 0x4 };
	const std::uint16_t isListed[]{ 0xFDE, 0xFB4 };
	// }

	// stLocalPlayer {
	const std::uint8_t pLocalSampPed[]{ 0x0, 0x0 };
	const std::uint8_t onFootData[]{ 0x18, 0x98 };
	const std::uint8_t inCarData[]{ 0xAA, 0x4 };
	const std::uint8_t passengerData[]{ 0x5C, 0xDC };
	const std::uint16_t currentWeapon[]{ 0x165, 0x189 };
	// }

	// stRemotePlayer {
	const std::uint8_t pRemotePlayerData[]{ 0x0, 0x0 };
	const std::uint8_t remotePlayerName[]{ 0xC, 0xC };
	const std::uint8_t score[]{ 0x24, 0x24 };
	const std::uint8_t ping[]{ 0x28, 0x4 };
	// }

	// stRemotePlayerData {
	const std::uint8_t pRemoteSampPed[]{ 0x0, 0x0 };
	const std::uint16_t armour[]{ 0x1B8, 0x1AC };
	const std::uint16_t health[]{ 0x1BC, 0x1B0 };
	const std::uint16_t afkState[]{ 0x1D1, 0x1D1 };
	// }

	// funcs
	const std::uint32_t addChatMessage[]{ 0x64010, 0x679F0 };
	const std::uint32_t registerChatCommand[]{ 0x65AD0, 0x69000 };
	const std::uint32_t remotePlayerNameColor[]{ 0x216378, 0x151578 };
	const std::uint16_t updateScoreboardData[]{ 0x8A10, 0x8BA0 };

	// patches
	const std::uint32_t nameTag[]{ 0x70D40, 0x74C30 };
	const std::uint32_t nameTagBar[]{ 0x6FC30, 0x73B20 };
}

namespace gameState {
	const std::uint8_t connected[]{ 0xE, 0x5 };
}

sampFuncs::sampFuncs() noexcept {
	std::tie(base_, ver_) = getSampBaseAndVer();

	samp_ = *reinterpret_cast<decltype(samp_)*>(base_ + offsets::pSamp[ver_]);
	pChat_ = *reinterpret_cast<decltype(pChat_)*>(base_ + offsets::pChat[ver_]);
	pInput_ = *reinterpret_cast<decltype(pInput_)*>(base_ + offsets::pInput[ver_]);

	serverIp_ = reinterpret_cast<decltype(serverIp_)>(samp_ + offsets::serverIp[ver_]);
	gameState_ = reinterpret_cast<decltype(gameState_)>(samp_ + offsets::gameState[ver_]);
	pRakClient_ = *reinterpret_cast<decltype(pRakClient_)*>(*reinterpret_cast<decltype(pRakClient_)>(samp_ + offsets::pRakClient[ver_]));

	const auto pools{ *reinterpret_cast<std::uint32_t*>(samp_ + offsets::pPools[ver_]) };

	const auto poolsPlayer{ *reinterpret_cast<std::uint32_t*>(pools + offsets::pPlayerPools[ver_]) };

	pRemotePlayer_ = reinterpret_cast<decltype(pRemotePlayer_)>(poolsPlayer + offsets::pRemotePlayer[ver_]);
	isListed_ = reinterpret_cast<decltype(isListed_)>(poolsPlayer + offsets::isListed[ver_]);

	const auto localPlayer{ *reinterpret_cast<std::uint32_t*>(poolsPlayer + offsets::pLocalPlayer[ver_]) };

	pLocalSampPed_ = *reinterpret_cast<decltype(pLocalSampPed_)*>(localPlayer + offsets::pLocalSampPed[ver_]);
	onFootData_ = reinterpret_cast<decltype(onFootData_)>(localPlayer + offsets::onFootData[ver_]);
	inCardata_ = reinterpret_cast<decltype(inCardata_)>(localPlayer + offsets::inCarData[ver_]);
	passengerData_ = reinterpret_cast<decltype(passengerData_)>(localPlayer + offsets::passengerData[ver_]);
	currentWeapon_ = reinterpret_cast<decltype(currentWeapon_)>(localPlayer + offsets::currentWeapon[ver_]);
}

sampFuncs::~sampFuncs() {
	disableNameTags(false);
}

std::tuple<std::uint32_t, sampFuncs::sampVer_> sampFuncs::getSampBaseAndVer() noexcept {
	if (const auto base{ reinterpret_cast<std::uint32_t>(GetModuleHandleA("samp.dll")) }) {
		switch (reinterpret_cast<IMAGE_NT_HEADERS*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew)->OptionalHeader.AddressOfEntryPoint) {
		case 0x31DF13: return { base, sampVer_::r1 };
		case 0xCC4D0: return { base, sampVer_::r3_1 };
		}
	}
	return { 0, sampVer_::unk };
}

bool sampFuncs::sampState() noexcept {
	static const auto [base, ver] { getSampBaseAndVer() };
	return *reinterpret_cast<void**>(base + offsets::pSamp[ver]) && *reinterpret_cast<void**>(base + offsets::pChat[ver]) && *reinterpret_cast<void**>(base + offsets::pInput[ver]);
}

std::uint32_t sampFuncs::getSendPacketAddr() const noexcept {
	return reinterpret_cast<std::uint32_t>(pRakClient_[6]);
}

void sampFuncs::setOrigSendPacket(oSend orig) noexcept {
	sendPacket_ = orig;
}

bool sampFuncs::sendPacket(BitStream* bs) const {
	return sendPacket_(*pRakClient_, bs, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_SEQUENCED, 0);
}

bool sampFuncs::sendRpc(RPCEnumeration id, BitStream* bs) const {
	return reinterpret_cast<bool(__thiscall*)(void*, RPCEnumeration*, BitStream*, PacketPriority, PacketReliability, char, bool)>(pRakClient_[25])(*pRakClient_, &id, bs, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_SEQUENCED, 0, false);
}

void sampFuncs::addChatMessage(int color, const char* message) const {
	if (ver_ == sampVer_::r1) return reinterpret_cast<void(__thiscall*)(void*, int, const char*, char*, int, std::uint32_t)>(base_ + offsets::addChatMessage[ver_])(pChat_, 8, message, nullptr, color, 0);
	reinterpret_cast<void(__thiscall*)(void*, int, const char*)>(base_ + offsets::addChatMessage[ver_])(pChat_, color, message);
}

void sampFuncs::registerChatCommand(const char* cmd, cmdProc commandFunc) const {
	reinterpret_cast<void(__thiscall*)(void*, const char*, cmdProc)>(base_ + offsets::registerChatCommand[ver_])(pInput_, cmd, commandFunc);
}

void sampFuncs::unregisterChatCommand(cmdProc commandFunc) const noexcept {
	for (const std::uint8_t i : std::ranges::views::iota(0u, std::extent_v<decltype(pInput_->pCMDs)>)) {
		if (auto& res{ pInput_->pCMDs[i] }; res == commandFunc) {
			res = nullptr;
			std::memset(pInput_->szCMDNames[i], 0, std::extent_v<decltype(pInput_->szCMDNames), 1>);
		}
	}
}

void sampFuncs::updateScoreboardData() const {
	reinterpret_cast<void(__thiscall*)(std::uint32_t)>(base_ + offsets::updateScoreboardData[ver_])(samp_);
}

void sampFuncs::disableNameTags(bool state) const {
	lemon::mem::safe_copy(base_ + offsets::nameTag[ver_], reinterpret_cast<std::uintptr_t>(state ? "\xC3" : "\x55"), 1);
	lemon::mem::safe_copy(base_ + offsets::nameTagBar[ver_], reinterpret_cast<std::uintptr_t>(state ? "\xC3" : "\x55"), 1);
}

const stInputInfo* sampFuncs::getInput() const noexcept {
	return pInput_;
}

bool sampFuncs::gameStateIsConnected() const noexcept {
	return *gameState_ == gameState::connected[ver_];
}

const char* sampFuncs::getServerIp() const noexcept {
	return serverIp_;
}

int sampFuncs::remotePlayerIsListed(std::uint16_t id) const noexcept {
	return isListed_[id];
}

const stSAMPPed* sampFuncs::getLocalSampPed() const noexcept {
	return pLocalSampPed_;
}

stOnFootData sampFuncs::getLocalOnFootData() const noexcept {
	return *onFootData_;
}

stInCarData sampFuncs::getLocalInCarData() const noexcept {
	return *inCardata_;
}

stPassengerData sampFuncs::getLocalPassengerData() const noexcept {
	return *passengerData_;
}

std::uint8_t sampFuncs::getCurrentWeapon() const noexcept {
	return *currentWeapon_;
}

const std::string& sampFuncs::getRemotePlayerName(std::uint16_t id) const noexcept {
	return *reinterpret_cast<std::string*>(pRemotePlayer_[id] + offsets::remotePlayerName[ver_]);
}

std::uint32_t sampFuncs::getRemotePlayerNameColor(std::uint16_t id) const noexcept {
	return *reinterpret_cast<std::uint32_t*>(base_ + offsets::remotePlayerNameColor[ver_] + id * 4);
}

int sampFuncs::getRemotePlayerScore(std::uint16_t id) const noexcept {
	return *reinterpret_cast<int*>(pRemotePlayer_[id] + offsets::score[ver_]);
}

int sampFuncs::getRemotePlayerPing(std::uint16_t id) const noexcept {
	return *reinterpret_cast<int*>(pRemotePlayer_[id] + offsets::ping[ver_]);
}

std::uint32_t sampFuncs::getRemotePlayer(std::uint16_t id) const noexcept {
	return pRemotePlayer_[id];
}

std::uint32_t sampFuncs::getRemotePlayerData(std::uint16_t id) const noexcept {
	return *reinterpret_cast<std::uint32_t*>(pRemotePlayer_[id] + offsets::pRemotePlayerData[ver_]);
}

const stSAMPPed* sampFuncs::getRemotePlayerSampPed(std::uint16_t id) const noexcept {
	return *reinterpret_cast<stSAMPPed**>(getRemotePlayerData(id) + offsets::pRemoteSampPed[ver_]);
}

float sampFuncs::getRemotePlayerArmour(std::uint16_t id) const noexcept {
	return *reinterpret_cast<float*>(getRemotePlayerData(id) + offsets::armour[ver_]);
}

float sampFuncs::getRemotePlayerHealth(std::uint16_t id) const noexcept {
	return *reinterpret_cast<float*>(getRemotePlayerData(id) + offsets::health[ver_]);
}

int sampFuncs::getRemotePlayerAfkState(std::uint16_t id) const noexcept {
	return *reinterpret_cast<int*>(getRemotePlayerData(id) + offsets::afkState[ver_]);
}