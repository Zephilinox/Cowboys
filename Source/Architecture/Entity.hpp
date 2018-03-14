#pragma once

//LIB
#include <Engine/Renderer.h>

//SELF
#include "Messages/Message.hpp"
#include "Networking/Packet.hpp"
#include "GameData.hpp"

struct EntityInfo
{
	uint32_t networkID;
	uint32_t ownerID;
	HashedID type;
};

inline Packet& operator <<(Packet& p, EntityInfo& e)
{
	p << e.networkID
		<< e.ownerID
		<< e.type;
	return p;
}

inline Packet& operator >>(Packet& p, EntityInfo& e)
{
	p >> e.networkID
		>> e.ownerID
		>> e.type;
	return p;
}

class Entity
{
public:
	Entity(GameData* game_data)
		: game_data(game_data)
	{};

	virtual ~Entity() = default;
	virtual void update(float dt) = 0;
	virtual void render(ASGE::Renderer* renderer) const = 0;

	inline bool isOwner()
	{
		return entity_info.ownerID == game_data->getNetworkManager()->network->getID();
	}

	inline void receivePacket(Packet&& p)
	{
		deserialize(p);
	}

	inline void sendPacket()
	{
		Packet p;
		p.setID(hash("Entity"));
		p << entity_info;
		serialize(p);
		game_data->getNetworkManager()->network->sendPacket(0, &p);
	}

	virtual void serialize(Packet& p) = 0;
	virtual void deserialize(Packet& p) = 0;

	EntityInfo entity_info;
	GameData* game_data;
};