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
	//TODO create map

	//This is lobby-related, leave it for now until I have a closer look at it
	Packet p;
	p.setID(hash("GameJoined"));
	this->game_data->getNetworkManager()->client->sendPacket(0, &p);

	auto serverPacketReceive = [this](Packet p)
	{
		switch(p.getID())
		{
			case hash("Connected"):
			{
				//If a client connects to us when the game has started, tell them to start their game too.
				p.reset();
				p.setID(hash("GameStart"));
				this->game_data->getNetworkManager()->server->sendPacketToOneClient(p.senderID, 0, &p);
			} break;

			case hash("GameJoined"):
			{
				//TODO ensure entities created use JSON to read in, active sends packets to recover state.
				//TODO ensure positions and facings are also updated
				//once a client has started their game (pushed GameState), send them all existing entities.
				for (const auto& ent : entities)
				{
					p.reset();
					p.setID(hash("CreateEntity"));
					p << ent->entity_info;
					this->game_data->getNetworkManager()->server->sendPacketToOneClient(p.senderID, 0, &p);
				}
			} break;

			case hash("Disconnected"):
			{
				//When a client disconnects, keep their entities rather than delete them.
				//So if they rejoin they're all still there? maybe? not sure how to go about this really.
				/*std::experimental::erase_if(entities, [p](const auto& entity)
				{
					return entity->entity_info.ownerID == p.senderID;
				});*/
			} break;

			case hash("Entity"):
			{
				EntityInfo info;
				p >> info;
				Entity* ent = getEntity(info.networkID);

				//Make sure the entity packet is all valid, then send it to the other client.
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
				//Call createEntity based on the type
				EntityInfo info;
				p >> info;
				std::cout << "server create ent : " << info.networkID << " " << info.ownerID << "\n";

				switch(info.type)
				{
					case hash("Unit"):
					{
						createEntity<Unit>(p.senderID, this->game_data);
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
				//Find the entity and give them the packet to deserialize
				EntityInfo info;
				p >> info;
				Entity* ent = getEntity(info.networkID);

				if (ent && !ent->isOwner())
				{
					ent->receivePacket(Packet(p));
				}
			} break;
		
			case hash("CreateEntity"):
			{
				//Server told us to create an entity, so we do.
				EntityInfo info;
				p >> info;

				std::cout << "client create ent : " << info.networkID << " " << info.ownerID << "\n";

				if (getEntity(info.networkID))
				{
					std::cout << "WAS ASKED TO CREATE EXISTING ENTITY " << info.networkID << ". REFUSING.\n";
					return;
				}

				switch(info.type)
				{
					case hash("Unit"):
					{
						entities.emplace_back(std::make_unique<Unit>(this->game_data));
						entities.back()->entity_info = info;

						//todo: refactor. once we have all 5 units from the other client constructed then we send the json load commands
						if (entities.back()->isOwner())
						{
							our_warband.addToNetworkIDs(info.networkID);
							//	once our warband has all 5 units pushed back, it sends 5 packets, 1 per unit
							//	saying which unit to load from json
							if (our_warband.getUnitNetworkIDsSize() == 5)
							{
								our_warband.sendJSONPackets();

								for (int i = 0; i < 5; i++)
								{
									Entity* ent = getEntity(our_warband.getUnitNetworkIDAt(i));
									Unit* unit = static_cast<Unit*>(ent);
									unit->loadFromJSON(our_warband.getUnitIDAt(i));
								}
							}
						}
						else
						{
							//for convenience, keep track of their unit network id's
							their_warband.addToNetworkIDs(info.networkID);
						}
					} break;
				}
			} break;
		}
	};

	managed_slot_1 = this->game_data->getNetworkManager()->client->on_packet_received.connect(clientPacketReceive);

	if (game_data->getNetworkManager()->server)
	{
		managed_slot_2 = this->game_data->getNetworkManager()->server->on_packet_received.connect(serverPacketReceive);
	}

	//todo: call create unit for our warband units
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

	if (client && client->isConnecting())
	{
		const float dt = gt.delta_time.count() / 1000.0f;

		for (auto& ent : entities)
		{
			ent->update(dt);
		}
	}

	if (game_data->getInputManager()->isActionPressed("escape"))
	{
		game_data->getStateManager()->pop();
	}
}

void GameState::render() const
{
	auto renderer = game_data->getRenderer();
	auto client = game_data->getNetworkManager()->client.get();

	if (client && client->isConnecting())
	{
		for (const auto& ent : entities)
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

	if (it != entities.end())
	{
		return it->get();
	}

	return nullptr;
}
