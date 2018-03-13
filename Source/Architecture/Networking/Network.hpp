#pragma once

//STD
#include <functional>

//LIB
#include <enet/enet.h>

//SELF
#include "Packet.hpp"

class ClientInfo
{
public:

	uint32_t id;
	uint32_t get_id() const noexcept
	{
		return id;
	}
};

class Network
{
public:
	virtual ~Network() = default;

	virtual void initialize() = 0;
	virtual void deinitialize() = 0;
	virtual void processEvents() = 0;

	virtual bool isConnected() const = 0;
	virtual bool isServer() const = 0;

	virtual void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;
	virtual void sendPacket(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;
	virtual void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate) = 0;

	inline bool isInitialized()
	{
		return initialized;
	}

	inline uint32_t getClientID()
	{
		return clientID;
	}

protected:
	bool initialized;
	uint32_t clientID;
};