#include "ClientStandard.hpp"

void ClientStandard::initialize()
{
	std::cout << "Client Initialized\n";
	client.connect(enetpp::client_connect_params()
		.set_channel_count(game_data->getNetworkManager()->getChannelCount())
		.set_server_host_name_and_port(game_data->getNetworkManager()->getServerIP(), game_data->getNetworkManager()->getServerPort()));
}

void ClientStandard::deinitialize()
{
	std::cout << "Client Deinitialized\n";
	client.disconnect();
}

void ClientStandard::processPackets()

{
	auto on_connected = [this]()
	{
		std::cout << "Connected\n";
		Packet p;
		p.setID(hash("Connected"));
		p.senderID = 1;
		pushPacket(std::move(p));
	};

	auto on_disconnected = [this]()
	{
		std::cout << "Disconnected\n";
		Packet p;
		p.setID(hash("Disconnected"));
		p.senderID = 1;
		pushPacket(std::move(p));
	};

	auto on_data_received = [this](const enet_uint8* data, size_t data_size)
	{
		Packet p(data, data_size);

		if (p.getID() == hash("ClientInitialized"))
		{
			p >> id;
			std::cout << "Our ID is " << id << "\n";
			return;
		}

		p.senderID = 1;
		pushPacket(std::move(p));
	};

	client.consume_events(std::move(on_connected),
		std::move(on_disconnected),
		std::move(on_data_received));
}

void ClientStandard::sendPacket(enet_uint8 channel_id, Packet * p, enet_uint32 flags)
{
	client.send_packet(channel_id, p->buffer.data(), p->buffer.size(), flags);
}

