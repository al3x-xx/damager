#ifndef PACKETS_HPP
#define PACKETS_HPP

// blast.hk

enum RPCEnumeration {
	RPC_GiveTakeDamage = 115
};

enum PacketEnumeration {
	ID_VEHICLE_SYNC = 200,
	ID_AIM_SYNC = 203,
	ID_WEAPONS_UPDATE,
	ID_BULLET_SYNC = 206,
	ID_PLAYER_SYNC,
	ID_PASSENGER_SYNC = 211
};

enum PacketPriority {
	HIGH_PRIORITY = 1
};

enum PacketReliability {
	UNRELIABLE_SEQUENCED = 7
};

#endif // PACKETS_HPP