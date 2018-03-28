#include "Unit.h"
#include <Engine\Renderer.h>
#include "../Architecture/Rng.h"
#include "../Architecture/Constants.hpp"


Unit::Unit(GameData* game_data) :
	Entity(game_data),
	horizontal_walk_sprite(game_data->getRenderer(), true),
	forward_walk_sprite(game_data->getRenderer(), true),
	backward_walk_sprite(game_data->getRenderer(), true)
{
	//Wouldn't it be nice if we did something inside this constructor? Everything happens before we even get in here...
	current_move_speed = base_move_speed;

	//TODO override base stats with ones read in from JSON or unit selection?
	entity_info.type = hash("Unit");

	idle_sprite_forward = game_data->getRenderer()->createUniqueSprite();
	idle_sprite_back = game_data->getRenderer()->createUniqueSprite();
	idle_sprite_left = game_data->getRenderer()->createUniqueSprite();
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
		enemy_target->getAttacked(this, weapon_damage);
		time_units -= time_unit_attack_cost;
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
		doAttack(attacker);
		hasReactiveFired = true;
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
}

void Unit::moveToPosition(float x, float y)
{
	char_state = UnitState::WALKING;
	horizontal_walk_sprite.play();
	backward_walk_sprite.play();
	forward_walk_sprite.play();
	target_x_position = x;
	target_y_position = y;
}


void Unit::update(float dt)
{
	if (!initialized)
	{
		return;
	}

	updateOverridePositions();

	switch(char_state)
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

		if((target_x_position - x_position) >= 0.1f)
		{
			x_position += base_move_speed * dt;
		}
		else if((x_position - target_x_position) >= 0.1f)
		{
			x_position -= base_move_speed * dt;
		}
		else
		{
			xPosMatched = true;
		}

		if((target_y_position - y_position) >= 0.1f)
		{
			y_position += base_move_speed * dt;
		}
		else if((y_position - target_y_position) >= 0.1f)
		{
			y_position -= base_move_speed * dt;
		}
		else
		{
			yPosMatched = true;
		}

		if(yPosMatched && xPosMatched)
		{
			char_state = UnitState::IDLE;

			horizontal_walk_sprite.restart();
			horizontal_walk_sprite.pause();
			backward_walk_sprite.restart();
			backward_walk_sprite.pause();
			forward_walk_sprite.restart();
			forward_walk_sprite.pause();
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

void Unit::render(ASGE::Renderer* renderer) const
{
	if (!initialized)
	{
		return;
	}

	switch(char_state)
	{
	case IDLE:
	{
		//render idle sprite in current facing
		switch(char_facing)
		{
		case NORTH:
		{
			renderer->renderSprite(*idle_sprite_back, Z_ORDER_LAYER::UNITS + this->y_position);
			break;
		}
		case EAST:
		{
			if(!idle_sprite_left->isFlippedOnX())
			{
				horizontal_walk_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);
			}
			renderer->renderSprite(*idle_sprite_left, Z_ORDER_LAYER::UNITS + this->y_position);
			break;
		}
		case SOUTH:
		{
			renderer->renderSprite(*idle_sprite_forward, Z_ORDER_LAYER::UNITS + this->y_position);
			break;
		}
		case WEST:
		{
			renderer->renderSprite(*idle_sprite_left, Z_ORDER_LAYER::UNITS + this->y_position);
			break;
		}
		}
		break;
	}

	case WALKING:
	{
		switch(char_facing)
		{
		case NORTH:
		{
			renderer->renderSprite(*backward_walk_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::UNITS + this->y_position);
			break;
		}
		case EAST:
		{
			if(!horizontal_walk_sprite.getCurrentFrameSprite()->isFlippedOnX())
			{
				horizontal_walk_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);
			}
			renderer->renderSprite(*horizontal_walk_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::UNITS + this->y_position);
			break;
		}
		case SOUTH:
		{
			renderer->renderSprite(*forward_walk_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::UNITS + this->y_position);
			break;
		}
		case WEST:
		{
			if(horizontal_walk_sprite.getCurrentFrameSprite()->isFlippedOnX())
			{
				horizontal_walk_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::NORMAL);
			}
			renderer->renderSprite(*horizontal_walk_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::UNITS + this->y_position);
			break;
		}
		}
		break;
	}
	case SHOOTING:
	{
		switch(char_facing)
		{
		case NORTH:
		{
			
			break;
		}
		case EAST:
		{
			break;
		}
		case SOUTH:
		{
			break;
		}
		case WEST:
		{
			break;
		}
		default:
		{
			break;
		}
		}
	}
	default:
	{
		break;
	}
	}
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
		health = (unitStats[id]["HP"].as_double());
		view_distance = (unitStats[id]["viewDistance"].as_double());
		time_units = (unitStats[id]["timeUnits"].as_double());
		stamina = (unitStats[id]["stamina"].as_double());
		bravery = (unitStats[id]["bravery"].as_double());
		reactions = (unitStats[id]["reactions"].as_double());
		firing_accuracy = (unitStats[id]["firing_accuracy"].as_double());
		strength = (unitStats[id]["strength"].as_double());
		initiative = (unitStats[id]["initiative"].as_double());
		base_move_speed = (unitStats[id]["base_move_speed"].as_double());

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

void Unit::serialize(Packet& p)
{
	p << x_position << y_position;
}

void Unit::deserialize(Packet & p)
{
	int packet_type;
	p >> packet_type;

	switch(packet_type)
	{
		case LOAD_JSON:
		{
			int unitID;
			p >> unitID;
			loadFromJSON(unitID);
			break;
		}
		case MOVE:
		{
			float pos_x;
			float pos_y;
			p >> pos_x >> pos_y;

			moveToPosition(pos_x, pos_y);
			break;
		}
		case ATTACK:
		{
			uint32_t defender;
			p >> defender;

			//todo: need access to the game state for get entity
			//Entity* ent_defender = getEntity(defender);
			//Unit* unit_defender = static_cast<Unit*>(ent_defender);
			//doAttack(unit_defender);
			break;
		}
	}
}