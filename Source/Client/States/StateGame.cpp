#include "../States/StateGame.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../States/StatePause.hpp"
#include "../../Architecture/Constants.hpp"
#include "../../Architecture/Networking/Client.hpp"
#include "../../Architecture/Networking/Server.hpp"
#include "../../Architecture/Audio/AudioLocator.hpp"


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

	//Init UI panel
	UI_panel_sprite = game_data->getRenderer()->createUniqueSprite();
	UI_panel_sprite->loadTexture("../../Resources/Textures/UI/UnitStats.png");
	UI_panel_sprite->xPos(0.0f);
	UI_panel_sprite->yPos(game_data->getWindowHeight() - UI_panel_sprite->height());

	portrait_highlight = game_data->getRenderer()->createUniqueSprite();
	portrait_highlight->loadTexture("../../Resources/Textures/UI/SelectedPortrait.png");

	yourTurnSprite = game_data->getRenderer()->createUniqueSprite();
	yourTurnSprite->loadTexture("../../Resources/Textures/Turns/yourTurn.png");
	yourTurnSprite->yPos(game_data->getWindowHeight() * 0.15f);
	yourTurnSprite->xPos(game_data->getWindowWidth() * 0.3f);
	endTurnSprite = game_data->getRenderer()->createUniqueSprite();
	endTurnSprite->loadTexture("../../Resources/Textures/Turns/endTurn.png");
	endTurnSprite->yPos(game_data->getWindowHeight() * 0.15f);
	endTurnSprite->xPos(game_data->getWindowWidth() * 0.3f);
	endRoundSprite = game_data->getRenderer()->createUniqueSprite();
	endRoundSprite->loadTexture("../../Resources/Textures/Turns/endRound.png");
	endRoundSprite->yPos(game_data->getWindowHeight() * 0.15f);
	endRoundSprite->xPos(game_data->getWindowWidth() * 0.3f);

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
				//p.reset();
				//p.setID(hash("GameStart"));
				//this->game_data->getNetworkManager()->server->sendPacketToOneClient(p.senderID, 0, &p);
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
				break;
			}
			case hash("UnitAttack"):
			{
				this->game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p, ENET_PACKET_FLAG_RELIABLE);
				std::cout << "attack packet received (server)";
				break;
			}
			case hash("EndTurn"):
			{
				this->game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p, ENET_PACKET_FLAG_RELIABLE);
				break;
			}


			case hash("Disconnected"):
			{
				//When a client disconnects, keep their entities rather than delete them.
				//So if they rejoin they're all still there? maybe? not sure how to go about this really.
				/*std::experimental::erase_if(entities, [p](const auto& entity)
				{
				return entity->entity_info.ownerID == p.senderID;
				});*/
				this->game_data->getStateManager()->pop();
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
			case hash("Disconnected"):
			{
				this->game_data->getStateManager()->pop();
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
						auto ent = ent_man.spawnEntity<Unit>(info);

						int x_start_pos = testGrid.getJsonXPos(ent->entity_info.ownerID, ent->entity_info.networkID);
						int y_start_pos = testGrid.getJsonYPos(ent->entity_info.ownerID, ent->entity_info.networkID);

						Unit* unit = static_cast<Unit*>(ent);
						unit->setCurrentTile(testGrid.getTile(x_start_pos, y_start_pos));
						unit->setPosition(unit->getCurrentTile()->getTerrainSprite()->xPos(), unit->getCurrentTile()->getTerrainSprite()->yPos());
						unit->getCurrentTile()->setIsBlocked(true);

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

			case hash("UnitAttack"):
			{
				uint32_t attacker_ID;
				uint32_t defender_ID;

				p >> attacker_ID >> defender_ID;

				Entity* ent_attacker = ent_man.getEntity(attacker_ID);
				Unit* attacker = static_cast<Unit*>(ent_attacker);
				Entity* ent_defender = ent_man.getEntity(defender_ID);
				Unit* defender = static_cast<Unit*>(ent_defender);

				attacker->doAttack(defender);
				std::cout << "attack packet received (clent)\n";
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

			case hash("EndTurn"):
			{
				endTurn();
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

	menu.addButton(game_data->getWindowWidth() - 100, game_data->getWindowHeight() - 300, "End Turn", ASGE::COLOURS::GREEN, ASGE::COLOURS::GREENYELLOW, 110.0f, 20.0f, "UI/lobbyButton");
	menu.getButton(0).on_click.connect([this]()
	{
		//send packet to trigger same function on other client here
		sendEndTurnPacket();
	});

	game_data->getMusicPlayer()->addMusicToPlaylist("game", "Piano Loop");
	game_data->getMusicPlayer()->addMusicToPlaylist("game", "FF7");
	game_data->getMusicPlayer()->startPlaylist("game");
}

StateGame::~StateGame()
{
	game_data->getNetworkManager()->stopServer();
	game_data->getNetworkManager()->stopClient();
}

void StateGame::update(const ASGE::GameTime& gt)
{
	//RICARDO - this won't cause any issue right?
	//right
	if(menu.getButton(0).isEnabled())
	{
		menu.update();
	}

	if(endRoundTimer > 0.0f)
	{
		endRoundTimer -= (float)gt.delta_time.count() / 1000.0f;;
	}
	else if(yourTurnTimer > 0.0f)
	{
		yourTurnTimer -= (float)gt.delta_time.count() / 1000.0f;;
	}
	else if(endTurnTimer > 0.0f)
	{
		endTurnTimer -= (float)gt.delta_time.count() / 1000.0f;;
	}



	if(our_warband.getUnitNetworkIDsSize() == 5 && their_warband.getUnitNetworkIDsSize() == 5
		&& gameReady == false)
	{
		gameReady = true;

		//check all entities in each warband are fully initialised
		for(auto& unit : our_warband.getUnitNetworkIDs())
		{
			Unit* un = static_cast<Unit*>(ent_man.getEntity(unit));
			if(!un->getInitialised())
			{
				gameReady = false;
			}
		}
		for(auto& unit : their_warband.getUnitNetworkIDs())
		{
			Unit* un = static_cast<Unit*>(ent_man.getEntity(unit));
			if(!un->getInitialised())
			{
				gameReady = false;
			}
		}

		if(gameReady)
		{
			//Now both warbands are initialised, init & sort their initiative trackers
			our_warband.initInitiativeTracker(ent_man);
			their_warband.initInitiativeTracker(ent_man);

			uint32_t test1 = our_warband.getNextUnitInInitiativeList();
			uint32_t test2 = their_warband.getNextUnitInInitiativeList();
			Unit* first = static_cast<Unit*>(ent_man.getEntity(test1));
			Unit* second = static_cast<Unit*>(ent_man.getEntity(test2));

			//Prevents units with same initiative breaking the ordering of turns
			if(first->getInitiative() == second->getInitiative())
			{
				if(game_data->getNetworkManager()->client->getID() == 2)
				{
					first->setInitiative(first->getInitiative() + 0.2);
				}
				else
				{
					second->setInitiative(second->getInitiative() + 0.2);
				}
			}

			//Check which warband has the highest starting initiative.
			if(first->getInitiative() > second->getInitiative())
			{
				active_turn_warband = &our_warband;
				menu.getButton(0).setEnabled(true);
			}
			else
			{
				active_turn_warband = &their_warband;
				menu.getButton(0).setEnabled(false);
			}
			active_turn_unit = active_turn_warband->getNextUnitInInitiativeList();
			static_cast<Unit*>(ent_man.getEntity(active_turn_unit))->setActiveTurn(true);
		}

	}

	auto client = game_data->getNetworkManager()->client.get();
	auto server = game_data->getNetworkManager()->server.get();

	double mouseX;
	double mouseY;
	game_data->getInputManager()->getMouseScreenPosition(mouseX, mouseY);

	const float dt = (float)gt.delta_time.count() / 1000.0f;

	screenScroll(dt, mouseX, mouseY);

	double mouse_x, mouse_y;
	game_data->getInputManager()->getMouseWorldPosition(mouse_x, mouse_y);


	if(client && client->isConnecting())
	{
		ent_man.update(dt);
	}

	if(gameReady)
	{
		if(game_data->getInputManager()->isMouseButtonPressed(0))
		{
			for(auto& ent : ent_man.entities)
			{
				//TODO -  highligh unit stats when left clicked on.
				Unit* unit = static_cast<Unit*>(ent.get());
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
						}
					}
				}
			}
		}

		if(game_data->getInputManager()->isMouseButtonPressed(1))
		{
			bool attacking = false;

			for(auto& ent_net_id : their_warband.getUnitNetworkIDs())
			{
				Entity* ent_ptr = ent_man.getEntity(ent_net_id);
				Unit* unit = static_cast<Unit*>(ent_ptr);


				//ATTACKING
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
						Entity* ent_attacker = ent_man.getEntity(active_turn_unit);
						Unit* attacker = static_cast<Unit*>(ent_attacker);

						//TODO STRETCH check manhattan distance from attacker to defender is within ATTACK RANGE rather than view distance?
						//if(attacker->getViewDistance() > testGrid.getManhattanDistance(attacker->getCurrentTile(), unit->getCurrentTile()))
						//{
						//	sendAttackPacket(active_turn_unit, ent_net_id);
						//}

						//testing
						sendAttackPacket(active_turn_unit, ent_net_id);
						attacking = true;
						break;
					}
				}
			}

			if(attacking == false)
			{
				//Detect terrain selection
				TerrainTile* temp = nullptr;
				TerrainTile* selected = nullptr;
				ASGE::Sprite* spr = nullptr;
				int packetStartX = 0;
				int packetStartY = 0;
				int packetEndX = 0;
				int packetEndY = 0;

				//TERRAIN TILE SELECTION
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
				//change this to, active unit selected ONLY, so other's cannot act without being their turn
				Unit* active_unit = static_cast<Unit*>(ent_man.getEntity(active_turn_unit));

				if(selected != nullptr && active_turn_warband == &our_warband
					&& active_unit->getSelected())
				{
					if(testGrid.findPathFromTo(active_unit->getCurrentTile(), selected))
					{
						active_unit->setPathToGoal(testGrid.getPathToGoal());
						std::cout << "unit move attempted";
						if(active_unit->getTimeUnits() >= 10.0f)
						{
							active_unit->move();
							active_unit->getCurrentTile()->setIsBlocked(false);

							packetStartX = active_unit->getCurrentTile()->xCoord;
							packetStartY = active_unit->getCurrentTile()->yCoord;
							packetEndX = selected->xCoord;
							packetEndY = selected->yCoord;

							Packet p;
							p.setID(hash("UnitMove"));
							p << packetStartX << packetStartY << packetEndX << packetEndY << active_unit->entity_info.networkID;
							game_data->getNetworkManager()->client->sendPacket(0, &p);
							active_unit->setCurrentTile(selected);
							active_unit->getCurrentTile()->setIsBlocked(true);
						}
						else
						{
							std::cout << "Insufficient Time units";
						}

						testGrid.clearMoveData();
					}
				}

			}
		}
		if(game_data->getInputManager()->isActionPressed(hash("Escape")))
		{
			game_data->getStateManager()->push<StatePause>();
		}
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

	game_data->getRenderer()->renderSprite(*UI_panel_sprite, Z_ORDER_LAYER::OVERLAY);
	testGrid.render();
	menu.render(Z_ORDER_LAYER::OVERLAY);
	renderUnitStatsToPanel();
	if(endRoundTimer > 0.0f)
	{
		game_data->getRenderer()->renderSprite(*endRoundSprite, Z_ORDER_LAYER::OVERLAY_TEXT + 500.0f);
	}
	else
	{
		if(yourTurnTimer > 0.0f)
		{
			game_data->getRenderer()->renderSprite(*yourTurnSprite, Z_ORDER_LAYER::OVERLAY_TEXT + 500.0f);
		}
		else if(endTurnTimer > 0.0f)
		{
			game_data->getRenderer()->renderSprite(*endTurnSprite, Z_ORDER_LAYER::OVERLAY_TEXT + 500.0f);
		}
	}	
}

