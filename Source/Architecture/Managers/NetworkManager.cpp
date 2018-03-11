#include "NetworkManager.hpp"

//STD
#include <iostream>

NetworkManager::NetworkManager(GameData* game_data)
	: game_data(game_data)
{
	enetpp::global_state::get().initialize();
}

NetworkManager::~NetworkManager()
{
	reset();
	enetpp::global_state::get().deinitialize();
}

void NetworkManager::initialize(bool hostServer)
{
	initialized = true;
	hosting_server = hostServer;

	if (hosting_server)
	{
		clientID = 1;
		auto client_init = [&](ClientInfo& client, const char* ip)
		{
			std::cout << "Client " << next_uid << " initialized with IP " << ip << "\n";
			client.id = next_uid;
			next_uid++;
			
			//Tell the client what its ID is
			Packet p;
			p.setID(hash("ClientID"));
			p << client.id;
			sendPacket(client.id, 0, &p);
		};

		server.start_listening(enetpp::server_listen_params<ClientInfo>()
			.set_max_client_count(2)
			.set_channel_count(2)
			.set_listen_port(11111)
			.set_initialize_client_function(std::move(client_init)));
	}
	else
	{
		client.connect(enetpp::client_connect_params()
			.set_channel_count(2)
			.set_server_host_name_and_port("localhost", 11111));
	}
}

void NetworkManager::reset()
{
	update();

	if (hosting_server)
	{
		server.stop_listening();
	}
	else
	{
		client.disconnect();
	}

	update();

	initialized = false;
	client_connected_to_server = false;
}

void NetworkManager::update()
{
	if (initialized)
	{
		if (hosting_server)
		{
			updateServer();
		}
		else
		{
			updateClient();
		}
	}
}

void NetworkManager::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	if (!hosting_server)
	{
		if (isConnected())
		{
			client.send_packet(channel_id, p->buffer.data(), p->buffer.size(), flags);
		}
	}
	else
	{
		sendPacket(channel_id, p, flags, [](const ClientInfo& client) {return true; });
	}
}

void NetworkManager::sendPacket(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	assert(hosting_server);
	server.send_packet_to(client_id, channel_id, p->buffer.data(), p->buffer.size(), flags);
}

void NetworkManager::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate)
{
	assert(hosting_server);
	server.send_packet_to_all_if(channel_id, p->buffer.data(), p->buffer.size(), flags, predicate);
}

bool NetworkManager::isServer() const noexcept
{
	return hosting_server;
}

bool NetworkManager::isConnected() const noexcept
{
	return client.is_connecting_or_connected() && client_connected_to_server;
}

bool NetworkManager::isInitialized() const noexcept
{
	return initialized;
}

void NetworkManager::updateServer()
{
	auto on_client_connected = [&](ClientInfo& client)
	{
		std::cout << "client " << client.id << " connected \n";
		client_connected.emit(&client);
	};

	auto on_client_disconnected = [&](uint32_t client_uid)
	{
		client_disconnected.emit(client_uid);
	};

	auto on_client_data_received = [&](ClientInfo& client, const enet_uint8* data, size_t data_size)
	{
		client_sent_packet.emit(&client, {data, data_size});
	};

	server.consume_events(std::move(on_client_connected),
		std::move(on_client_disconnected),
		std::move(on_client_data_received));
}

void NetworkManager::updateClient()
{
	auto on_connected = [&]()
	{
		client_connected_to_server = true;
		connected.emit();
	};

	auto on_disconnected = [&]()
	{
		client_connected_to_server = false;
		disconnected.emit();
	};

	auto on_data_received = [this]( const enet_uint8* data, size_t data_size)
	{
		Packet p(data, data_size);
		if (p.getID() != hash("ClientID"))
		{
			this->server_sent_packet.emit(std::move(p));
		}
		else
		{
			p >> clientID;
			//eww
			p.deserializePosition = sizeof(HashedID);
			std::cout << "ClientID " << this->clientID << " received\n";
			this->server_sent_packet.emit(std::move(p));
		}
	};

	client.consume_events(std::move(on_connected),
		std::move(on_disconnected),
		std::move(on_data_received));
}
