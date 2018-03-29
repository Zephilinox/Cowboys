#include "../States/StateGame.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../../Architecture/Constants.hpp"
#include "../../Architecture/Networking/Client.hpp"
#include "../../Architecture/Networking/Server.hpp"


//TODO - send packet to server to request coin flip to determine who goes first!
//TODO - send packet to declare turn over
//TODO - send packet allUnitsActed
//TODO - receive packet to server to request coin flip to determine who goes first!
//TODO - receive packet to declare turn over
//TODO - receive packet allUnitsActed


StateGame::StateGame(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID)
	: State(game_data)
	, menu(game_data)
	, our_warband(game_data, unit1ID, unit2ID, unit3ID, unit4ID, unit5ID)
	, their_warband(game_data)
	, ent_man(game_data)
{
	//TODO create map

	//This is lobby-related, leave it for now until I have a closer look at it
	Packet p;
	p.setID(hash("GameJoined"));
	this->game_data->getNetworkManager()->client->sendPacket(0, &p);

	auto serverPacketReceive = [this](Packet p)
	{
		switch (p.getID())
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
				//once a client has started their game (pushed StateGame), send them all existing entities.
				for (const auto& ent : ent_man.entities)
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
				Entity* ent = ent_man.getEntity(info.networkID);

				//Make sure the entity packet is all valid, then send it to the other client.
				if (ent && //exists
					ent->entity_info.ownerID == info.ownerID && //owners match
					ent->entity_info.type == info.type && //types match
					info.ownerID == p.senderID) //client owns it
				{
					this->game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [senderID = p.senderID](const ClientInfo& ci)
					{
						return ci.id != senderID;
					});
				}
				else
				{
					std::cout << "uhoh... server received invalid Entitypacketfrom client " + p.senderID << "\n";
				}
			} break;

			case hash("CreateEntity"):
			{
				//Call createEntity based on the type
				EntityInfo info;
				p >> info;
				std::cout << "server create ent : " << info.networkID << " " << info.ownerID << "\n";

				switch (info.type)
				{
					case hash("Unit"):
					{
						ent_man.createEntityForClient<Unit>(p.senderID);
					} break;
				}
			} break;
		}
	};

	auto clientPacketReceive = [this](Packet p)
	{
		switch (p.getID())
		{
			case hash("Entity"):
			{
				//Find the entity and give them the packet to deserialize
				EntityInfo info;
				p >> info;
				Entity* ent = ent_man.getEntity(info.networkID);

				if (ent)
				{
					if (!ent->isOwner())
					{
						ent->receivePacket(Packet(p));
					}
					else
					{
						std::cout << "huh... received EntityPacket for an entity that we own?\n";
					}
				}
				else
				{
					std::cout << "uhoh, received EntityPacket for an entity that doesn't exist?\n";
				}
			} break;

			case hash("CreateEntity"):
			{
				//Server told us to create an entity, so we do.
				EntityInfo info;
				p >> info;

				std::cout << "client create ent : " << info.networkID << " " << info.ownerID << "\n";

				if (ent_man.getEntity(info.networkID))
				{
					std::cout << "WAS ASKED TO CREATE EXISTING ENTITY " << info.networkID << ". REFUSING.\n";
					return;
				}

				switch (info.type)
				{
					case hash("Unit"):
					{
						ent_man.entities.emplace_back(std::make_unique<Unit>(this->game_data, &ent_man));
						ent_man.entities.back()->entity_info = info;
						ent_man.entities.back()->onSpawn();

						if (ent_man.entities.back()->isOwner())
						{
							our_warband.addToNetworkIDs(info.networkID);
							our_warband.checkReady(ent_man);
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

	//Create 5 units for our warband
	for (int i = 0; i < 5; ++i)
	{
		ent_man.createEntityRequest<Unit>();
	}

	game_data->getMusicPlayer()->play("Piano Loop");
}

StateGame::~StateGame()
{
	game_data->getNetworkManager()->stopServer();
	game_data->getNetworkManager()->stopClient();
	ent_man.entities.clear();
}

void StateGame::update(const ASGE::GameTime& gt)
{
	auto client = game_data->getNetworkManager()->client.get();
	auto server = game_data->getNetworkManager()->server.get();

	if (client && client->isConnecting())
	{
		const float dt = (float)gt.delta_time.count() / 1000.0f;

		for (auto& ent : ent_man.entities)
		{
			ent->update(dt);
		}
	}

	if (game_data->getInputManager()->isActionPressed(hash("Escape")))
	{
		game_data->getStateManager()->pop();
	}
}

void StateGame::render() const
{
	auto renderer = game_data->getRenderer();
	auto client = game_data->getNetworkManager()->client.get();

	if (client && client->isConnecting())
	{
		for (const auto& ent : ent_man.entities)
		{
			ent->render(renderer);
		}
	}
}

void StateGame::onActive()
{
	game_data->getMusicPlayer()->play("Piano Loop");
}

void StateGame::onInactive()
{
}

void StateGame::endTurn()
{
	//set active player's warband.endTurn(networkID)
	if(our_warband.getAllUnitsActed() && their_warband.getAllUnitsActed())
	{
		endRound();
	}
}

void StateGame::endRound()
{
	our_warband.resetAllActed();
	their_warband.resetAllActed();
}