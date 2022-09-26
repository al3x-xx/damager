#include "rak_client.hpp"

#include "settings.hpp"
#include "dmg_funcs.hpp"

rakClient::rakClient() {
	hookSend_.install(std::make_tuple(this, &rakClient::hookedSend));
	sampFuncs::i->setOrigSendPacket(hookSend_.orig_func());
}

rakClient::~rakClient() {
	hookSend_.remove();
}

bool rakClient::hookedSend(sampFuncs::oSend orig, void* this_, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) {
	switch (auto processInvis{ [bitStream] <typename T> {
		constexpr auto offset{ []() consteval {
			constexpr auto b{ std::numeric_limits<std::uint8_t>::digits };
			return b + offsetof(T, fPosition[2]) * b;
		}() };

		bitStream->SetReadOffset(offset);
		if (float z; bitStream->Read(z)) {
			bitStream->SetWriteOffset(offset);
			bitStream->Write(z - settings::i->misc.invisz);
		}
	} }; [bitStream] {
		bitStream->ResetReadPointer();
		PacketEnumeration packetId;
		bitStream->Read(packetId);
		return packetId;
	}()) {
	case PacketEnumeration::ID_AIM_SYNC:
		if (settings::i->main.packetAim && dmgFuncs::i->checkActivate() && dmgFuncs::i->remotePlayerIsExist(settings::i->main.dmgId)) return false;
		break;
	case PacketEnumeration::ID_PLAYER_SYNC:
		if (settings::i->main.packetVehPlayerPass && dmgFuncs::i->checkActivate() && dmgFuncs::i->remotePlayerIsExist(settings::i->main.dmgId)) return false;
		if (settings::i->misc.invis) processInvis.template operator()<stOnFootData>();
		break;
	case PacketEnumeration::ID_PASSENGER_SYNC:
		if (settings::i->main.packetVehPlayerPass && dmgFuncs::i->checkActivate() && dmgFuncs::i->remotePlayerIsExist(settings::i->main.dmgId)) return false;
		if (settings::i->misc.invis) processInvis.template operator()<stPassengerData>();
		break;
	case PacketEnumeration::ID_VEHICLE_SYNC:
		if (settings::i->main.packetVehPlayerPass && dmgFuncs::i->checkActivate() && dmgFuncs::i->remotePlayerIsExist(settings::i->main.dmgId)) return false;
		if (settings::i->misc.invis) processInvis.template operator()<stInCarData>();
		break;
	case PacketEnumeration::ID_WEAPONS_UPDATE:
		if (settings::i->main.packetWeapon && dmgFuncs::i->checkActivate() && dmgFuncs::i->remotePlayerIsExist(settings::i->main.dmgId)) return false;
	}
	return orig(this_, bitStream, priority, reliability, orderingChannel);
}