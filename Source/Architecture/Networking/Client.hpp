#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "../Signals/Signal.hpp"
#include "Packet.hpp"
#include "../Timer.hpp"
#include "../GameData.hpp"

class Client
{
public:
	Client(GameData* game_data)
		: game_data(game_data)
	{}

	virtual ~Client() = default;

	virtual void initialize() = 0;
	virtual void deinitialize() = 0;
	virtual void processPackets() = 0;

	virtual void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;

	virtual bool isConnected() const = 0;
	
	inline void update()
	{
		std::lock_guard<std::mutex> guard(mutex);

		packetsReceived += packets.size();

		if (packetsTimer.getElapsedTime() > 5)
		{
			packetsTimer.restart();
			std::cout << "client received " << packetsReceived / 5 << " packets per second\n";
			packetsReceived = 0;
		}

		while (!packets.empty())
		{
			on_packet_received.emit(packets.front());
			packets.pop();
		}
	}

	inline void pushPacket(Packet&& p)
	{
		std::lock_guard<std::mutex> lock(mutex);
		packets.push(std::move(p));
	}
	
	inline uint32_t getID()
	{
		return id;
	}

	Signal<Packet> on_packet_received;

protected:
	GameData* game_data;

	uint32_t id;

private:
	std::mutex mutex;
	std::queue<Packet> packets;

	Timer packetsTimer;
	uint32_t packetsReceived;
};