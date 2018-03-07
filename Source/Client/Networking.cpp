#include "stdafx.h"
#include "Networking.h"

bool ClientComponent::initialize()
{
	client.connect(enetpp::client_connect_params()
		.set_channel_count(channel_count)
		.set_server_host_name_and_port("localhost", 8888));


	on_connected = [&]()
	{
		trace("on_connected");
	};

	on_disconnected = [&]()
	{
		trace("on_disconnected");
	};

	on_data_received = [&](const enet_uint8* data, size_t data_size)
	{
		trace("received packet from server : '" +
			std::string(reinterpret_cast<const char*>(data), data_size) + "'");
	};

	return true;
}

bool ClientComponent::deinitialize()
{
	client.disconnect();
	enetpp::global_state::get().deinitialize();
	return true;
}

void ClientComponent::consumeEvents()
{
	while (client.is_connecting_or_connected())
	{
		client.consume_events(
			on_connected,
			on_disconnected,
			on_data_received);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

bool ClientComponent::isConnected() const
{
	return 
		client.get_connection_status() == enetpp::client::CONNECTED;
}

bool ClientComponent::isConnecting() const
{
	return
		client.get_connection_status() == enetpp::client::CONNECTING;
}



