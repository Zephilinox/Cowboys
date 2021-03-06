#pragma once
#include <Engine\Sprite.h>
#include "..\Architecture\AnimatedSprite.hpp"
#include "..\Architecture\GameData.hpp"
#include "../Architecture/Entity.hpp"
#include "../Architecture/Constants.hpp"
//LIB
#include <jsoncons/json.hpp>

class TerrainTile;
class Grid;

class Unit : public Entity
{
public:

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
		DEAD
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
	void setCurrentTile(TerrainTile* new_tile);
	TerrainTile* getCurrentTile() { return current_tile; }

	void setGrid(Grid& grid) { game_grid = &grid; }

	void update(float dt);
	void render(ASGE::Renderer* renderer) const;

	void serialize(Packet& p) override final;
	void deserialize(Packet& p) override final;

	void setFacing(UnitFacing new_facing);
	virtual UnitFacing getFacing() const;
	void setState(UnitState new_state);
	virtual UnitState getState() const;

	bool getIsMoving();

	float getXPosition() const { return x_position; }
	float getYPosition() const { return y_position; }
	void setPosition(float x, float y);

	void doAttack(Unit* enemy);
	void reactiveAttack(Unit* enemy);
	void getAttacked(Unit* attacker, float weapon_damage);
	void getReactiveAttacked(Unit* attacker, float damage);

	void endTurn();
	void endRound();


	void setActiveTurn(bool new_val) { active_turn = new_val; }
	bool getActiveTurn() { return active_turn; }
	void move();
	void setSelected(bool new_val);
	bool getSelected();
	void setSerializedPacketType(PacketType new_type);

	float getViewDistance() const { return view_distance; }
	int getTimeUnits() const { return time_units; }
	float getStamina() const { return stamina; }
	float getHealth() const { return  health; }
	float getBravery() const { return bravery; }
	float getReactions() const { return reactions; }
	float getFiringAccuracy() const { return firing_accuracy; }
	float getStrength() const { return strength; }
	float getInitiative() const { return initiative; }
	ASGE::Sprite* getCurrentSprite() const;
	ASGE::Sprite* getPortraitSprite();

	std::string getPortraitSource();

	std::string getFullName();

	bool getInitialised() { return initialized; }

	int getWeaponDamage() { return weapon_damage; }

	void setViewDistance(float new_val) { view_distance = new_val; }
	void setTimeUnits(int new_val) { time_units = new_val; }
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


	void setPathToGoal(std::vector<MoveData> path);

protected:
	void commonUpdate(float dt);

	//DISPLAY / UI
	std::string first_name;
	std::string last_name;
	std::string nick_name;

	//FACING
	UnitFacing char_facing = SOUTH;

	//STATE
	UnitState char_state = IDLE;

	//STATS
	float view_distance = 0.0f;
	int start_time_units = 0;
	int time_units = 0;
	float stamina = 0.0f;
	float health = 0.0f;
	float bravery = 0.0f;
	float reactions = 0.0f;
	float firing_accuracy = 0.0f;
	float strength = 0.0f;
	float initiative = 0.0f;
	bool hasReactiveFired = false;

	bool isAlive = true;
	float base_move_speed = 0.0f;
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
	std::unique_ptr<ASGE::Sprite> dead;

	std::unique_ptr<ASGE::Sprite> selected_sprite;
	std::unique_ptr<ASGE::Sprite> marker_sprite;
	std::unique_ptr<ASGE::Sprite> portrait;

	bool initialized = false;
	PacketType serializePacketType = PacketType::INVALID;

	bool selected = false;
	bool active_turn = false;

	void endMove();

	std::string portrait_source;

	std::vector<MoveData> movement_pos_list;
	int movement_pos_list_counter = 0;

	Grid* game_grid = nullptr;
	TerrainTile* current_tile = nullptr;

};
