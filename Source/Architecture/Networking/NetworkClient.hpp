#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "Network.hpp"

class NetworkClient : public Network
{
public:
	NetworkClient(GameData* game_data);
	~NetworkClient() final = default;

	void initialize() final;
	void deinitialize() final;
	void processEvents() final;

	bool isConnected() const final;
	bool isServer() const final;

	void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;
	void sendPacket(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;
	void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate) final;

private:
	enetpp::client client;
	bool client_connected_to_server;
};