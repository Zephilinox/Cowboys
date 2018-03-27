#include "../States/GameState.h"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../../Architecture/Constants.hpp"
#include "../../Architecture/Networking/Client.hpp"
#include "../../Architecture/Networking/Server.hpp"

GameState::GameState(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID)
	: State(game_data)
	, menu(game_data),
	our_warband(game_data, unit1ID, unit2ID, unit3ID, unit4ID, unit5ID),
	their_warband(game_data)
{

	//Warband is container of network IDs
	//Create entity function call
	//tell warband what created entity's Id is


	//TODO create map

	Packet p;
	p.setID(hash("GameJoined"));
	this->game_data->getNetworkManager()->client->sendPacket(0, &p);

	auto serverPacketReceive = [this](Packet p)
	{
		switch(p.getID())
		{
		case hash("Connected"):
		{
			p.reset();
			p.setID(hash("GameStart"));
			this->game_data->getNetworkManager()->server->sendPacketToOneClient(p.senderID, 0, &p);
		}
		case hash("GameJoined"):
		{
			//TODO ensure entities created use JSON to read in, active sends packets to recover state.
			//TODO ensure positions and facings are also updated
			for(const auto& ent : entities)
			{
				p.reset();
				p.setID(hash("CreateEntity"));
				p << ent->entity_info;
				this->game_data->getNetworkManager()->server->sendPacketToOneClient(p.senderID, 0, &p);
			}
		} break;
		case hash("Disconnected"):
		{
			std::experimental::erase_if(entities, [p](const auto& entity)
			{
				return entity->entity_info.ownerID == p.senderID;
			});
		} break;
		case hash("Entity"):
		{
			EntityInfo info;
			p >> info;
			Entity* ent = getEntity(info.networkID);
			if(ent && //exists
				ent->entity_info.ownerID == info.ownerID && //owners match
				ent->entity_info.type == info.type && //types match
				info.ownerID == p.senderID) //client owns it
			{
				this->game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [senderID = p.senderID](const ClientInfo& ci)
				{
					return ci.id != senderID;
				});
			}
		} break;
		case hash("CreateEntity"):
		{
			EntityInfo info;
			p >> info;
			std::cout << "server create ent : " << info.networkID << " " << info.ownerID << "\n";
			switch(info.type)
			{
			case hash("Paddle"):
			{
				createEntity<Paddle>(p.senderID, this->game_data);
			} break;
			case hash("Ball"):
			{
				createEntity<Ball>(p.senderID, this->game_data);
			} break;
			}
		} break;
		}
	};

	auto clientPacketReceive = [this](Packet p)
	{
		switch(p.getID())
		{
		case hash("Entity"):
		{
			EntityInfo info;
			p >> info;
			Entity* ent = getEntity(info.networkID);

			if(ent && !ent->isOwner())
			{
				ent->receivePacket(Packet(p));
			}

		} break;
		
		case hash("CreateEntity"):
		{
			EntityInfo info;
			p >> info;

			std::cout << "client create ent : " << info.networkID << " " << info.ownerID << "\n";
			if(getEntity(info.networkID))
			{
				std::cout << "WAS ASKED TO CREATE EXISTING ENTITY " << info.networkID << ". REFUSING.\n";
				return;
			}
			switch(info.type)
			{
				case hash("Unit"):
				{
					entities.emplace_back(std::make_unique<Unit>(this->game_data));
					if(entities.back()->isOwner())
					{
						our_warband.addToNetworkIDs(info.networkID);
						//	once our warband has all 5 units pushed back, it sends 5 packets, 1 per unit
						//	saying which unit to load from json
						if(our_warband.getUnitNetworkIDsSize() == 5)
						{
							our_warband.sendJSONPackets();

							for(int i = 0; i < 5; i++)
							{
								Entity* ent = getEntity(our_warband.getUnitNetworkIDAt(i));
								Unit* unit = static_cast<Unit*>(ent);
								unit->loadFromJSON(our_warband.getUnitIDAt(i));
							}
						}
					}
					else
					{
						their_warband.addToNetworkIDs(info.networkID);
					}
				}
			}

			entities.back()->entity_info = info;
		} break;
		}
	};

	managed_slot_1 = this->game_data->getNetworkManager()->client->on_packet_received.connect(clientPacketReceive);

	if(game_data->getNetworkManager()->server)
	{
		managed_slot_2 = this->game_data->getNetworkManager()->server->on_packet_received.connect(serverPacketReceive);
	}

	if(game_data->getNetworkManager()->client)
	{
		createEntity<Paddle>(this->game_data);
	}

	if(game_data->getNetworkManager()->server)
	{
		createEntity<Ball>(this->game_data);
		serverPaddle = static_cast<Paddle*>(getEntity(1));
		serverBall = static_cast<Ball*>(getEntity(2));
	}
}

