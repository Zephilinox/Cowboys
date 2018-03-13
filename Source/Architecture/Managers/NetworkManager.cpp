#include "NetworkManager.hpp"

//STD
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

NetworkManager::NetworkManager(GameData* game_data)
	: game_data(game_data)
{
	enetpp::global_state::get().initialize();
	network_thread = std::thread(&NetworkManager::update, this);
	network_thread.detach();
}

NetworkManager::~NetworkManager()
{
	enetpp::global_state::get().deinitialize();
	if (network)
	{
		network->deinitialize();
	}
}

void NetworkManager::initialize(bool server)
{
	if (server)
	{
		network = std::move(std::make_unique<NetworkServer>());
	}
	else
	{
		network = std::move(std::make_unique<NetworkClient>());
	}

	network->initialize();
}

void NetworkManager::update()
{
	while (true)
	{
		std::cout << "Network Tick\n";
		if (network)
		{
			network->processEvents();
		}
		std::this_thread::sleep_for(250ms);
	}
}
