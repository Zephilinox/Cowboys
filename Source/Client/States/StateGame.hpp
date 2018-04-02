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

	void screenScroll(float dt, double mouseX, double mouseY);

	void endTurn();
	void endRound();

private:
	ManagedConnection managed_slot_1;
	ManagedConnection managed_slot_2;

	Menu menu;

	Warband our_warband;
	Warband their_warband;
	Warband* active_turn_warband;
	uint32_t active_turn_unit;

	//TODO refactor as final grid
	Grid testGrid;

	bool gameReady = false;

	std::unique_ptr<ASGE::Sprite> yourTurnSprite;
	std::unique_ptr<ASGE::Sprite> endTurnSprite;
	std::unique_ptr<ASGE::Sprite> endRoundSprite;

	EntityManager ent_man;

	float offset_x = 0.0f;
	float offset_y = 0.0f;
};
