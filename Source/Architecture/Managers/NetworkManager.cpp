#include "NetworkManager.hpp"

//STD
#include <iostream>
#include <chrono>

//SELF
#include "../Networking/ServerHost.hpp"
#include "../Networking/ClientHost.hpp"
#include "../Networking/ClientStandard.hpp"

using namespace std::chrono_literals;

NetworkManager::NetworkManager(GameData* game_data)
	: game_data(game_data)
{
	std::cout << "Initializing global state\n";
	enetpp::global_state::get().initialize();

	network_thread = std::thread(&NetworkManager::runThreadedNetwork, this);
	network_thread.detach();
}

NetworkManager::~NetworkManager()
{
	std::cout << "Deinitializing global state\n";
	exit_thread = true;

	stopHost();

	enetpp::global_state::get().deinitialize();
}

void NetworkManager::startHost()
{
	std::cout << "starting server/client\n";
	server = std::move(std::make_unique<ServerHost>(game_data));
	client = std::move(std::make_unique<ClientHost>(game_data));

	server->initialize();
	client->initialize();
}

void NetworkManager::startClient()
{
	assert(!server);

	std::cout << "starting client\n";
	client = std::move(std::make_unique<ClientStandard>(game_data));
	client->initialize();
}

void NetworkManager::stopHost()
{
	std::cout << "stopHost()\n";

	if (server)
	{
		std::cout << "stopping server\n";
		server->deinitialize();
		server.reset(nullptr);
	}

	stopClient();
}

void NetworkManager::stopClient()
{
	std::cout << "stopClient()\n";
	if (client)
	{
		std::cout << "stopping client\n";
		client->deinitialize();
		client.reset(nullptr);
	}
}

void NetworkManager::update()
{
	if (server)
	{
		server->update();
	}
	
	if (client)
	{
		client->update();
	}

	if (networkSendTimer.getElapsedTime() > 1.0f / float(networkSendRate))
	{
		networkSendTimer.restart();
		on_network_tick.emit();
	}
}

void NetworkManager::runThreadedNetwork()
{
	while (!exit_thread)
	{
		if (server)
		{
			server->processPackets();
		}

		if (client)
		{
			client->processPackets();
		}

		std::this_thread::sleep_for(1s / float(networkTickRate));
	}

	std::cout << "Network thread stopped\n";
}
