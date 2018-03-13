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

class GameData;

class NetworkManager
{
public:
	NetworkManager(GameData* game_data);
	~NetworkManager();

	void initialize(bool server);

	std::unique_ptr<Network> network;

private:
	void runThreadedNetworking();

	GameData* game_data;
	std::thread network_thread;
	bool exit_thread = false;
};