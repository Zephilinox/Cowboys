#include "ClientHost.hpp"

void ClientHost::initialize()
{
	std::cout << "Client Initialized\n";
}

void ClientHost::deinitialize()
{
	std::cout << "Client Deinitialized\n";
}

void ClientHost::sendPacket(enet_uint8 channel_id, Packet * p, enet_uint32 flags)
{
	game_data->getNetworkManager()->server->on_packet_received.emit(*p);
}