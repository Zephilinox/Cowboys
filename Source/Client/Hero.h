#pragma once
#include <Engine\Sprite.h>
#include "..\Architecture\GameData.hpp"
#include "Unit.h"

class GameData;

class Hero : public Unit
{
public:

	Hero(GameData* game_data);
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