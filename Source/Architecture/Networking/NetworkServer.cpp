#include "NetworkServer.hpp"

//STD
#include <iostream>

void NetworkServer::initialize()
{
	initialized = true;
	
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

void NetworkServer::deinitialize()
{
	initialized = false;
	server.stop_listening();
}

void NetworkServer::processEvents()
{
	auto on_client_connected = [&](ClientInfo& client)
	{
		std::cout << "client " << client.id << " connected \n";
		//client_connected.emit(&client);
	};

	auto on_client_disconnected = [&](uint32_t client_uid)
	{
		//client_disconnected.emit(client_uid);
	};

	auto on_client_data_received = [&](ClientInfo& client, const enet_uint8* data, size_t data_size)
	{
		//client_sent_packet.emit(&client, { data, data_size });
	};

	server.consume_events(std::move(on_client_connected),
		std::move(on_client_disconnected),
		std::move(on_client_data_received));
}

bool NetworkServer::isConnected() const
{
	return false;
}

bool NetworkServer::isServer() const
{
	return true;
}

void NetworkServer::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	//Send to all clients
	sendPacket(channel_id, p, flags, [](const ClientInfo& client) {return true;});
}

void NetworkServer::sendPacket(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags)
{
	//Send to one client
	server.send_packet_to(client_id, channel_id, p->buffer.data(), p->buffer.size(), flags);
}

void NetworkServer::sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo&client)> predicate)
{
	//Send to specific clients
	server.send_packet_to_all_if(channel_id, p->buffer.data(), p->buffer.size(), flags, predicate);
}
