#include "NetworkClient.hpp"

//STD
#include <iostream>

void NetworkClient::initialize()
{
	initialized = true;

	client.connect(enetpp::client_connect_params()
		.set_channel_count(2)
		.set_server_host_name_and_port("localhost", 11111));
}

void NetworkClient::deinitialize()
{
	initialized = false;
	client.disconnect();
}

void NetworkClient::processEvents()
{
	auto on_connected = [&]()
	{
		std::cout << "Client Connected\n";
		client_connected_to_server = true;
	};

	auto on_disconnected = [&]()
	{
		std::cout << "Client Disconnected\n";
		client_connected_to_server = false;
	};

	auto on_data_received = [this](const enet_uint8* data, size_t data_size)
	{
		Packet p(data, data_size);
		if (p.getID() == hash("ClientID"))
		{
			p >> id;
			std::cout << "ClientID " << this->id << " received\n";
		}
		else
		{
			std::string msg;
			p >> msg;
			std::cout << msg;
		}
	};

	client.consume_events(std::move(on_connected),
		std::move(on_disconnected),
		std::move(on_data_received));
}

bool NetworkClient::isConnected() const
{
	return client.is_connecting_or_connected() && client_connected_to_server;
}

bool NetworkClient::isServer() const
{
	return false;
}

void NetworkClient::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	if (isConnected())
	{
		client.send_packet(channel_id, p->buffer.data(), p->buffer.size(), flags);
	}
}

void NetworkClient::sendPacket(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	assert(false);
}

void NetworkClient::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo&client)> predicate)
{
	assert(false);
}
