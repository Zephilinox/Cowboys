#include "Unit.hpp"

//LIBS
#include <Engine/Renderer.h>

//SELF
#include "../Architecture/Rng.hpp"
#include "../Architecture/Constants.hpp"
#include "../Architecture/Managers/EntityManager.hpp"

Unit::Unit(GameData* game_data, EntityManager* ent_man)
	: Entity(game_data, ent_man, hash("Unit"))
	, horizontal_walk_sprite(game_data->getRenderer(), true)
	, forward_walk_sprite(game_data->getRenderer(), true)
	, backward_walk_sprite(game_data->getRenderer(), true)
{
	idle_sprite_forward = game_data->getRenderer()->createUniqueSprite();
	idle_sprite_back = game_data->getRenderer()->createUniqueSprite();
	idle_sprite_left = game_data->getRenderer()->createUniqueSprite();
	selected_sprite = game_data->getRenderer()->createUniqueSprite();

	if (!selected_sprite->loadTexture("../../Resources/Textures/UI/Selected.png"))
	{
		throw;
	}
}

void Unit::onSpawn()
{
	//randomly position unit, test move packet
	if (isOwner())
	{
		std::cout << "owner on spawn for " << entity_info.networkID << ", " << entity_info.ownerID << "\n";
		//TODO change this to be read in from JSON
		//setPosition(entity_info.networkID * 40.0f, entity_info.networkID * 40.0f);
		//serializePacketType = PacketType::SET_POSITION;
		//sendPacket();
		
		//serializePacketType = PacketType::ATTACK;
		//sendPacket();
	}
}

void Unit::setCurrentTile(TerrainTile * new_tile)
{
	current_tile = new_tile;
}

void Unit::update(float dt)
{
	if (!initialized)
	{
		return;
	}

	commonUpdate(dt);
}

void Unit::render(ASGE::Renderer* renderer) const
{
	if (!initialized)
	{
		return;
	}

	renderer->renderSprite(*getCurrentSprite(), Z_ORDER_LAYER::UNITS + this->y_position);

	if (selected)
	{
		//todo: align this to the match the tile position when the unit is idle
		selected_sprite->xPos(getCurrentSprite()->xPos());
		selected_sprite->yPos(getCurrentSprite()->yPos());
		renderer->renderSprite(*selected_sprite, Z_ORDER_LAYER::UNITS + 1000.0f + this->y_position);
	}
}

void Unit::serialize(Packet& p)
{
	p << serializePacketType;

	switch (serializePacketType)
	{
		case SET_POSITION:
		{
			std::cout << "sending SET_POSITION\n";
			p << x_position << y_position;
		} break;
		case ATTACK:
		{
			std::cout << "sending ATTACK\n";
			p << 1;
		} break;
		//case MOVE:
		//{
		//	std::cout << "sending MOVE\n";
		//	p << target_x_position << target_y_position;
		//} break;
	}

	serializePacketType = PacketType::INVALID;
}

void Unit::deserialize(Packet& p)
{
	int packet_type;
	p >> packet_type;

	switch (packet_type)
	{
		case LOAD_JSON:
		{
			std::cout << "received LOAD_JSON\n";
			int unitID;
			p >> unitID;
			loadFromJSON(unitID);
			break;
		}
		case MOVE:
		{
			//TODO change the way this works
			std::cout << "received MOVE\n";
			float pos_x;
			float pos_y;
			p >> pos_x >> pos_y;

			move();
			break;
		}
		case SET_POSITION:
		{
			std::cout << "received SET_POSITION\n";
			float pos_x;
			float pos_y;
			p >> pos_x >> pos_y;

			setPosition(pos_x, pos_y);
		} break;
		case ATTACK:
		{
			std::cout << "received ATTACK\n";
			uint32_t defender;
			p >> defender;

			Entity* ent_defender = ent_man->getEntity(defender);
			Unit* unit_defender = static_cast<Unit*>(ent_defender);
			//causes infinite loop
			//doAttack(unit_defender);
			break;
		}
	}
}

void Unit::setFacing(UnitFacing new_facing)
{
	char_facing = new_facing;
}

Unit::UnitFacing Unit::getFacing() const
{
	return char_facing;
}

void Unit::setState(UnitState new_state)
{
	char_state = new_state;
}

Unit::UnitState Unit::getState() const
{
	return char_state;
}


void Unit::doAttack(Unit* enemy_target)
{
	//Arithmetic in here to make the Unit's facing in the general direction of the enemy.

	//BRENDON
	//how many time CHARACTERS will it take to shoot? These should be either constexpr in the constants class OR determined by the conditions in the level (foggy weather, rain etc)

	//here you can balance the attack against the enemy etc.
	if(time_units >= time_unit_attack_cost)
	{
		//RNG shit to roll d100 against the aim stat?
		time_units -= time_unit_attack_cost;
		enemy_target->getAttacked(this, weapon_damage);
	}
	else
	{
		std::cout << "Insufficient Time CHARACTERS. Attack cancelled";
	}
}

