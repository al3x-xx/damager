#include "samp_funcs.hpp"

#include "lemon/mem.hpp"

namespace offsets {
	// r1, r3-1
	
	constexpr std::uint32_t pSAMP[]{ 0x21A0F8, 0x26E8DC };
	constexpr std::uint32_t pCHAT[]{ 0x21A0E4, 0x26E8C8 };
	constexpr std::uint32_t pINPUT[]{ 0x21A0E8, 0x26E8CC };

	// stSamp {
	constexpr std::uint8_t serverIP[]{ 0x20, 0x30 };
	constexpr std::uint16_t gameState[]{ 0x3BD, 0x3CD };
	constexpr std::uint16_t pRakClient[]{ 0x3C9, 0x2C };
	constexpr std::uint16_t pPools[]{ 0x3CD, 0x3DE };
	// }

	// stSAMPPools {
	constexpr std::uint8_t pPlayerPools[]{ 0x18, 0x8 };
	// }
	
	// stPlayerPool {
	constexpr std::uint16_t pLocalPlayer[]{ 0x22, 0x2F3A };
	constexpr std::uint8_t pRemotePlayer[]{ 0x2E, 0x4 };
	constexpr std::uint16_t isListed[]{ 0xFDE, 0xFB4 };
	// }

	// stLocalPlayer {
	constexpr std::uint8_t pLocalSampPed[]{ 0x0, 0x0 };
	constexpr std::uint8_t onFootData[]{ 0x18, 0x98 };
	constexpr std::uint8_t inCarData[]{ 0xAA, 0x4 };
	constexpr std::uint8_t passengerData[]{ 0x5C, 0xDC };
	constexpr std::uint16_t currentWeapon[]{ 0x165, 0x189 };
	// }

	// stRemotePlayer {
	constexpr std::uint8_t pRemotePlayerData[]{ 0x0, 0x0 };
	constexpr std::uint8_t remotePlayerName[]{ 0xC, 0xC };
	constexpr std::uint8_t score[]{ 0x24, 0x24 };
	constexpr std::uint8_t ping[]{ 0x28, 0x4 };
	// }

	// stRemotePlayerData {
	constexpr std::uint8_t pRemoteSampPed[]{ 0x0, 0x0 };
	constexpr std::uint16_t armour[]{ 0x1B8, 0x1AC };
	constexpr std::uint16_t health[]{ 0x1BC, 0x1B0 };
	constexpr std::uint16_t afkState[]{ 0x1D1, 0x1D1 };
	// }

	// funcs
	constexpr std::uint32_t addChatMessage[]{ 0x64010, 0x679F0 };
	constexpr std::uint32_t registerChatCommand[]{ 0x65AD0, 0x69000 };
	constexpr std::uint32_t remotePlayerNameColor[]{ 0x216378, 0x151578 };
	constexpr std::uint16_t updateScoreboardData[]{ 0x8A10, 0x8BA0 };

	// patches
	constexpr std::uint32_t nameTag[]{ 0x70D40, 0x74C30 };
	constexpr std::uint32_t nameTagBar[]{ 0x6FC30, 0x73B20 };
}

namespace gameState {
	constexpr std::uint8_t connected[]{ 0xE, 0x5 };
}

