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

#include "../Unit.h"
#include "../Warband.h"
#include "../../Architecture/Managers/EntityManager.hpp"

class GameData;

class GameState : public State
{
public:
	GameState(GameData* game_data, int unit1ID, int unit2ID, int unit3ID, int unit4ID, int unit5ID);
	~GameState();

	void update(const ASGE::GameTime&) override final;
	void render() const override final;
	void onActive() override final;
	void onInactive() override final;

private:
	ManagedConnection managed_slot_1;
	ManagedConnection managed_slot_2;

	Menu menu;

	Warband our_warband;
	Warband their_warband;

	EntityManager ent_man;
};