void StateGame::onActive()
{

}

void StateGame::onInactive()
{
}

void StateGame::sendEndTurnPacket()
{
	Packet p;
	p.setID(hash("EndTurn"));
	game_data->getNetworkManager()->client->sendPacket(0, &p);
}

void StateGame::sendAttackPacket(uint32_t attacker_ID, uint32_t defender_ID)
{
	Packet p;
	p.setID(hash("UnitAttack"));
	p << attacker_ID << defender_ID;
	std::cout << "attack packet send\n";
	game_data->getNetworkManager()->client->sendPacket(0, &p);
}

void StateGame::endTurn()
{
	bool your_turn_now = false;
	bool end_of_turn = false;
	bool end_of_round = false;


	active_turn_warband->endTurn(ent_man, active_turn_unit);
	//Swap active warband
	if(active_turn_warband == &our_warband)
	{
		our_warband.unitActed(active_turn_unit, true);
		//TODO play ENDTURN animation
		end_of_turn = true;
		active_turn_warband = &their_warband;
		menu.getButton(0).setEnabled(false);
	}
	else
	{
		their_warband.unitActed(active_turn_unit, true);
		//TODO play YOURTURN animation
		your_turn_now = true;
		active_turn_warband = &our_warband;
		menu.getButton(0).setEnabled(true);
	}

	our_warband.checkAllActed();
	their_warband.checkAllActed();

	//set active player's warband.endTurn(networkID)
	if(our_warband.getAllUnitsActed() && their_warband.getAllUnitsActed())
	{
		end_of_round = true;
		endRound();
		//TODO Play ROUNDEND animation
	}

	if(end_of_round)
	{
		endRoundTimer = 1.5f;
	}
	else if(end_of_turn)
	{
		endTurnTimer = 1.5f;
	}
	else if(your_turn_now)
	{
		yourTurnTimer = 1.5f;
	}

	active_turn_unit = active_turn_warband->getNextUnitInInitiativeList();
	static_cast<Unit*>(ent_man.getEntity(active_turn_unit))->setActiveTurn(true);
}