sampFuncs::sampFuncs() {
	std::tie(base_, ver_) = getSampBaseAndVer();

	pSAMP_ = *reinterpret_cast<void**>(base_ + offsets::pSAMP[ver_]);
	pCHAT_ = *reinterpret_cast<void**>(base_ + offsets::pCHAT[ver_]);
	pINPUT_ = *reinterpret_cast<stInputInfo**>(base_ + offsets::pINPUT[ver_]);

	const auto pSAMP{ reinterpret_cast<std::uint32_t>(pSAMP_) };

	serverIP_ = reinterpret_cast<const char*>(pSAMP + offsets::serverIP[ver_]);
	gameState_ = reinterpret_cast<int*>(pSAMP + offsets::gameState[ver_]);

	pRakClient_ = *reinterpret_cast<void***>(*reinterpret_cast<void**>(pSAMP + offsets::pRakClient[ver_]));

	const auto pPools{ *reinterpret_cast<std::uint32_t*>(pSAMP + offsets::pPools[ver_]) };

	const auto pPoolsPlayer{ *reinterpret_cast<std::uint32_t*>(pPools + offsets::pPlayerPools[ver_]) };

	pRemotePlayer_ = reinterpret_cast<std::uint32_t*>(pPoolsPlayer + offsets::pRemotePlayer[ver_]);
	isListed_ = reinterpret_cast<int*>(pPoolsPlayer + offsets::isListed[ver_]);

	const auto pLocalPlayer{ *reinterpret_cast<std::uint32_t*>(pPoolsPlayer + offsets::pLocalPlayer[ver_]) };

	pLocalSampPed_ = *reinterpret_cast<stSAMPPed**>(pLocalPlayer + offsets::pLocalSampPed[ver_]);
	onFootData_ = reinterpret_cast<stOnFootData*>(pLocalPlayer + offsets::onFootData[ver_]);
	inCardata_ = reinterpret_cast<stInCarData*>(pLocalPlayer + offsets::inCarData[ver_]);
	passengerData_ = reinterpret_cast<stPassengerData*>(pLocalPlayer + offsets::passengerData[ver_]);
	currentWeapon_ = reinterpret_cast<std::uint8_t*>(pLocalPlayer + offsets::currentWeapon[ver_]);
}

sampFuncs::~sampFuncs() {
	disableNameTags(false);
}

sampFuncs& sampFuncs::i() {
	static sampFuncs instance;
	return instance;
}

std::tuple<std::uint32_t, sampVer> sampFuncs::getSampBaseAndVer() {
	const auto base{ reinterpret_cast<std::uint32_t>(GetModuleHandleA("samp.dll")) };
	switch (const auto ep{ reinterpret_cast<IMAGE_NT_HEADERS*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew)->OptionalHeader.AddressOfEntryPoint }; ep) {
	case 0x31DF13: return { base, sampVer::SAMP_VER_R1 };
	case 0xCC4D0: return { base, sampVer::SAMP_VER_R3_1 };
	default: return { 0, sampVer::SAMP_VER_UNK };
	}
}

bool sampFuncs::sampInit() {
	static auto [base, ver] = getSampBaseAndVer();
	return {
		*reinterpret_cast<void**>(base + offsets::pSAMP[ver]) != nullptr &&
		*reinterpret_cast<void**>(base + offsets::pCHAT[ver]) != nullptr &&
		*reinterpret_cast<void**>(base + offsets::pINPUT[ver]) != nullptr
	};
}

std::uint32_t sampFuncs::getSendPacketAddr() const {
	return reinterpret_cast<std::uint32_t>(pRakClient_[6]);
}

void sampFuncs::setOrigSendPacket(oSend orig) {
	sendPacket_ = orig;
}

bool sampFuncs::sendPacket(BitStream* bs) const {
	//return reinterpret_cast<bool(__thiscall*)(void*, BitStream*, PacketPriority, PacketReliability, char)>(pRakClient_[6])(*pRakClient_, bs, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_SEQUENCED, 0);
	return sendPacket_(*pRakClient_, bs, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_SEQUENCED, 0);
}

bool sampFuncs::sendRPC(int uniqueID, BitStream* bs) const {
	return reinterpret_cast<bool(__thiscall*)(void*, int*, BitStream*, PacketPriority, PacketReliability, char, bool)>(pRakClient_[25])(*pRakClient_, &uniqueID, bs, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_SEQUENCED, 0, false);
}

void sampFuncs::addChatMessage(int color, const char* text) const {
	if (ver_ == sampVer::SAMP_VER_R1) {
		reinterpret_cast<void(__thiscall*)(void*, int, const char*, char*, int, std::uint32_t)>(base_ + offsets::addChatMessage[ver_])(pCHAT_, 8, text, nullptr, color, 0);
	}
	else {
		reinterpret_cast<void(__thiscall*)(void*, int, const char*)>(base_ + offsets::addChatMessage[ver_])(pCHAT_, color, text);
	}
}

