#pragma once

#include "..\Architecture\GameData.hpp"
#include "../Architecture/Managers/EntityManager.hpp"

struct InitiativeTrack
{
	uint32_t net_ID;
	float initiative;
	bool hasActed;
	bool isAlive;
};

struct isLowerFunctor
{
	bool operator()(const InitiativeTrack& x, const InitiativeTrack& y) const
	{
		return y.initiative < x.initiative;
	}
};

class Warband
{
public:
	Warband(GameData* game_data);
	Warband(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID);
	~Warband() = default;

	void addToNetworkIDs(uint32_t new_ID);
	void sendJSONPackets();
	void checkReady(EntityManager & ent_man);
	void initInitiativeTracker(EntityManager & ent_man);


	//bool compareByInitiative(const InitiativeTrack & a, const InitiativeTrack & b);

	uint32_t getNextUnitInInitiativeList();
	std::vector<uint32_t> getUnitNetworkIDs();
	unsigned int getUnitNetworkIDsSize();
	void unitActed(uint32_t netID, bool new_val);
	bool getIsAlive(uint32_t netID);
	void setIsAlive(uint32_t netID, bool new_val);

	bool getAllUnitsActed();
	void checkAllActed();
	void resetAllActed();
	void endTurn(EntityManager & ent_man, uint32_t netID);

	//after selecting unit with mouse?
	//void selectUnit(); // Or Set Controlled unit?
	uint32_t getUnitNetworkIDAt(int index);
	int getUnitIDAt(int index);

private:

	GameData* game_data;
	std::vector<int> units;
	std::vector<uint32_t> unit_network_IDs;
	std::vector<InitiativeTrack> initiativeTracker;

	bool allUnitsActed = false;
	bool already_checked = false;
};