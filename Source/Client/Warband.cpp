#include "Warband.h"
#include "Character.h"

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
		//set up manual info as cannot pass gamestate into warband class
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