void sampFuncs::registerChatCommand(const char* szCmd, CMDPROC pFunc) const {
	reinterpret_cast<void(__thiscall*)(void*, const char*, CMDPROC)>(base_ + offsets::registerChatCommand[ver_])(pINPUT_, szCmd, pFunc);
}

void sampFuncs::unregisterChatCommand(CMDPROC pFunc) const {
	for (std::uint8_t i{}; i < 144; i++) {
		if (pINPUT_->pCMDs[i] == pFunc) {
			std::memset(&pINPUT_->pCMDs[i], 0, 4);
			std::memset(pINPUT_->szCMDNames[i], 0, 33);
			std::memset(&pINPUT_->szCMDNames[i], 0, 4);
		}
	}
}

void sampFuncs::updateScoreboardData() const {
	reinterpret_cast<void(__thiscall*)(void*)>(base_ + offsets::updateScoreboardData[ver_])(pSAMP_);
}

void sampFuncs::disableNameTags(bool state) const {
	lemon::mem::safe_copy(base_ + offsets::nameTag[ver_], reinterpret_cast<std::uintptr_t>(state ? "\xC3" : "\x55"), 1);
	lemon::mem::safe_copy(base_ + offsets::nameTagBar[ver_], reinterpret_cast<std::uintptr_t>(state ? "\xC3" : "\x55"), 1);
}

stInputInfo* sampFuncs::getInput() const {
	return pINPUT_;
}

bool sampFuncs::gameStateIsConnected() const {
	return *gameState_ == gameState::connected[ver_];
}

const char* sampFuncs::getServerIP() const {
	return serverIP_;
}

int sampFuncs::remotePlayerIsListed(std::uint16_t id) const {
	return isListed_[id];
}

stSAMPPed* sampFuncs::getLocalSampPed() const {
	return pLocalSampPed_;
}

stOnFootData sampFuncs::getLocalOnFootData() const {
	return *onFootData_;
}

stInCarData sampFuncs::getLocalInCarData() const {
	return *inCardata_;
}

stPassengerData sampFuncs::getLocalPassengerData() const {
	return *passengerData_;
}

std::uint8_t sampFuncs::getCurrentWeapon() const {
	return *currentWeapon_;
}

std::string sampFuncs::getRemotePlayerName(std::uint16_t id) const {
	return *reinterpret_cast<std::string*>(pRemotePlayer_[id] + offsets::remotePlayerName[ver_]);
}

std::uint32_t sampFuncs::getRemotePlayerNameColor(std::uint16_t id) const {
	return *reinterpret_cast<std::uint32_t*>(base_ + offsets::remotePlayerNameColor[ver_] + id * 4);
}

int sampFuncs::getRemotePlayerScore(std::uint16_t id) const {
	return *reinterpret_cast<int*>(pRemotePlayer_[id] + offsets::score[ver_]);
}

int sampFuncs::getRemotePlayerPing(std::uint16_t id) const {
	return *reinterpret_cast<int*>(pRemotePlayer_[id] + offsets::ping[ver_]);
}

std::uint32_t sampFuncs::getRemotePlayer(std::uint16_t id) const {
	return pRemotePlayer_[id];
}

std::uint32_t sampFuncs::getRemotePlayerData(std::uint16_t id) const {
	return *reinterpret_cast<std::uint32_t*>(pRemotePlayer_[id] + offsets::pRemotePlayerData[ver_]);
}

stSAMPPed* sampFuncs::getRemotePlayerSampPed(std::uint16_t id) const {
	return *reinterpret_cast<stSAMPPed**>(getRemotePlayerData(id) + offsets::pRemoteSampPed[ver_]);
}

float sampFuncs::getRemotePlayerArmour(std::uint16_t id) const {
	return *reinterpret_cast<float*>(getRemotePlayerData(id) + offsets::armour[ver_]);
}

float sampFuncs::getRemotePlayerHealth(std::uint16_t id) const {
	return *reinterpret_cast<float*>(getRemotePlayerData(id) + offsets::health[ver_]);
}

int sampFuncs::getRemotePlayerAFKState(std::uint16_t id) const {
	return *reinterpret_cast<int*>(getRemotePlayerData(id) + offsets::afkState[ver_]);
}