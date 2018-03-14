#pragma once

//LIB
#include <Engine/Renderer.h>

//SELF
#include "Messages/Message.hpp"
#include "Networking/Packet.hpp"

struct EntityInfo
{
	uint32_t networkID;
	uint32_t ownerID;
	HashedID type;
};

Packet& operator <<(Packet& p, EntityInfo* e)
{
	p << e->networkID
		<< e->ownerID
		<< e->type;
	return p;
}

Packet& operator >>(Packet& p, EntityInfo* e)
{
	p >> e->networkID
		>> e->ownerID
		>> e->type;
	return p;
}

class GameData;

class Entity
{
public:
	Entity(GameData* game_data)
		: game_data(game_data)
	{};

	virtual ~Entity() = default;
	virtual void update(float dt) = 0;
	virtual void render(ASGE::Renderer* renderer) const = 0;
	virtual void receivedPacket(Packet&& p) = 0;

	inline bool isOwner()
	{
		return entity_info.ownerID == game_data->getNetworkManager()->network->getID();
	}

	EntityInfo entity_info;
	GameData* game_data;
};