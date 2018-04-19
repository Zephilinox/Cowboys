#pragma once

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/UI/Menu.hpp"
#include "../../Architecture/Dialogues/DialogueTree.hpp"
#include "../../Architecture/Timer.hpp"

#include "../Grid.h"

class GameData;

/**
*  See State for overriden functions
*/
class StateMenu : public State
{
public:
	StateMenu(GameData* game_data);
	
	void update(const ASGE::GameTime&) override final;
	void render() const override final;
	void onActive() override final;
	void onInactive() override final;

private:
	void randomiseCameraMovement();

	Menu menu;
	Grid grid;

	float dir_x = 0;
	float dir_y = 0;
	float speed = 0;
	float offset_x = 0;
	float offset_y = 0;

	Timer randomiseTimer;
	float delay;
	bool active = true;
};
