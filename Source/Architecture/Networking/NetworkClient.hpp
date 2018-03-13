#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "Network.hpp"

class NetworkClient : public Network
{
public:
	virtual ~NetworkClient() override final = default;

	virtual void initialize() override final;
	virtual void deinitialize() override final;
	virtual void processEvents() override final;

	virtual bool isConnected() const override final;
	virtual bool isServer() const override final;

	virtual void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) override final;
	virtual void sendPacket(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) override final;
	virtual void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate) override final;

private:
	enetpp::client client;
	bool client_connected_to_server;
};