void StateGame::endRound()
{
	our_warband.resetAllActed();
	their_warband.resetAllActed();
}

void StateGame::renderUnitStatsToPanel() const
{
	int x_pos = 10;
	for(auto& ent : ent_man.entities)
	{
		if(ent->isOwner())
		{
			Unit* unit = static_cast<Unit*>(ent.get());

			if(unit->getInitialised())
			{
				const int baseHeight = 600;
				//game_data->getRenderer()->renderText(unit->getNickName(), x_pos, baseHeight, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
				game_data->getRenderer()->renderText("Health: " + std::to_string((int)unit->getHealth()), x_pos, baseHeight + 15, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
				game_data->getRenderer()->renderText("T. Units: " + std::to_string((int)unit->getTimeUnits()), x_pos, baseHeight + 30, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
				game_data->getRenderer()->renderText("Stamina: " + std::to_string((int)unit->getStamina()), x_pos, baseHeight + 45, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
				game_data->getRenderer()->renderText("Bravery: " + std::to_string((int)unit->getBravery()), x_pos, baseHeight + 60, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
				game_data->getRenderer()->renderText("Accuracy: " + std::to_string((int)unit->getFiringAccuracy()), x_pos, baseHeight + 75, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
				game_data->getRenderer()->renderText("Strength: " + std::to_string((int)unit->getStrength()), x_pos, baseHeight + 90, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
				game_data->getRenderer()->renderText("Initiative: " + std::to_string((int)unit->getInitiative()), x_pos, baseHeight + 105, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
				unit->getPortraitSprite()->xPos(x_pos + 136);
				unit->getPortraitSprite()->yPos(baseHeight - 71);
				game_data->getRenderer()->renderSprite(*unit->getPortraitSprite(), Z_ORDER_LAYER::OVERLAY_TEXT + 2);
				x_pos += 256.0f;

				if(unit->getSelected())
				{
					portrait_highlight->xPos(unit->getPortraitSprite()->xPos());
					portrait_highlight->yPos(unit->getPortraitSprite()->yPos());
					game_data->getRenderer()->renderSprite(*portrait_highlight, Z_ORDER_LAYER::OVERLAY_TEXT + 3);
				}

			}
		}

	}
}
