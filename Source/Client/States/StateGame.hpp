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

	void updateAllFogOfWar();
	void updateUnitFogOfWar(Unit* unit);


	void sendEndTurnPacket();
	void sendAttackPacket(uint32_t attacker_ID, uint32_t defender_ID);

	void screenScroll(float dt, double mouseX, double mouseY);

	void movingUnitLineOfSight();
	void endTurn();
	void endRound();

private:
	ManagedConnection managed_slot_1;
	ManagedConnection managed_slot_2;

	EntityManager ent_man;

	void renderUnitStatsToPanel() const;

	Menu menu;

	Warband our_warband;
	Warband their_warband;
	Warband* active_turn_warband;
	uint32_t active_turn_unit;

	Grid game_grid;
	bool gameReady = false;

	std::unique_ptr<ASGE::Sprite> portrait_highlight;
	std::unique_ptr<ASGE::Sprite> your_turn_sprite;
	std::unique_ptr<ASGE::Sprite> end_turn_sprite;
	std::unique_ptr<ASGE::Sprite> end_round_sprite;
	std::unique_ptr<ASGE::Sprite> UI_panel_sprite;

	float endTurnTimer = 0.0f;
	float endRoundTimer = 0.0f;
	float yourTurnTimer = 0.0f;

	float offset_x = 0.0f;
	float offset_y = 0.0f;
};
