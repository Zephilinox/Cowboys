#include "EntityManager.hpp"

EntityManager::EntityManager(GameData* game_data)
	: game_data(game_data)
{
}

Entity* EntityManager::getEntity(uint32_t networkID)
{
	auto it = std::find_if(entities.begin(), entities.end(), [&](const auto& ent)
	{
		return ent->entity_info.networkID == networkID;
	});

	if (it != entities.end())
	{
		return it->get();
	}

	return nullptr;
}
