#pragma once
#include <Engine\Sprite.h>
#include "Architecture\AnimatedSprite.hpp"


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

	Character(ASGE::Renderer* rend);
	~Character();

	void setFacing(CharacterFacing new_facing);
	void setState(CharacterState new_state);

	float getXPosition() const { return x_position; } 
	float getYPosition() const { return y_position; }
	void setPosition(float x, float y);

	bool getIsActive() const;
	void setIsActive(bool new_active);

	void doAttack(Character* enemy);
	void getAttacked(Character* attacker, float weapon_damage);

	void turnEnded();

	//These may end up overridden
	virtual void slowMoveToPosition(float x, float y);
	virtual void fastMoveToPosition(float x, float y);

	void update(float dt);
    void render(ASGE::Renderer* renderer) const;

	float getViewDistance() const		{ return view_distance; }
	float getTimeUnits() const			{ return time_units; }
	float getStamina() const			{ return stamina; }
	float getHealth() const				{ return  health; }
	float getBravery() const			{ return bravery; }
	float getReactions() const			{ return reactions; }
	float getFiringAccuracy() const		{ return firing_accuracy; }
	float getStrength() const			{ return strength; }

	std::string getFullName();

	void setViewDistance(float new_val)		{ view_distance = new_val; }
	void setTimeUnits(float new_val)		{ time_units = new_val; }
	void setStamina(float new_val)			{ stamina = new_val; }
	void setHealth(float new_val)			{ health = new_val; }
	void setBravery(float new_val)			{ bravery = new_val; }
	void setReactions(float new_val)		{ reactions = new_val; }
	void setFiringAccuracy(float new_val)	{ firing_accuracy = new_val; }
	void setStrength(float new_val)			{ strength = new_val; }


private:
	//FACING
	CharacterFacing char_facing = SOUTH;

	//STATE
	CharacterState char_state = IDLE;

	//DISPLAY / UI
	std::string first_name;
	std::string last_name;
	std::string nick_name;

	//STATS
	float view_distance = 10.0f;
	float time_units = 10.0f;
	float stamina = 10.0f;
	float health = 10.0f;
	float bravery = 10.0f;
	float reactions = 10.0f;
	float firing_accuracy = 10.0f;
	float strength = 10.0f;
	bool hasReactiveFired = false;

	bool isActive = false;
	float base_move_speed = 30.0f;
	void updateOverridePositions();

	//POSITIONS
	float x_position = 0.0f;
	float y_position = 0.0f;
	float target_x_position = 0.0f;
	float target_y_position = 0.0f;

	//SPRITES
	AnimatedSprite horizontal_walk_sprite;
	AnimatedSprite forward_walk_sprite;
	AnimatedSprite backward_walk_sprite;

	AnimatedSprite horizontal_shoot_sprite;
	AnimatedSprite forward_shoot_sprite;
	AnimatedSprite backward_shoot_sprite;

	std::unique_ptr<ASGE::Sprite> idle_sprite_forward;
	std::unique_ptr<ASGE::Sprite> idle_sprite_right;
	std::unique_ptr<ASGE::Sprite> idle_sprite_back;
	std::unique_ptr<ASGE::Sprite> idle_sprite_left;
};