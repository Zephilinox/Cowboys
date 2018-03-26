#pragma once
#include <Engine\Sprite.h>
#include "..\Architecture\GameData.hpp"
#include "Unit.h"

class Hero : public Unit
{
public:
	enum CharacterFacing
	{
		NORTH = 0,
		EAST,
		SOUTH,
		WEST
	};

	enum CharacterState
	{
		IDLE = 0,
		WALKING,
		SHOOTING,
	};

	Hero(ASGE::Renderer* rend);
	~Hero() = default;


	void doAttack(Unit* enemy) override;
	void getAttacked(Unit* attacker, float weapon_damage) override;

	void update(float dt) override;
	void render(ASGE::Renderer* renderer) const override;

	//void useSpecialAbility();

private:

	//Great place here for any multipliers we want to add , e.g. movespeed multiplier unique to Hero class?
	float base_move_speed = 30.0f;
};