#include "Warband.hpp"
#include "../Architecture/Networking/Packet.hpp"
#include "../Architecture/Entity.hpp"
#include "Unit.hpp"
#include <algorithm>

Warband::Warband(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID)
	: game_data(game_data)
{
	units.reserve(5);
	units.push_back(unit1ID);
	units.push_back(unit2ID);
	units.push_back(unit3ID);
	units.push_back(unit4ID);
	units.push_back(unit5ID);
}

void Warband::addToNetworkIDs(uint32_t new_ID)
{
	unit_network_IDs.push_back(new_ID);
}

void Warband::sendJSONPackets()
{
	//send 5 packets, 1 per ID of JSON files to load (from units vector)
	for(int i = 0; i < 5; i++)
	{
		//make a packet
		Packet p;
		p.setID(hash("Entity"));
		//set up manual info as cannot pass StateGame into warband class
		EntityInfo info;
		info.networkID = unit_network_IDs[i];
		info.ownerID = game_data->getNetworkManager()->client->getID();
		info.type = (hash("Unit"));
		//put info, packet type and the unitID into the packet
		p << info;
		p << Unit::PacketType::LOAD_JSON;
		p << units[i];
		//Send the packet
		game_data->getNetworkManager()->client->sendPacket(0, &p);
	}
}

//bool Warband::compareByInitiative(const InitiativeTrack &a, const InitiativeTrack &b)
//{
//	return a.initiative < b.initiative;
//}

void Warband::checkReady(EntityManager & ent_man)
{
	//	once our warband has all 5 units pushed back, it sends 5 packets, 1 per unit
	//	saying which unit to load from json
	if (!already_checked && unit_network_IDs.size() == 5)
	{
		already_checked = true;
		sendJSONPackets();

		for (int i = 0; i < 5; i++)
		{
			Entity* ent = ent_man.getEntity(unit_network_IDs[i]);
			Unit* unit = static_cast<Unit*>(ent);
			unit->loadFromJSON(units[i]);

			InitiativeTrack unitTrack;
			unitTrack.net_ID = unit_network_IDs[i];
			unitTrack.initiative = unit->getInitiative();
			unitTrack.hasActed = false;
			initiativeTracker.push_back(std::move(unitTrack));
		}
		//do sort here
		std::sort(initiativeTracker.begin(), initiativeTracker.end(), isLowerFunctor());
	}
}

uint32_t Warband::getNextUnitInInitiativeList()
{
	for (auto& tracker : initiativeTracker)
	{
		if (!tracker.hasActed)
		{
			return tracker.net_ID;
		}
	}

	return 0;
}


//RICARDO check pls
//this is fine, but it might be better to make these functions on the unit itself.
//I'll take a look at improving some of the base entity stuff so this is easier
void Warband::sendMoveCommand(uint32_t unit_network_ID, int target_grid_x, int target_grid_y )
{
	//make a packet
	Packet p;
	p.setID(hash("Entity"));
	//set up manual info as cannot pass StateGame into warband class
	EntityInfo info;
	info.networkID = unit_network_ID;
	info.ownerID = game_data->getNetworkManager()->client->getID();
	info.type = (hash("UnitMove"));

	//put info, packet type and the unitID into the packet
	p << info;
	p << Unit::PacketType::MOVE;
	p << target_grid_x << target_grid_y;
	//Send the packet
	game_data->getNetworkManager()->client->sendPacket(0, &p);
}

void Warband::sendAttackCommand(uint32_t attacking_unit_network_ID, uint32_t defending_unit_network_ID)
{
	//make packet
	//get damage caused (unit function)
	//send packet containing attacker ID, defender ID and damage dealt

	//make a packet
	Packet p;
	p.setID(hash("Entity"));
	//set up manual info as cannot pass StateGame into warband class
	EntityInfo info;
	info.networkID = attacking_unit_network_ID;
	info.ownerID = game_data->getNetworkManager()->client->getID();
	info.type = (hash("UnitAttack"));

	//put info, packet type and the unitID into the packet
	p << info;
	p << Unit::PacketType::ATTACK;
	p << attacking_unit_network_ID << defending_unit_network_ID;

	//Send the packet
	game_data->getNetworkManager()->client->sendPacket(0, &p);
}

bool Warband::getAllUnitsActed()
{
	return allUnitsActed;
}

void Warband::resetAllActed()
{
	for(auto& tracker : initiativeTracker)
	{
		tracker.hasActed = false;
	}
}

void Warband::endTurn(EntityManager & ent_man, uint32_t netID)
{
	Entity* ent = ent_man.getEntity(netID);
	Unit* unit = static_cast<Unit*>(ent);
	unit->endTurn();
}


unsigned int Warband::getUnitNetworkIDsSize()
{
	return unit_network_IDs.size();
}

uint32_t Warband::getUnitNetworkIDAt(int index)
{
	return unit_network_IDs[index];
}

int Warband::getUnitIDAt(int index)
{
	return units[index];
}

Warband::Warband(GameData* game_data)
	: game_data(game_data)
{

}

