#include "NetworkManager.hpp"

//STD
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

NetworkManager::NetworkManager(GameData* game_data)
	: game_data(game_data)
{
	enetpp::global_state::get().initialize();
	network_thread = std::thread(&NetworkManager::runThreadedNetworking, this);
	network_thread.detach();
}

NetworkManager::~NetworkManager()
{
	exit_thread = true;
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
		network = std::move(std::make_unique<NetworkServer>(game_data));
	}
	else
	{
		network = std::move(std::make_unique<NetworkClient>(game_data));
	}

	network->initialize();
}

void NetworkManager::update()
{
	std::lock_guard<std::mutex> guard(packets_mutex);
	while (!packets.empty())
	{
		auto packet = packets.front();
		packet_received.emit(packet);
		packets.pop();
	}
}

void NetworkManager::pushPacket(Packet&& p)
{
	std::lock_guard<std::mutex> guard(packets_mutex);
	packets.push(p);
}

void NetworkManager::runThreadedNetworking()
{
	while (!exit_thread)
	{
		if (network)
		{
			network->processEvents();
		}
		std::this_thread::sleep_for(250ms);
	}
}
