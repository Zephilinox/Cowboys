#pragma once
#pragma once

//STD
#include <chrono>
#include <vector>
#include <algorithm>

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/Timer.hpp"
#include "../../Architecture/Messages/Message.hpp"
#include "../../Architecture/Managers/NetworkManager.hpp"
#include "../../Architecture/AnimatedSprite.hpp"
#include "../../Architecture/UI/Menu.hpp"

#include "../Unit.hpp"
#include "../Warband.hpp"
#include "../../Architecture/Managers/EntityManager.hpp"

#include "../ActionQueue.h"
#include "../../Architecture/Rng.hpp"
#include "../EnemyPanel.h"

//Map
#include "../Grid.h"

class GameData;

class StateGame : public State
{
public:
	StateGame(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID);
	~StateGame();

	void update(const ASGE::GameTime&) override final;
	void render() const override final;
	void onActive() override final;
	void onInactive() override final;

	//Fog of war
	void updateAllFogOfWar();
	void updateUnitFogOfWar(Unit* unit);
	void movingUnitLineOfSight();

	//Packet senders
	void sendEndTurnPacket();
	void sendAttackPacket(uint32_t attacker_ID, uint32_t defender_ID, bool isHit, bool isReactive, bool reactive_hit);

	//ActionQueue
	void nextAction();
	void endTurn();
	void endRound();

	//Utility
	bool attackAccuracyCheck(float unit_accuracy);
	void screenScroll(float dt, double mouseX, double mouseY);

private:
	ManagedConnection managed_slot_1;
	ManagedConnection managed_slot_2;

	//Display timers
	float endTurnTimer = 0.0f;
	float endRoundTimer = 0.0f;
	float yourTurnTimer = 0.0f;

	//Stats panels
	EnemyPanel enemy_panel;
	void renderUnitStatsToPanel() const;

	//Menu
	Menu menu;

	//ActionQueue
	ActionQueue action_queue;
	const float default_queue_timer = 0.5f;
	float queue_timer = 0.5f;

	//Warbands
	Warband our_warband;
	Warband their_warband;
	Warband* active_turn_warband;
	uint32_t active_turn_unit;

	//Grid/movement
	Grid game_grid;
	TerrainTile* selected_tile = nullptr;
	float move_cost;
	bool render_move_cost = false;

	//Utility
	Rng rand_no_generator;
	bool gameReady = false;
	EntityManager ent_man;
	float offset_x = 0.0f;
	float offset_y = 0.0f;
	void checkWinCondition();

	//Sprites
	std::unique_ptr<ASGE::Sprite> portrait_highlight;
	std::unique_ptr<ASGE::Sprite> your_turn_sprite;
	std::unique_ptr<ASGE::Sprite> end_turn_sprite;
	std::unique_ptr<ASGE::Sprite> end_round_sprite;
	std::unique_ptr<ASGE::Sprite> UI_panel_sprite;
};
