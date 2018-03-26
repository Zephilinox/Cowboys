#include "Hero.h"
#include <Engine\Renderer.h>
#include "Constants.hpp"


Hero::Hero(ASGE::Renderer* rend) : Unit::Unit(rend)
{
	//Wouldn't it be nice if we did something inside this constructor? Everything happens before we even get in here...
}


void Hero::doAttack(Unit* enemy_target)
{
	//Arithmetic in here to make the character's facing in the general direction of the enemy.

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

void Hero::getAttacked(Unit* attacker, float damage)
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

void Hero::update(float dt)
{
	//do everything parent update would do
	Unit::update(dt);

	//then any extra shit
}

void Hero::render(ASGE::Renderer* renderer) const
{
	//Render what parent wants to render
	Unit::render(renderer);

	//then any extra shit you wanna do
}

