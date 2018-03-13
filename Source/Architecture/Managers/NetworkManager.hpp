#pragma once

//STD
#include <functional>
#include <thread>

//LIB
#include <enetpp/client.h>
#include <enetpp/server.h>

//SELF
#include "../Networking/Network.hpp"
#include "../Networking/NetworkServer.hpp"

class GameData;

class NetworkManager
{
public:
	NetworkManager(GameData* game_data);
	~NetworkManager();

	std::unique_ptr<Network> network;

private:
	void update();

	GameData* game_data;

	std::thread network_thread;

};