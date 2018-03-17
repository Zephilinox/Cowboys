#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "Client.hpp"
#include "../GameData.hpp"

class ClientStandard : public Client
{
public:
	ClientStandard(GameData* game_data)
		: Client(game_data)
	{}

	~ClientStandard() = default;

	void initialize() final;
	void deinitialize() final;

	void processPackets() final;

	void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) final;

	inline bool isConnected() const final
	{
		return client.get_connection_status() == enetpp::client::Status::CONNECTED;
	}

private:
	enetpp::client client;
};