#pragma once

#include "Character.h"
#include "Unit.h"
#include "Hero.h"
#include "..\Architecture\GameData.hpp"

class Warband
{
public:
	Warband(GameData* game_data);
	Warband(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID);
	~Warband() = default;

	void addToNetworkIDs(uint32_t new_ID);
	void sendJSONPackets();
	
	unsigned int getUnitNetworkIDsSize();

	//after selecting unit with mouse?
	void selectUnit(); // Or Set Controlled unit?
	uint32_t getUnitNetworkIDAt(int index);
	int getUnitIDAt(int index);


private:
	GameData* game_data;
	std::vector<int> units;
	std::vector<uint32_t> unit_network_IDs;
};