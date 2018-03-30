#include "../States/StateGame.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../States/StatePause.hpp"
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
	, ent_man(game_data),
	testGrid(game_data)
{
	//TODO create map
	testGrid.generateCharGrid(10);
	testGrid.loadHardCodedMap();

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
						auto ent = ent_man.spawnEntity<Unit>(info);

						if (ent->isOwner())
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

	managed_slot_1 = this->game_data->getNetworkManager()->client->on_packet_received.connect(std::move(clientPacketReceive));

	if (game_data->getNetworkManager()->server)
	{
		managed_slot_2 = this->game_data->getNetworkManager()->server->on_packet_received.connect((serverPacketReceive));
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
}

void StateGame::update(const ASGE::GameTime& gt)
{
	auto client = game_data->getNetworkManager()->client.get();
	auto server = game_data->getNetworkManager()->server.get();

	double mouseX;
	double mouseY;
	game_data->getInputManager()->getMousePosition(mouseX, mouseY);

	screenScroll(mouseX, mouseY);

	if (client && client->isConnecting())
	{
		const float dt = (float)gt.delta_time.count() / 1000.0f;
		ent_man.update(dt);
	}

	if (game_data->getInputManager()->isActionPressed(hash("Escape")))
	{
		game_data->getStateManager()->push<StatePause>();
	}
}

void StateGame::screenScroll(double mouseX, double mouseY)
{
	double screenEdgeThreshold = 30.0;
	int maxRightTile = (mapWidth - 32);
	int maxDownTile = (mapHeight - 18);

	if(mouseX <= screenEdgeThreshold && testGrid.getTileXPosAtArrayPos(0,0) < 0.0f)
	{
		//scroll left
		testGrid.applyOffset(5.0f, 0.0f);
		ent_man.applyOffsetToUnits(5.0f, 0.0f);
	}

	if(mouseX >= (double)(game_data->getWindowWidth() - screenEdgeThreshold) && testGrid.getTileXPosAtArrayPos(maxRightTile, 0) > 0.0f)
	{
		//scroll right
		testGrid.applyOffset(-5.0f, 0.0f);
		ent_man.applyOffsetToUnits(-5.0f, 0.0f);
	}
	if(mouseY <= screenEdgeThreshold && testGrid.getTileYPosAtArrayPos(0, 0) < 0.0f)
	{
		//scroll up
		testGrid.applyOffset(0.0f, 5.0f);
		ent_man.applyOffsetToUnits(0.0f, 5.0f);
	}
	if(mouseY >= (double)(game_data->getWindowHeight() - screenEdgeThreshold) && testGrid.getTileYPosAtArrayPos(0, maxDownTile) > 0.0f)
	{
		//scroll down
		testGrid.applyOffset(0.0f, -5.0f);
		ent_man.applyOffsetToUnits(0.0f, -5.0f);
	}
}

void StateGame::render() const
{
	auto client = game_data->getNetworkManager()->client.get();

	if (client && client->isConnecting())
	{
		ent_man.render();
	}

	testGrid.render();
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