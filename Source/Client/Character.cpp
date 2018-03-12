#include "Character.h"
#include <Engine\Renderer.h>
#include "..\Common\Constants.hpp"


Character::Character(ASGE::Renderer* rend) :
	horizontal_walk_sprite(rend, true),
	forward_walk_sprite(rend, true),
	backward_walk_sprite(rend, true),

	horizontal_shoot_sprite(rend, true),
	forward_shoot_sprite(rend, true),
	backward_shoot_sprite(rend, true)
{
	//Wouldn't it be nice if we did something inside this constructor? Everything happens before we even get in here...
}

Character::~Character()
{

}

void Character::setFacing(CharacterFacing new_facing)
{
	char_facing = new_facing;
}

void Character::setState(CharacterState new_state)
{
	char_state = new_state;
}

void Character::setIsActive(bool new_active)
{
	isActive = new_active;
}

void Character::doAttack(Character* enemy_target)
{
	//BRENDON
	//how many time units will it take to shoot? These should be either constexpr in the constants class OR determined by the conditions in the level (foggy weather, rain etc)
	float shooting_threshold = 5.0f;
	float weapon_damage = 5.0f;

	//here you can balance the attack against the enemy etc.
	if(time_units >= shooting_threshold)
	{
		//RNG shit to roll d100 against the aim stat?
		enemy_target->getAttacked(this, weapon_damage);
	}
	else
	{
		std::cout << "Insufficient Time Units. Attack cancelled";
	}
}

void Character::getAttacked(Character* attacker, float damage)
{
	//BRENDON - balancing
	//These should be either constexpr in the constants class OR determined by the conditions in the level(foggy weather, rain etc)
	float reactions_threshold = 5.0f;
	float shooting_threshold = 5.0f;

	//take the damage
	//Apply any damage reduction from armour or any of that shit here?
	health -= damage;

	if(reactions >= reactions_threshold 
		&& !hasReactiveFired
		&& time_units >= shooting_threshold)
	{
		doAttack(attacker);
		hasReactiveFired = true;
	}
}

void Character::turnEnded()
{
	hasReactiveFired = false;
}

bool Character::getIsActive() const
{
	return isActive;
}

void Character::setPosition(float x, float y)
{
	x_position = x;
	y_position = y;
}

void Character::slowMoveToPosition(float x, float y)
{
	char_state = CharacterState::WALKING;
	horizontal_walk_sprite.play();
	backward_walk_sprite.play();
	forward_walk_sprite.play();
	base_move_speed = 35.0f;
	target_x_position = x;
	target_y_position = y;
}

void Character::fastMoveToPosition(float x, float y)
{
	char_state = CharacterState::WALKING;
	horizontal_walk_sprite.play();
	backward_walk_sprite.play();
	forward_walk_sprite.play();
	base_move_speed = 50.0f;
	target_x_position = x;
	target_y_position = y;
}


void Character::update(float dt)
{
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
				char_state = CharacterState::IDLE;

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
		horizontal_shoot_sprite.update(dt);
		forward_shoot_sprite.update(dt);
		backward_shoot_sprite.update(dt);
		break;
	}
	default:
		{
			break;
		}
	}
}

void Character::render(ASGE::Renderer* renderer) const
{
	switch(char_state)
	{
			case IDLE:
			{
				//render idle sprite in current facing
				switch(char_facing)
				{
					case NORTH:
					{
						renderer->renderSprite(*idle_sprite_back, Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case EAST:
					{
						renderer->renderSprite(*idle_sprite_right, Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case SOUTH:
					{
						renderer->renderSprite(*idle_sprite_forward, Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case WEST:
					{
						renderer->renderSprite(*idle_sprite_left, Z_ORDER_LAYER::CHARACTERS + this->y_position);
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
						renderer->renderSprite(*backward_walk_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case EAST:
					{
						if(horizontal_walk_sprite.getCurrentFrameSprite()->isFlippedOnX())
						{
							horizontal_walk_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::NORMAL);
						}
						renderer->renderSprite(*horizontal_walk_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case SOUTH:
					{
						renderer->renderSprite(*forward_walk_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case WEST:
					{
						if(!horizontal_walk_sprite.getCurrentFrameSprite()->isFlippedOnX())
						{
							horizontal_walk_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);
						}
						renderer->renderSprite(*horizontal_walk_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::CHARACTERS + this->y_position);
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
						renderer->renderSprite(*backward_shoot_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case EAST:
					{
						if(horizontal_shoot_sprite.getCurrentFrameSprite()->isFlippedOnX())
						{
							horizontal_shoot_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::NORMAL);
						}
						renderer->renderSprite(*horizontal_shoot_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case SOUTH:
					{
						renderer->renderSprite(*forward_shoot_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::CHARACTERS + this->y_position);
						break;
					}
					case WEST:
					{
						if(!horizontal_shoot_sprite.getCurrentFrameSprite()->isFlippedOnX())
						{
							horizontal_shoot_sprite.getCurrentFrameSprite()->setFlipFlags(ASGE::Sprite::FlipFlags::FLIP_X);
						}
						renderer->renderSprite(*horizontal_shoot_sprite.getCurrentFrameSprite(), Z_ORDER_LAYER::CHARACTERS + this->y_position);
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
std::string Character::getFullName()
{
	std::string name = first_name + " '" + nick_name + "' " + last_name;
	return name;
}

void Character::updateOverridePositions()
{
	//update all sprites positions to the same values
	idle_sprite_forward->xPos(x_position);
	idle_sprite_forward->yPos(y_position);

	idle_sprite_right->xPos(x_position);
	idle_sprite_right->yPos(y_position);

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

	//update walk anim sprite positions
	forward_shoot_sprite.xPos = x_position;
	backward_shoot_sprite.xPos = x_position;
	horizontal_shoot_sprite.xPos = x_position;

	forward_shoot_sprite.yPos = y_position;
	backward_shoot_sprite.yPos = y_position;
	horizontal_shoot_sprite.yPos = y_position;

}
