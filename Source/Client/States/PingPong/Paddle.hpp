#pragma once

//SELF
#include "../../../Architecture/Entity.hpp"
#include "../../../Architecture/AnimatedSprite.hpp"

class Paddle : public Entity
{
public:
	Paddle(GameData* game_data);

	void update(float dt) override final;
	void render(ASGE::Renderer* renderer) const override final;

	void serialize(Packet& p) override final;
	void deserialize(Packet& p) override final;

	AnimatedSprite sprite;
};