void Unit::getAttacked(Unit* attacker, float damage)
{
	//BRENDON - balancing
	//These should be either constexpr in the constants class OR determined by the conditions in the level(foggy weather, rain etc)

	//take the damage
	//Apply any damage reduction from armour or any of that shit here?
	health -= damage;

	if(reactions >= time_unit_reactive_cost
		&& !hasReactiveFired
		&& time_units >= time_unit_attack_cost)
	{
		hasReactiveFired = true;
		doAttack(attacker);
	}
}

void Unit::getReactiveAttacked(Unit* attacker, float damage)
{
	//TODO attacker->attackAnimation()

	//take the damage
	//Apply any damage reduction from armour or any of that shit here?
	health -= damage;
}

void Unit::endTurn()
{
	//TODO - low priority, trigger any visuals to notify user that unit has acted (dim overlay?)

}

void Unit::endRound()
{
	//TODO - low priority, re-init any temp value changes to default
	hasReactiveFired = false;
}

void Unit::setPosition(float x, float y)
{
	x_position = x;
	y_position = y;
	std::cout << "UNIT POS SET TO " << x << ", " << y << "\n";
}

// TODO make sure this is called AFTER positions list has been populated
void Unit::move()
{
	char_state = UnitState::WALKING;
	horizontal_walk_sprite.play();
	backward_walk_sprite.play();
	forward_walk_sprite.play();

	//SET TO PATH TO FOLLOW VECTOR POs 0
	target_x_position = movement_pos_list.front().x;
	target_y_position = movement_pos_list.front().y;
}

void Unit::setSelected(bool new_val)
{
	selected = new_val;
}

bool Unit::getSelected()
{
	return selected;
}

void Unit::setSerializedPacketType(PacketType new_type)
{
	serializePacketType = new_type;
}

std::string Unit::getFullName()
{
	std::string name = first_name + " '" + nick_name + "' " + last_name;
	return name;
}

void Unit::randomiseName()
{
	//Rng::getRandomInt(0, ListOfFirstNames.getLength())
}

void Unit::loadFromJSON(int unit_to_load)
{
	std::string id = "unit" + std::to_string(unit_to_load);
	std::ifstream file("../../Resources/unitStats.json");
	jsoncons::json unitStats;
	file >> unitStats;

	try
	{
		//STATS
		health = (float)(unitStats[id]["HP"].as_double());
		view_distance = (float)(unitStats[id]["viewDistance"].as_double());
		time_units = (float)(unitStats[id]["timeUnits"].as_double());
		stamina = (float)(unitStats[id]["stamina"].as_double());
		bravery = (float)(unitStats[id]["bravery"].as_double());
		reactions = (float)(unitStats[id]["reactions"].as_double());
		firing_accuracy = (float)(unitStats[id]["firing_accuracy"].as_double());
		strength = (float)(unitStats[id]["strength"].as_double());
		initiative = (float)(unitStats[id]["initiative"].as_double());
		base_move_speed = (float)(unitStats[id]["base_move_speed"].as_double());
		current_move_speed = base_move_speed;

		//SPRITES
		horizontal_walk_sprite.addFrame(unitStats[id]["walkLeft1"].as_string(), 0.25f, 0.0f, 0.0f);
		horizontal_walk_sprite.addFrame(unitStats[id]["idleLeft"].as_string(), 0.25f, 0.0f, 0.0f);
		horizontal_walk_sprite.addFrame(unitStats[id]["walkLeft2"].as_string(), 0.25f, 0.0f, 0.0f);
		horizontal_walk_sprite.addFrame(unitStats[id]["idleLeft"].as_string(), 0.25f, 0.0f, 0.0f);

		forward_walk_sprite.addFrame(unitStats[id]["walkForward1"].as_string(), 0.25f, 0.0f, 0.0f);
		forward_walk_sprite.addFrame(unitStats[id]["idleForward"].as_string(), 0.25f, 0.0f, 0.0f);
		forward_walk_sprite.addFrame(unitStats[id]["walkForward2"].as_string(), 0.25f, 0.0f, 0.0f);
		forward_walk_sprite.addFrame(unitStats[id]["idleForward"].as_string(), 0.25f, 0.0f, 0.0f);

		backward_walk_sprite.addFrame(unitStats[id]["idleBack"].as_string(), 0.25f, 0.0f, 0.0f);
		backward_walk_sprite.addFrame(unitStats[id]["walkBack1"].as_string(), 0.25f, 0.0f, 0.0f);
		backward_walk_sprite.addFrame(unitStats[id]["idleBack"].as_string(), 0.25f, 0.0f, 0.0f);
		backward_walk_sprite.addFrame(unitStats[id]["walkBack2"].as_string(), 0.25f, 0.0f, 0.0f);

		idle_sprite_forward->loadTexture((std::string("../../Resources/Textures/" + unitStats[id]["idleForward"].as_string() + ".png").c_str()));
		idle_sprite_back->loadTexture((std::string("../../Resources/Textures/" + unitStats[id]["idleBack"].as_string() + ".png").c_str()));
		idle_sprite_left->loadTexture((std::string("../../Resources/Textures/" + unitStats[id]["idleLeft"].as_string() + ".png").c_str()));

		initialized = true;
	}
	catch(std::runtime_error& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}
}

