#pragma once
#include <Engine\Sprite.h>
#include "Architecture\AnimatedSprite.hpp"
#include "Play.hpp"


class Character
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

	//Character(ASGE::Renderer* rend);
	Character();
	~Character();

	void setFacing(CharacterFacing new_facing);
	void setState(CharacterState new_state);

	float getXPosition() const { return x_position; }
	float getYPosition() const { return y_position; }

	void setIsActive(bool new_active);
	bool getIsActive() const;
	
	//void initCharacter(Play::SceneCharacters actor, ASGE::Renderer* rend);

	void setPosition(float x, float y);

	void slowMoveToPosition(float x, float y);
	void fastMoveToPosition(float x, float y);

	void update(float dt);
	//void render(ASGE::Renderer* renderer) const;

private:

	float move_speed = 10.0f;

	bool isActive = false;

	std::string charTextureStrings[70];

	void updateOverridePositions();

	float x_position = 0.0f;
	float y_position = 0.0f;
	float target_x_position = 0.0f;
	float target_y_position = 0.0f;

	AnimatedSprite horizontal_walk_sprite;
	AnimatedSprite forward_walk_sprite;
	AnimatedSprite backward_walk_sprite;
	std::unique_ptr<ASGE::Sprite> idle_sprite_forward;
	std::unique_ptr<ASGE::Sprite> idle_sprite_right;
	std::unique_ptr<ASGE::Sprite> idle_sprite_back;
	std::unique_ptr<ASGE::Sprite> idle_sprite_left;

	void loadCharacterTextureStrings();

	CharacterFacing char_facing = SOUTH;
	CharacterState char_state = IDLE;
};