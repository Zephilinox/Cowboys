#include "../States/StateGame.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../States/StateMenu.hpp"
#include "../States/StatePause.hpp"
#include "../States/StateGameOver.h"
#include "../../Architecture/Constants.hpp"
#include "../../Architecture/Networking/Client.hpp"
#include "../../Architecture/Networking/Server.hpp"
#include "../../Architecture/Audio/AudioLocator.hpp"
#include "../Action.h"


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
	game_grid(game_data)
{
	//TODO replace 1 with seed from map selection screen
	game_grid.generateCharGrid(1);
	game_grid.loadHardCodedMap();

	//Init UI panel
	UI_panel_sprite = game_data->getRenderer()->createUniqueSprite();
	UI_panel_sprite->loadTexture("../../Resources/Textures/UI/UnitStats.png");
	UI_panel_sprite->xPos(0.0f);
	UI_panel_sprite->yPos(game_data->getWindowHeight() - UI_panel_sprite->height());

	portrait_highlight = game_data->getRenderer()->createUniqueSprite();
	portrait_highlight->loadTexture("../../Resources/Textures/UI/SelectedPortrait.png");

	your_turn_sprite = game_data->getRenderer()->createUniqueSprite();
	your_turn_sprite->loadTexture("../../Resources/Textures/Turns/yourTurn.png");
	your_turn_sprite->yPos(game_data->getWindowHeight() * 0.15f);
	your_turn_sprite->xPos(game_data->getWindowWidth() * 0.3f);
	end_turn_sprite = game_data->getRenderer()->createUniqueSprite();
	end_turn_sprite->loadTexture("../../Resources/Textures/Turns/endTurn.png");
	end_turn_sprite->yPos(game_data->getWindowHeight() * 0.15f);
	end_turn_sprite->xPos(game_data->getWindowWidth() * 0.3f);
	end_round_sprite = game_data->getRenderer()->createUniqueSprite();
	end_round_sprite->loadTexture("../../Resources/Textures/Turns/endRound.png");
	end_round_sprite->yPos(game_data->getWindowHeight() * 0.15f);
	end_round_sprite->xPos(game_data->getWindowWidth() * 0.3f);

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
				this->game_data->getNetworkManager()->server->sendPacketToAllClients(0, &p, ENET_PACKET_FLAG_RELIABLE);
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

						int x_start_pos = game_grid.getJsonXPos(ent->entity_info.ownerID, ent->entity_info.networkID);
						int y_start_pos = game_grid.getJsonYPos(ent->entity_info.ownerID, ent->entity_info.networkID);

						Unit* unit = static_cast<Unit*>(ent);
						unit->setCurrentTile(game_grid.getTile(x_start_pos, y_start_pos));
						unit->setPosition(unit->getCurrentTile()->getTerrainSprite()->xPos(), unit->getCurrentTile()->getTerrainSprite()->yPos());
						unit->getCurrentTile()->setIsOccupied(true);

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
				bool hit;
				bool is_reactive;
				bool reactive_hit;

				p >> attacker_ID >> defender_ID >> hit >> is_reactive >> reactive_hit;

				Entity* ent_ptr = ent_man.getEntity(attacker_ID);
				Unit* attacker = static_cast<Unit*>(ent_ptr);

				//TODO action queue
				//Add attack action to action queue.
				Action new_action;
				if(is_reactive)
				{
					new_action.action_id = ActionID::REACTIVE_ATTACK;
				}
				else
				{
					new_action.action_id = ActionID::ATTACK;
				}
				new_action.actor_id = attacker_ID;
				new_action.defender_id = defender_ID;
				new_action.isHit = hit;
				new_action.isReactive = is_reactive;
				new_action.damage = attacker->getWeaponDamage();
				new_action.reactiveHit = reactive_hit;
				action_queue.addToQueue(new_action);

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

				Action new_action;
				new_action.action_id = ActionID::MOVE;
				new_action.actor_id = netId;
				new_action.defender_id = 0;
				new_action.isHit = false;
				new_action.damage = 0;
				new_action.start_pos_x_y = { packetStartX, packetStartY };
				new_action.end_pos_x_y = { packetEndX, packetEndY };

				action_queue.addToQueue(new_action);
				break;
			}
			case hash("EndTurn"):
			{
				Action new_action;
				new_action.action_id = ActionID::END_TURN;
				new_action.actor_id = 0;
				new_action.defender_id = 0;
				new_action.isHit = false;
				new_action.damage = 0;
				action_queue.addToQueue(new_action);
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

void StateGame::nextAction()
{
	if(action_queue.getSize() > 0)
	{
		std::cout << "Queue size: " << action_queue.getSize() << std::endl;
		Action action = action_queue.getNextAction();
		action_queue.removeCurrentAction();

		Entity* ent_ptr = ent_man.getEntity(action.actor_id);
		Unit* actor = static_cast<Unit*>(ent_ptr);

		switch(action.action_id)
		{
			case ActionID::REACTIVE_ATTACK:
			{
				Entity* ent_defender = ent_man.getEntity(action.defender_id);
				Unit* defender = static_cast<Unit*>(ent_defender);

				if(action.isHit && actor->getIsAlive())
				{
					actor->reactiveAttack(defender);
				}
				break;
			}
			case ActionID::ATTACK:
			{
				Entity* ent_defender = ent_man.getEntity(action.defender_id);
				Unit* defender = static_cast<Unit*>(ent_defender);

				//TODO redo unit attack based on if action.isHit == true
				actor->doAttack(defender);

				sendAttackPacket(action.defender_id, action.actor_id, action.reactiveHit, true, true);

				break;
			}
			case ActionID::MOVE:
			{
				if(game_grid.findPathFromTo
				(&game_grid.map[action.start_pos_x_y.first][action.start_pos_x_y.second],
					&game_grid.map[action.end_pos_x_y.first][action.end_pos_x_y.second]))
				{
					actor->setPathToGoal(game_grid.getPathToGoal());
					std::cout << "unit moving\n";
					actor->move();
					actor->setCurrentTile
					(&game_grid.map[action.start_pos_x_y.first][action.start_pos_x_y.second]);
					actor->getCurrentTile()->setIsOccupied(false);
					game_grid.clearMoveData();
				}
				break;
			}
			case ActionID::END_TURN:
			{
				endTurn();
				break;
			}
			default:
			{
				std::cout << "Incorrect action_ID!" << std::endl;
				break;
			}
		}

		checkWinCondition();

	}
}

void StateGame::checkWinCondition()
{
	int enemies_dead = 0;
	int friendlies_dead = 0;

	for(auto& ent_net_id : their_warband.getUnitNetworkIDs())
	{
		Entity* ent = ent_man.getEntity(ent_net_id);
		Unit* unit = static_cast<Unit*>(ent);

		if(!unit->getIsAlive())
		{
			their_warband.setIsAlive(ent_net_id, false);
			enemies_dead++;
		}
	}
	for(auto& ent_net_id : our_warband.getUnitNetworkIDs())
	{
		Entity* ent = ent_man.getEntity(ent_net_id);
		Unit* unit = static_cast<Unit*>(ent);

		if(!unit->getIsAlive())
		{
			our_warband.setIsAlive(ent_net_id, false);
			friendlies_dead++;
		}
	}

	if(enemies_dead >= 5)
	{
		//TODO you win state
		game_data->getStateManager()->push<StateGameOver>(true);
	}

	if(friendlies_dead >= 5)
	{
		//TODO you lose state
		game_data->getStateManager()->push<StateGameOver>(false);
	}

}

bool StateGame::attackAccuracyCheck(float unit_accuracy)
{
	float result = rand_no_generator.getRandomFloat(0.1f, 10.0f);

	if(result < unit_accuracy)
	{
		return true;
	}
	return false;
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
			//give all units reference to grid
			for(auto& ent : ent_man.entities)
			{
				//TODO -  highligh unit stats when left clicked on.
				Unit* unit = static_cast<Unit*>(ent.get());
				unit->setGrid(game_grid);
			}
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
					first->setInitiative(first->getInitiative() + 0.2f);
				}
				else
				{
					second->setInitiative(second->getInitiative() + 0.2f);
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

			updateAllFogOfWar();
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


	//TODO Action Queue
	//go through action queue here with timer between actions as 0.25f seconds?
	if(queue_timer <= 0.0f)
	{
		nextAction();
		queue_timer = default_queue_timer;
	}
	else
	{
		queue_timer -= dt;
	}

	if(client && client->isConnecting())
	{
		ent_man.update(dt);
	}

	movingUnitLineOfSight();

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
					//	TODO - Accuracy check here
						
						bool attack_hit = attackAccuracyCheck(attacker->getFiringAccuracy());
						bool reactive_hit = attackAccuracyCheck(unit->getFiringAccuracy());

						sendAttackPacket(active_turn_unit, ent_net_id, attack_hit, false, reactive_hit);
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
						temp = game_grid.getTile(x, y);
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

				Unit* active_unit = static_cast<Unit*>(ent_man.getEntity(active_turn_unit));
				if(selected != nullptr && active_turn_warband == &our_warband
					&& active_unit->getSelected())
				{
					if(game_grid.findPathFromTo(active_unit->getCurrentTile(), selected))
					{
						//active_unit->setPathToGoal(game_grid.getPathToGoal());
						std::cout << "unit move attempted";
						if (active_unit->getTimeUnits() >= game_grid.getPathToGoal().at(0).time_units)
						{

							packetStartX = active_unit->getCurrentTile()->xCoord;
							packetStartY = active_unit->getCurrentTile()->yCoord;
							packetEndX = selected->xCoord;
							packetEndY = selected->yCoord;

							Packet p;
							p.setID(hash("UnitMove"));
							p << packetStartX << packetStartY << packetEndX << packetEndY << active_unit->entity_info.networkID;
							game_data->getNetworkManager()->client->sendPacket(0, &p);
						}
						else
						{
							std::cout << "Insufficient Time units";
						}

						game_grid.clearMoveData();
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

	auto map_left = game_grid.getTileXPosAtArrayPos(0, 0);
	auto map_right = game_grid.getTileXPosAtArrayPos(max_right_tile, 0);
	auto map_top = game_grid.getTileYPosAtArrayPos(0, 0);
	auto map_bottom = game_grid.getTileYPosAtArrayPos(0, max_downtile);
	
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

	game_grid.applyOffset(offset_x, offset_y);
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
	game_grid.render();
	menu.render(Z_ORDER_LAYER::OVERLAY);
	renderUnitStatsToPanel();

	if(endRoundTimer > 0.0f)
	{
		game_data->getRenderer()->renderSprite(*end_round_sprite, Z_ORDER_LAYER::OVERLAY_TEXT + 500.0f);
	}
	else
	{
		if(yourTurnTimer > 0.0f)
		{
			game_data->getRenderer()->renderSprite(*your_turn_sprite, Z_ORDER_LAYER::OVERLAY_TEXT + 500.0f);
		}
		else if(endTurnTimer > 0.0f)
		{
			game_data->getRenderer()->renderSprite(*end_turn_sprite, Z_ORDER_LAYER::OVERLAY_TEXT + 500.0f);
		}
	}	
}

void StateGame::onActive()
{

}

void StateGame::onInactive()
{
}

void StateGame::updateAllFogOfWar()
{
	game_grid.unseeAllTiles();
	for(auto& ent : ent_man.entities)
	{
		if(ent->entity_info.type == hash("Unit") && ent->isOwner())
		{
			Unit* unit = static_cast<Unit*>(ent.get());
			updateUnitFogOfWar(unit);
		}
	}
}

void StateGame::updateUnitFogOfWar(Unit* unit)
{
	int unit_x = unit->getCurrentTile()->xCoord;
	int unit_y = unit->getCurrentTile()->yCoord;
	for(int wid = 0; wid < mapWidth; wid++)
	{
		for(int height = 0; height < mapHeight; height++)
		{
			game_grid.getFogOfWar(unit->getViewDistance(), unit_x, unit_y, wid, height);
		}
	}
}

void StateGame::sendEndTurnPacket()
{
	Packet p;
	p.setID(hash("EndTurn"));
	game_data->getNetworkManager()->client->sendPacket(0, &p);
}

void StateGame::sendAttackPacket(uint32_t attacker_ID, uint32_t defender_ID, bool is_hit, bool is_reactive, bool reactive_hit)
{
	Packet p;
	p.setID(hash("UnitAttack"));
	p << attacker_ID << defender_ID << is_hit << is_reactive << reactive_hit;
	std::cout << "attack packet send\n";
	game_data->getNetworkManager()->client->sendPacket(0, &p);
}

void StateGame::movingUnitLineOfSight()
{
	for(auto& ent : ent_man.entities)
	{
		if(ent->isOwner())
		{
			Unit* unit = static_cast<Unit*>(ent.get());
			if(unit->getIsMoving())
			{
				updateUnitFogOfWar(unit);
			}
		}
	}
}

void StateGame::endTurn()
{
	bool your_turn_now = false;
	bool end_of_turn = false;
	bool end_of_round = false;

	//clear the queue
	for(unsigned int index = 0; index < action_queue.getSize(); index++)
	{
		action_queue.removeCurrentAction();
	}
	
	active_turn_warband->endTurn(ent_man, active_turn_unit);
	//Swap active warband
	if(active_turn_warband == &our_warband)
	{
		our_warband.unitActed(active_turn_unit, true);
		end_of_turn = true;
		active_turn_warband = &their_warband;
		menu.getButton(0).setEnabled(false);
	}
	else
	{
		their_warband.unitActed(active_turn_unit, true);
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
	
	updateAllFogOfWar();

	active_turn_unit = active_turn_warband->getNextUnitInInitiativeList();

	Entity* ent_active = ent_man.getEntity(active_turn_unit);
	if(active_turn_unit != 0)
	{
		Unit* un = static_cast<Unit*>(ent_active);
		un->setActiveTurn(true);
	}
	
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
