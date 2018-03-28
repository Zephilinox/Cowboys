#pragma once
#pragma once

//STD
#include <chrono>
#include <vector>
#include <algorithm>

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/Timer.hpp"
#include "../../Architecture/Messages/Message.hpp"
#include "../../Architecture/Managers/NetworkManager.hpp"
#include "../../Architecture/AnimatedSprite.hpp"
#include "../../Architecture/UI/Menu.hpp"
#include "PingPong/Paddle.hpp"
#include "PingPong/Ball.hpp"

#include "../Unit.h"
#include "../Warband.h"

class GameData;

class GameState : public State
{
public:
	GameState(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID);
	~GameState();

	void update(const ASGE::GameTime&) override final;
	void render() const override final;
	void onActive() override final;
	void onInactive() override final;

	template <class T, class... Args>
	void createEntity(Args... args)
	{
		assert(game_data->getNetworkManager()->client);

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
	void createEntity(uint32_t ownerID, Args... args)
	{
		assert(game_data->getNetworkManager()->server);

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

private:
	Entity* getEntity(uint32_t networkID);

	std::vector<std::unique_ptr<Entity>> entities;
	Menu menu;
	Paddle* serverPaddle;
	Ball* serverBall;
	Paddle* clientPaddle;

	Warband our_warband;
	Warband their_warband;

	uint32_t next_network_id = 1;

	ManagedConnection managed_slot_1;
	ManagedConnection managed_slot_2;
};
