#ifndef RAK_CLIENT_HPP
#define RAK_CLIENT_HPP

#include "lemon/detour.hpp"

#include "samp_funcs.hpp"

class rakClient {
public:
	rakClient();
	~rakClient();
private:
	lemon::detour<sampFuncs::oSend> hookSend_{ sampFuncs::i->getSendPacketAddr() };

	bool hookedSend(sampFuncs::oSend orig, void* this_, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel);
};

#endif