void Unit::commonUpdate(float dt)
{
	updateOverridePositions();

	switch (char_state)
	{
	case IDLE:
	{
		break;
	}

	case WALKING:
	{
		forward_walk_sprite.update(dt);
		backward_walk_sprite.update(dt);
		horizontal_walk_sprite.update(dt);

		bool xPosMatched = false;
		bool yPosMatched = false;

		if ((target_x_position - x_position) >= 0.1f)
		{
			x_position += base_move_speed * dt;
		}
		else if ((x_position - target_x_position) >= 0.1f)
		{
			x_position -= base_move_speed * dt;
		}
		else
		{
			xPosMatched = true;
		}

		if ((target_y_position - y_position) >= 0.1f)
		{
			y_position += base_move_speed * dt;
		}
		else if ((y_position - target_y_position) >= 0.1f)
		{
			y_position -= base_move_speed * dt;
		}
		else
		{
			yPosMatched = true;
		}

		if (yPosMatched && xPosMatched)
		{			
			
			if (movement_pos_list_counter >= movement_pos_list.size() - 1 )
			{
				movement_pos_list_counter = 0;
				movement_pos_list.clear();
				char_state = UnitState::IDLE;

				horizontal_walk_sprite.restart();
				horizontal_walk_sprite.pause();
				backward_walk_sprite.restart();
				backward_walk_sprite.pause();
				forward_walk_sprite.restart();
				forward_walk_sprite.pause();



			}
			else
			{
				movement_pos_list_counter++;
				target_x_position = movement_pos_list[movement_pos_list_counter].x;
				target_y_position = movement_pos_list[movement_pos_list_counter].y;
			}
		}

		break;
	}
	case SHOOTING:
	{
		break;
	}
	default:
	{
		break;
	}
	}
}

ASGE::Sprite* Unit::getCurrentSprite() const
{
	switch (char_state)
	{
		case IDLE:
		{
			//render idle sprite in current facing
			switch (char_facing)
			{
				case NORTH:
				{
					return idle_sprite_back.get();
				}
				case EAST:
				{
					if (!idle_sprite_left->isFlippedOnX())
					{
						idle_sprite_left->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);
					}

					return idle_sprite_left.get();
				}
				case SOUTH:
				{
					return idle_sprite_forward.get();
				}
				case WEST:
				{
					return idle_sprite_left.get();
				}
			}
		}
		case WALKING:
		{
			switch (char_facing)
			{
				case NORTH:
				{
					return backward_walk_sprite.getCurrentFrameSprite();
				}
				case EAST:
				{
					if (!horizontal_walk_sprite.getCurrentFrameSprite()->isFlippedOnX())
					{
						horizontal_walk_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);
					}

					return horizontal_walk_sprite.getCurrentFrameSprite();
				}
				case SOUTH:
				{
					return forward_walk_sprite.getCurrentFrameSprite();
				}
				case WEST:
				{
					if (horizontal_walk_sprite.getCurrentFrameSprite()->isFlippedOnX())
					{
						horizontal_walk_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::NORMAL);
					}

					return horizontal_walk_sprite.getCurrentFrameSprite();
				}
			}
		}
		case SHOOTING:
		{
			switch (char_facing)
				{
				case NORTH:
				{
					return nullptr;
				}
				case EAST:
				{
					return nullptr;
				}
				case SOUTH:
				{
					return nullptr;
				}
				case WEST:
				{
					return nullptr;
				}
				default:
				{
					return nullptr;
				}
			}
		}
		default:
		{
			return nullptr;
		}
	}
}

void Unit::updateOverridePositions()
{
	//update all sprites positions to the same values
	idle_sprite_forward->xPos(x_position);
	idle_sprite_forward->yPos(y_position);

	idle_sprite_back->xPos(x_position);
	idle_sprite_back->yPos(y_position);

	idle_sprite_left->xPos(x_position);
	idle_sprite_left->yPos(y_position);

	//update walk anim sprite positions
	forward_walk_sprite.xPos = x_position;
	backward_walk_sprite.xPos = x_position;
	horizontal_walk_sprite.xPos = x_position;

	forward_walk_sprite.yPos = y_position;
	backward_walk_sprite.yPos = y_position;
	horizontal_walk_sprite.yPos = y_position;
}

void Unit::setPathToGoal(std::vector<MoveData> path)
{
	for(int i = path.size() - 1; i >= 0; i--)
	{
		MoveData move_data;
		move_data.x = path[i].x;
		move_data.y = path[i].y;
		move_data.time_units = path[i].time_units;
		movement_pos_list.push_back(std::move(move_data));
	}
}
