#pragma once

//STD
#include <functional>
#include <thread>

//LIB
#include <enetpp/client.h>
#include <enetpp/server.h>

//SELF
#include "../Networking/NetworkServer.hpp"
#include "../Networking/NetworkClient.hpp"
#include "../Signals/Signal.hpp"

class GameData;

class NetworkManager
{
public:
	NetworkManager(GameData* game_data);
	~NetworkManager();

	void initialize(bool server);
	void update();
	void pushPacket(Packet&& p);

	std::unique_ptr<Network> network;
	Signal<Packet> packet_received;

private:
	void runThreadedNetworking();

	GameData* game_data;

	std::thread network_thread;
	bool exit_thread = false;

	std::mutex packets_mutex;
	std::queue<Packet> packets;
};