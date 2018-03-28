#pragma once

#include "..\Architecture\GameData.hpp"
#include "../Architecture/Managers/EntityManager.hpp"

class Warband
{
public:
	Warband(GameData* game_data);
	Warband(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID);
	~Warband() = default;

	void addToNetworkIDs(uint32_t new_ID);
	void sendJSONPackets();
	void checkReady(EntityManager& ent_man);

	unsigned int getUnitNetworkIDsSize();
	void sendMoveCommand(uint32_t unit_network_ID, int grid_x, int grid_y);
	void sendAttackCommand(uint32_t attacking_unit_network_ID, uint32_t defending_unit_network_ID);

	//after selecting unit with mouse?
	//void selectUnit(); // Or Set Controlled unit?
	uint32_t getUnitNetworkIDAt(int index);
	int getUnitIDAt(int index);


private:
	GameData* game_data;
	std::vector<int> units;
	std::vector<uint32_t> unit_network_IDs;
};