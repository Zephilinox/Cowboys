#pragma once
#include <Engine\Sprite.h>
#include "..\Architecture\AnimatedSprite.hpp"
#include "..\Architecture\GameData.hpp"
#include "../Architecture/Entity.hpp"

//LIB
#include <jsoncons/json.hpp>

class Unit : public Entity
{
public:

	struct Positions
	{
		float x = 0;
		float y = 0;
	};

	enum UnitFacing
	{
		NORTH = 0,
		EAST,
		SOUTH,
		WEST
	};

	enum UnitState
	{
		IDLE = 0,
		WALKING,
		SHOOTING,
	};

	enum PacketType
	{
		INVALID,
		LOAD_JSON,
		MOVE,
		ATTACK,
		SET_POSITION
	};

	Unit(GameData* game_data, EntityManager* ent_man);
	virtual ~Unit() = default;

	void onSpawn() final;

	void update(float dt);
	void render(ASGE::Renderer* renderer) const;

	void serialize(Packet& p) override final;
	void deserialize(Packet& p) override final;

	void setFacing(UnitFacing new_facing);
	virtual UnitFacing getFacing() const;
	void setState(UnitState new_state);
	virtual UnitState getState() const;

	float getXPosition() const { return x_position; }
	float getYPosition() const { return y_position; }
	void setPosition(float x, float y);

	void doAttack(Unit* enemy);
	void getAttacked(Unit* attacker, float weapon_damage);
	void getReactiveAttacked(Unit* attacker, float damage);

	void endTurn();
	void endRound();

	void moveToPosition(float x, float y);

	float getViewDistance() const { return view_distance; }
	float getTimeUnits() const { return time_units; }
	float getStamina() const { return stamina; }
	float getHealth() const { return  health; }
	float getBravery() const { return bravery; }
	float getReactions() const { return reactions; }
	float getFiringAccuracy() const { return firing_accuracy; }
	float getStrength() const { return strength; }
	float getInitiative() const { return initiative; }

	std::string getFullName();

	void setViewDistance(float new_val) { view_distance = new_val; }
	void setTimeUnits(float new_val) { time_units = new_val; }
	void setStamina(float new_val) { stamina = new_val; }
	void setHealth(float new_val) { health = new_val; }
	void setBravery(float new_val) { bravery = new_val; }
	void setReactions(float new_val) { reactions = new_val; }
	void setFiringAccuracy(float new_val) { firing_accuracy = new_val; }
	void setStrength(float new_val) { strength = new_val; }
	void setInitiative(float new_val) { initiative = new_val; }

	bool getIsAlive() const { return isAlive; };

	void randomiseName();

	void loadFromJSON(int unit_to_load);

	void addPosToList (float x, float y);

protected:
	void commonUpdate(float dt);
	ASGE::Sprite* getCurrentSprite() const;

	//DISPLAY / UI
	std::string first_name;
	std::string last_name;
	std::string nick_name;

	//FACING
	UnitFacing char_facing = SOUTH;

	//STATE
	UnitState char_state = IDLE;

	//STATS
	float view_distance = 10.0f;
	float time_units = 10.0f;
	float stamina = 10.0f;
	float health = 10.0f;
	float bravery = 10.0f;
	float reactions = 10.0f;
	float firing_accuracy = 10.0f;
	float strength = 10.0f;
	float initiative = 10.0f;
	bool hasReactiveFired = false;

	bool isAlive = true;
	float base_move_speed = 30.0f;
	float current_move_speed = 0.0f;
	void updateOverridePositions();

	//ATTACKING
	float weapon_damage = 5.0f;
	float time_unit_attack_cost = 10.0f;
	float time_unit_reactive_cost = 5.0f;

	//POSITIONS
	float x_position = 0.0f;
	float y_position = 0.0f;
	float target_x_position = 0.0f;
	float target_y_position = 0.0f;

	//SPRITES
	AnimatedSprite horizontal_walk_sprite;
	AnimatedSprite forward_walk_sprite;
	AnimatedSprite backward_walk_sprite;

	//Can render for a few frames when shooting, placement will be determined by direction from unit to enemy unit.
	std::unique_ptr<ASGE::Sprite> shoot_sprite;

	std::unique_ptr<ASGE::Sprite> idle_sprite_forward;
	std::unique_ptr<ASGE::Sprite> idle_sprite_back;
	std::unique_ptr<ASGE::Sprite> idle_sprite_left;

	std::unique_ptr<ASGE::Sprite> selected_sprite;

	bool initialized = false;
	PacketType serializePacketType = PacketType::INVALID;

	bool selected;

	std::vector<Positions> movement_pos_list;
	int movement_pos_list_counter = 0;

};
