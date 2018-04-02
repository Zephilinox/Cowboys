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
	//TODO replace 1 with seed from map selection screen
	testGrid.generateCharGrid(1);
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

			case hash("UnitMove"):
			{
				this->game_data->getNetworkManager()->server->sendPacketToSomeClients(0, &p, ENET_PACKET_FLAG_RELIABLE, [senderID = p.senderID](const ClientInfo& ci)
				{
					return ci.id != senderID;
				});
			}

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
							Unit* unit = static_cast<Unit*>(ent);
							unit->setPosition(unit->entity_info.networkID * 40.0f, unit->entity_info.networkID * 40.0f);
							unit->setCurrentTile(testGrid.getTile(unit->entity_info.networkID, unit->entity_info.networkID));
							unit->getCurrentTile()->setIsBlocked(true);
						}
						else
						{
							//for convenience, keep track of their unit network id's
							their_warband.addToNetworkIDs(info.networkID);
						}


					} break;
				}
			} break;

			case hash("UnitMove"):
			{
				int packetStartX = 0;
				int packetStartY = 0;
				int packetEndX = 0;
				int packetEndY = 0;
				uint32_t netId = 0;

				p >> packetStartX >> packetStartY >> packetEndX >> packetEndY >> netId;

				Entity* ent_ptr = ent_man.getEntity(netId);
				Unit* unit = static_cast<Unit*>(ent_ptr);

				if(testGrid.findPathFromTo(&testGrid.map[packetStartX][packetStartY], &testGrid.map[packetEndX][packetEndY]))
				{
					unit->setPathToGoal(testGrid.getPathToGoal());
					std::cout << "unit moving\n";
					unit->move();
					unit->setCurrentTile(&testGrid.map[packetStartX][packetStartY]);
					unit->getCurrentTile()->setIsBlocked(false);
					unit->setCurrentTile(&testGrid.map[packetEndX][packetEndY]);
					unit->getCurrentTile()->setIsBlocked(true);
					testGrid.clearMoveData();
				}

				break;
			}
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
	game_data->getInputManager()->getMouseScreenPosition(mouseX, mouseY);

	const float dt = (float)gt.delta_time.count() / 1000.0f;

	screenScroll(dt, mouseX, mouseY);

	double mouse_x, mouse_y;
	game_data->getInputManager()->getMouseWorldPosition(mouse_x, mouse_y);


	if (client && client->isConnecting())
	{
		ent_man.update(dt);
	}

	if(game_data->getInputManager()->isMouseButtonPressed(0))
	{
		for(auto& ent : our_warband.getUnitNetworkIDs())
		{
			//TODO - stretch, show unit stats when left clicked on.
			Entity* ent_ptr = ent_man.getEntity(ent);
			Unit* unit = static_cast<Unit*>(ent_ptr);
			unit->setSelected(false);
			if(ASGE::Sprite* sprite = unit->getCurrentSprite())
			{
				float sprite_x = sprite->xPos();
				float sprite_y = sprite->yPos();
				float sprite_width = sprite->width();
				float sprite_height = sprite->height();

				if(sprite_x < mouse_x &&
					sprite_x + sprite_width > mouse_x &&
					sprite_y < mouse_y &&
					sprite_y + sprite_height > mouse_y)
				{
					std::cout << "unit selected\n";
					if(unit->getState() == Unit::UnitState::IDLE)
					{
						unit->setSelected(true);
						selected_unit_netID = unit->entity_info.networkID;
					}
				}
			}
		}
	}

	if(game_data->getInputManager()->isMouseButtonPressed(1) && (selected_unit_netID != 0))
	{
		for(auto& ent : their_warband.getUnitNetworkIDs())
		{
			//TODO - stretch, show unit stats when left clicked on.
			Entity* ent_ptr = ent_man.getEntity(ent);
			Unit* unit = static_cast<Unit*>(ent_ptr);
			if(ASGE::Sprite* sprite = unit->getCurrentSprite())
			{
				float sprite_x = sprite->xPos();
				float sprite_y = sprite->yPos();
				float sprite_width = sprite->width();
				float sprite_height = sprite->height();

				if(sprite_x < mouse_x &&
					sprite_x + sprite_width > mouse_x &&
					sprite_y < mouse_y &&
					sprite_y + sprite_height > mouse_y)
				{
					std::cout << "unit ATTACCCCKED\n";
					Entity* ent_attacker = ent_man.getEntity(selected_unit_netID);
					Unit* attacker = static_cast<Unit*>(ent_attacker);
					attacker->doAttack(unit);
					unit->setSelected(false);
					selected_unit_netID = 0;
					//TODO send attack packet
					break;
				}
			}

		}

		TerrainTile* temp = nullptr;
		TerrainTile* selected = nullptr;
		ASGE::Sprite* spr = nullptr;
		int packetStartX = 0;
		int packetStartY = 0;
		int packetEndX = 0;
		int packetEndY = 0;

		for(int x = 0; x < mapWidth; x++)
		{
			for(int y = 0; y < mapHeight; y++)
			{
				temp = testGrid.getTile(x, y);
				if(temp != nullptr)
				{
					spr = temp->getTerrainSprite();
					float sprite_x = spr->xPos();
					float sprite_y = spr->yPos();
					float sprite_width = spr->width();
					float sprite_height = spr->height();

					if(sprite_x < mouse_x &&
						sprite_x + sprite_width > mouse_x &&
						sprite_y < mouse_y &&
						sprite_y + sprite_height > mouse_y)
					{
						std::cout << "tile selected\n";
						//	spr->loadTexture("../../Resources/Textures/Tiles/grassTile.png");
						if(!temp->getIsBlocked())
						{
							selected = temp;
							break;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		for(auto& ent : ent_man.entities)
		{
			Entity* ent_ptr = ent.get();
			Unit* unit = dynamic_cast<Unit*>(ent_ptr);
			if(unit != nullptr)
			{
				if(unit->getSelected() && selected != nullptr)
				{
					//TODO find path from to needs target sprite
					if(testGrid.findPathFromTo(unit->getCurrentTile(), selected))
					{
						unit->setPathToGoal(testGrid.getPathToGoal());
						std::cout << "unit moving\n";
						unit->move();
						unit->getCurrentTile()->setIsBlocked(false);

						packetStartX = unit->getCurrentTile()->xCoord;
						packetStartY = unit->getCurrentTile()->yCoord;
						packetEndX = selected->xCoord;
						packetEndY = selected->yCoord;

						Packet lol;
						lol.setID(hash("UnitMove"));
						lol << packetStartX << packetStartY << packetEndX << packetEndY << unit->entity_info.networkID;
						game_data->getNetworkManager()->client->sendPacket(0, &lol);

						unit->setSelected(false);
						selected_unit_netID = 0;
						unit->setCurrentTile(selected);
						unit->getCurrentTile()->setIsBlocked(true);
						testGrid.clearMoveData();
					}
				}
			}
		}
	}


	if (game_data->getInputManager()->isActionPressed(hash("Escape")))
	{
		game_data->getStateManager()->push<StatePause>();
	}
}

void StateGame::screenScroll(float dt, double mouseX, double mouseY)
{
	double screen_edge_threshold = 30.0;
	int max_right_tile = (mapWidth - 32);
	int max_downtile = (mapHeight - 18);

	auto map_left = testGrid.getTileXPosAtArrayPos(0, 0);
	auto map_right = testGrid.getTileXPosAtArrayPos(max_right_tile, 0);
	auto map_top = testGrid.getTileYPosAtArrayPos(0, 0);
	auto map_bottom = testGrid.getTileYPosAtArrayPos(0, max_downtile);
	
	//left
	if ((mouseX <= screen_edge_threshold && mouseX > 0) || game_data->getInputManager()->isActionDown(hash("Left")))
	{
		if (offset_x > map_left)
		{
			offset_x -= 500.0f * dt;

			if (offset_x < map_left)
			{
				offset_x = map_left;
			}
		}
	}

	//right
	if ((mouseX >= (double)(game_data->getWindowWidth() - screen_edge_threshold) && mouseX < (double)(game_data->getWindowWidth()))
		|| game_data->getInputManager()->isActionDown(hash("Right")))
	{
		if (offset_x < map_right)
		{
			offset_x += 500.0f * dt;

			if (offset_x > map_right)
			{
				offset_x = map_right;
			}
		}
	}

	//up
	if ((mouseY <= screen_edge_threshold && mouseY > 0)
		|| game_data->getInputManager()->isActionDown(hash("Up")))
	{
		if (offset_y > map_top)
		{
			offset_y -= 500.0f * dt;

			if (offset_y < map_top)
			{
				offset_y = map_top;
			}
		}
	}

	//down
	if ((mouseY >= (double)(game_data->getWindowHeight() - screen_edge_threshold) && mouseY < (double)(game_data->getWindowHeight()))
		|| game_data->getInputManager()->isActionDown(hash("Down")))
	{
		if (offset_y < map_bottom)
		{
			offset_y += 500.0f * dt;

			if (offset_y > map_bottom)
			{
				offset_y = map_bottom;
			}
		}
	}

	testGrid.applyOffset(offset_x, offset_y);
	ent_man.applyOffset(offset_x, offset_y);
	game_data->getInputManager()->applyOffset(offset_x, offset_y);
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