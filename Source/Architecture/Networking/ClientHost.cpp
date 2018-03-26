#include "ClientHost.hpp"

void ClientHost::initialize()
{
	initialized = true;
	std::cout << "Client Initialized\n";
}

void ClientHost::deinitialize()
{
	initialized = false;
	std::cout << "Client Deinitialized\n";
}

void ClientHost::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	p->senderID = id;
	game_data->getNetworkManager()->server->on_packet_received.emit(*p);
}