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
	std::uint8_t packetId;
	bitStream->ResetReadPointer();
	bitStream->Read(packetId);

	switch (packetId) {
	case PacketEnumeration::ID_AIM_SYNC: {
		if (set::i().main.packetAim && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.gDMGId)) {
			return false;
		}
	} break;
	case PacketEnumeration::ID_PLAYER_SYNC: {		
		if (const auto res{ set::i().main.packetVehPlayerPass && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.gDMGId) }; res || set::i().misc.invi) {
			stOnFootData data{};
			bitStream->Read(reinterpret_cast<char*>(&data), sizeof(data));

			if (res) {
				return false;
			}
			else if (set::i().misc.invi) {
				data.fPosition[2] -= set::i().misc.inviZ;

				bitStream->ResetWritePointer();
				bitStream->Write(packetId);
				bitStream->Write(reinterpret_cast<char*>(&data), sizeof(data));
			}
		}
	} break;
	case PacketEnumeration::ID_PASSENGER_SYNC: {
		if (const auto res{ set::i().main.packetVehPlayerPass && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.gDMGId) }; res || set::i().misc.invi) {
			stPassengerData data{};
			bitStream->Read(reinterpret_cast<char*>(&data), sizeof(data));

			if (res) {
				return false;
			}
			else if (set::i().misc.invi) {
				data.fPosition[2] -= set::i().misc.inviZ;

				bitStream->ResetWritePointer();
				bitStream->Write(packetId);
				bitStream->Write(reinterpret_cast<char*>(&data), sizeof(data));
			}
		}
	} break;
	case PacketEnumeration::ID_VEHICLE_SYNC: {
		if (const auto res{ set::i().main.packetVehPlayerPass && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.gDMGId) }; res || set::i().misc.invi) {
			stInCarData data{};
			bitStream->Read(reinterpret_cast<char*>(&data), sizeof(data));

			if (res) {
				return false;
			}
			else if (set::i().misc.invi) {
				data.fPosition[2] -= set::i().misc.inviZ;

				bitStream->ResetWritePointer();
				bitStream->Write(packetId);
				bitStream->Write(reinterpret_cast<char*>(&data), sizeof(data));
			}
		}
	} break;
	case PacketEnumeration::ID_WEAPONS_UPDATE: {
		if (set::i().main.packetWeapon && dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.gDMGId)) {
			return false;
		}
	} break;
	}
	return orig(this_, bitStream, priority, reliability, orderingChannel);
}