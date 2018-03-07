#pragma once
#include <Server\Networking.h>


class Server
{
public:
	Server() = default;
	~Server() = default;

	void initialise();
	int  run();
	
private:
	ServerComponent network_server;
	void onClientData(server_client& client, const enet_uint8* data, size_t data_size);

public:
	static std::mutex s_cout_mutex;
};