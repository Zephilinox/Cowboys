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

class GameData;

class StatePingPong : public State
{
public:
	StatePingPong(GameData* game_data);
	~StatePingPong();

	void update(const ASGE::GameTime&) override final;
	void render() const override final;
	void onActive() override final;
	void onInactive() override final;

	template <class T, class... Args>
	void createEntity(Args... args)
	{
		assert(game_data->getNetworkManager()->network);

		if (game_data->getNetworkManager()->network->isServer())
		{
			entities.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
			entities.back()->entity_info.networkID = next_network_id;
			next_network_id++;
			entities.back()->entity_info.ownerID = game_data->getNetworkManager()->network->getID();

			//Send packets to all clients (except us, of course)
			Packet p;
			p.setID(hash("CreateEntity"));
			p << entities.back()->entity_info;
			game_data->getNetworkManager()->network->sendPacket(0, &p);
		}
		else
		{
			//Send packet to server with what we want to create
			T ent(game_data);
			Packet p;
			p.setID(hash("CreateEntity"));
			p << ent.entity_info;
			game_data->getNetworkManager()->network->sendPacket(0, &p);
		}
	}

	template <class T, class... Args>
	void createEntity(uint32_t ownerID, Args... args)
	{
		assert(game_data->getNetworkManager()->network->isInitialized() && game_data->getNetworkManager()->network->isServer());

		entities.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
		entities.back()->entity_info.networkID = next_network_id;
		next_network_id++;
		entities.back()->entity_info.ownerID = ownerID;

		//Send packet to all clients
		Packet p;
		p.setID(hash("CreateEntity"));
		p << entities.back()->entity_info;
		game_data->getNetworkManager()->network->sendPacket(0, &p);
	}

private:
	Entity* getEntity(uint32_t networkID);

	std::vector<std::unique_ptr<Entity>> entities;
	Menu menu;
	Paddle* serverPaddle;
	Ball* serverBall;
	Paddle* clientPaddle;

	uint32_t next_network_id = 1;

	ManagedConnection managed_slot_1;
	ManagedConnection managed_slot_2;
};