GameState::~GameState()
{
	game_data->getNetworkManager()->stopServer();
	game_data->getNetworkManager()->stopClient();
}

void GameState::update(const ASGE::GameTime& gt)
{
	auto client = game_data->getNetworkManager()->client.get();
	auto server = game_data->getNetworkManager()->server.get();

	if(client && client->isConnecting())
	{
		const float dt = gt.delta_time.count() / 1000.0f;

		for(auto& ent : entities)
		{
			ent->update(dt);
		}

		if(server)
		{
			//Simple AABB collision check
			if(serverPaddle->sprite.xPos < serverBall->sprite.xPos + serverBall->sprite.getCurrentFrameSprite()->width() &&
				serverPaddle->sprite.xPos + serverPaddle->sprite.getCurrentFrameSprite()->width() > serverBall->sprite.xPos &&
				serverPaddle->sprite.yPos < serverBall->sprite.yPos + serverBall->sprite.getCurrentFrameSprite()->height() &&
				serverPaddle->sprite.yPos + serverPaddle->sprite.getCurrentFrameSprite()->height() > serverBall->sprite.yPos)
			{
				serverBall->movingLeft = false;
				serverBall->dirY = game_data->getRandomNumberGenerator()->getRandomFloat(-0.8, 0.8);
			}

			uint32_t clientPaddleID = 0;
			clientPaddle = nullptr;
			for(const auto& ent : entities)
			{
				if(!ent->isOwner() &&
					ent->entity_info.type == hash("Paddle"))
				{
					clientPaddleID = ent->entity_info.networkID;
					clientPaddle = static_cast<Paddle*>(getEntity(clientPaddleID));
					break;
				}
			}

			if(clientPaddle)
			{
				if(clientPaddle->sprite.xPos < serverBall->sprite.xPos + serverBall->sprite.getCurrentFrameSprite()->width() &&
					clientPaddle->sprite.xPos + clientPaddle->sprite.getCurrentFrameSprite()->width() > serverBall->sprite.xPos &&
					clientPaddle->sprite.yPos < serverBall->sprite.yPos + serverBall->sprite.getCurrentFrameSprite()->height() &&
					clientPaddle->sprite.yPos + clientPaddle->sprite.getCurrentFrameSprite()->height() > serverBall->sprite.yPos)
				{
					serverBall->movingLeft = true;
					serverBall->dirY = game_data->getRandomNumberGenerator()->getRandomFloat(-0.8, 0.8);
				}
			}
		}
	}

	if(game_data->getInputManager()->isActionPressed("escape"))
	{
		game_data->getStateManager()->pop();
	}
}

void GameState::render() const
{
	auto renderer = game_data->getRenderer();
	auto client = game_data->getNetworkManager()->client.get();

	if(client && client->isConnecting())
	{
		for(const auto& ent : entities)
		{
			ent->render(renderer);
		}
	}
}

void GameState::onActive()
{
}

void GameState::onInactive()
{
}

Entity* GameState::getEntity(uint32_t networkID)
{
	auto it = std::find_if(entities.begin(), entities.end(), [&](const auto& ent)
	{
		return ent->entity_info.networkID == networkID;
	});

	if(it != entities.end())
	{
		return it->get();
	}

	return nullptr;
}
