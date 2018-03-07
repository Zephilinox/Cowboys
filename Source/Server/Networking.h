#pragma once
#include <enetpp/server.h>
#include <Common/Networking.h>

static unsigned int next_uid = 0;
struct server_client
{
	//MUST return globally unique value here
	unsigned int _uid;
	unsigned int get_id() const { return _uid; } 
};

class ServerComponent :
	public NetworkComponent
{
public:
	ServerComponent() = default;
	~ServerComponent() = default;

	// Inherited via NetworkComponent
	virtual bool initialize() override;
	virtual bool deinitialize() override;
	virtual void consumeEvents() override;

private:
	// three consume functions to process networking
	std::function<void(server_client& client)> on_connected;
	std::function<void(unsigned int client_uid)> on_disconnected;
	std::function<void(server_client& client, const enet_uint8* data, size_t data_size)> on_data_received;

	// the server thread listenting for events
	enetpp::server<server_client> server;

	// the number of clients supported by this server
	static const int MAX_CLIENT_COUNT = 10;

public:
	// setters for the three consumer functions
	auto onData(decltype(on_data_received) fnc)         { on_data_received = fnc; }
	auto onConnected(decltype (on_connected) fnc)       { on_connected = fnc;     }
	auto onDisconnected(decltype (on_disconnected) fnc) { on_disconnected = fnc;  }
};
