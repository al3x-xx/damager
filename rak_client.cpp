#include "rak_client.hpp"

#include "settings.hpp"

rakClient::rakClient() {
	hookSend_.install(std::make_tuple(this, &rakClient::hookedSend));
	sampFuncs::i().setOrigSendPacket(hookSend_.orig_func());
}

rakClient::~rakClient() {
	hookSend_.remove();
}

bool rakClient::hookedSend(oSend orig, void* this_, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) {
	bitStream->ResetReadPointer();
	PacketEnumeration packetId;
	bitStream->Read(packetId);

	auto f{ [] <typename T>() consteval {
		constexpr auto b{ std::numeric_limits<std::uint8_t>::digits };
		return b + offsetof(T, fPosition[2]) * b;
	} };

	switch (packetId) {
	case PacketEnumeration::ID_AIM_SYNC: {
		if (set::i().main.packetAim && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.dmgId)) {
			return false;
		}
	} break;
	case PacketEnumeration::ID_PLAYER_SYNC: {
		if (set::i().main.packetVehPlayerPass && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.dmgId)) {
			return false;
		}
		else if (set::i().misc.invi) {
			constexpr auto offset{ f.template operator()<stOnFootData>() };

			bitStream->SetReadOffset(offset);
			float z;
			bitStream->Read(z);

			bitStream->SetWriteOffset(offset);
			bitStream->Write(z - set::i().misc.inviZ);
		}
	} break;
	case PacketEnumeration::ID_PASSENGER_SYNC: {
		if (set::i().main.packetVehPlayerPass && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.dmgId)) {
			return false;
		}
		else if (set::i().misc.invi) {
			constexpr auto offset{ f.template operator()<stPassengerData>() };

			bitStream->SetReadOffset(offset);
			float z;
			bitStream->Read(z);

			bitStream->SetWriteOffset(offset);
			bitStream->Write(z - set::i().misc.inviZ);
		}
	} break;
	case PacketEnumeration::ID_VEHICLE_SYNC: {
		if (set::i().main.packetVehPlayerPass && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.dmgId)) {
			return false;
		}
		else if (set::i().misc.invi) {
			constexpr auto offset{ f.template operator()<stInCarData>() };

			bitStream->SetReadOffset(offset);
			float z;
			bitStream->Read(z);

			bitStream->SetWriteOffset(offset);
			bitStream->Write(z - set::i().misc.inviZ);
		}
	} break;
	case PacketEnumeration::ID_WEAPONS_UPDATE: {
		if (set::i().main.packetWeapon && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.dmgId)) {
			return false;
		}
	} break;
	}
	return orig(this_, bitStream, priority, reliability, orderingChannel);
}