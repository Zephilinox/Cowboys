#pragma once

//SELF
#include "../../../Architecture/Entity.hpp"
#include "../../../Architecture/AnimatedSprite.hpp"

class Paddle : public Entity
{
public:
	Paddle(GameData* game_data);

	void update(float dt) final;
	void render(ASGE::Renderer* renderer) const final;

	void serialize(Packet& p) final;
	void deserialize(Packet& p) final;

	AnimatedSprite sprite;
};