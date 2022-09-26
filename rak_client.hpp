#ifndef RAK_CLIENT_HPP
#define RAK_CLIENT_HPP

#include "lemon/detour.hpp"

#include "dmg_funcs.hpp"

class rakClient {
public:
	rakClient();
	~rakClient();
private:
	lemon::detour<oSend> hookSend_{ sampFuncs::i().getSendPacketAddr() };

	bool hookedSend(oSend orig, void* this_, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel);
};

#endif