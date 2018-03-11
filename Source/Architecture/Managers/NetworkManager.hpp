#pragma once

//STD
#include <functional>

//LIB
#include <enetpp/client.h>
#include <enetpp/server.h>

//SELF
#include "../Signals/Signal.hpp"
#include "../Networking/Packet.hpp"

class GameData;

struct ClientInfo
{
	uint32_t id;
	uint32_t get_id() const noexcept
	{
		return id;
	}
};

class NetworkManager
{
public:
	NetworkManager(GameData* game_data);
	~NetworkManager();

	void initialize(bool hostServer);
	void reset();

	void update();
	void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
	void sendPacket(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE);
	void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate);
	
	bool isServer() const noexcept;
	bool isInitialized() const noexcept;

	//Server
	Signal<ClientInfo*> client_connected;
	Signal<uint32_t> client_disconnected;
	Signal<ClientInfo*, Packet> client_sent_packet;
	enetpp::server<ClientInfo> server;
	uint32_t next_uid = 2; //0 is invalid, 1 is server

	//Client
	Signal<> connected;
	Signal<> disconnected;
	Signal<Packet> server_sent_packet;

	bool isConnected() const noexcept;
	uint32_t clientID = 0;
	enetpp::client client;

private:
	void updateServer();
	void updateClient();

	GameData* game_data;
	bool initialized = false;
	bool hosting_server = true;
	bool client_connected_to_server = false;
};