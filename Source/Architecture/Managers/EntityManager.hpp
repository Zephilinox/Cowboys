#pragma once

//STD
#include <vector>

//SELF
#include "../Entity.hpp"
#include "../GameData.hpp"

class EntityManager
{
public:
	EntityManager(GameData* game_data);

	template <class T, class... Args>
	void createEntityRequest(Args... args)
	{
		assert(game_data->getNetworkManager()->client);
		std::cout << "createEntityRequest\n";
		//Ask server to create our entity
		T ent(std::forward<Args>(args)...);
		ent.entity_info.ownerID = game_data->getNetworkManager()->client->getID();

		Packet p;
		p.setID(hash("CreateEntity"));
		p.senderID = game_data->getNetworkManager()->client->getID();
		p << ent.entity_info;

		game_data->getNetworkManager()->client->sendPacket(0, &p);
	}

	template <class T, class... Args>
	void createEntityForClient(uint32_t ownerID, Args... args)
	{
		assert(game_data->getNetworkManager()->server);
		std::cout << "createEntityForClient " << ownerID << "\n";
		T ent(std::forward<Args>(args)...);
		EntityInfo info = ent.entity_info;
		info.networkID = next_network_id;
		next_network_id++;
		info.ownerID = ownerID;

		//Tell clients to create this entity
		Packet p;
		p.setID(hash("CreateEntity"));
		p.senderID = 1;
		p << info;

		game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p);
	}

	Entity* getEntity(uint32_t networkID);

	//refactor stuff so that this can stay private
	std::vector<std::unique_ptr<Entity>> entities;

private:
	GameData* game_data;
	uint32_t next_network_id = 1;
};
