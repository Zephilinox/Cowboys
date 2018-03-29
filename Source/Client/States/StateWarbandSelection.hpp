#pragma once

//SELF
#include "../../Architecture/State.hpp"
#include "../UnitSelectPanel.hpp"
#include "../../Architecture/UI/Menu.hpp"

class GameData;

/**
*  See State for overriden functions
*/
class StateWarbandSelection : public State
{
public:
	StateWarbandSelection(GameData* game_data);

	void update(const ASGE::GameTime&) override final;
	void render() const override final;
	void onActive() override final;
	void onInactive() override final;

private:
	UnitSelectPanel panel1;
	UnitSelectPanel panel2;
	UnitSelectPanel panel3;
	UnitSelectPanel panel4;
	UnitSelectPanel panel5;

	float panel1_x_pos = 0.0f;
	float panel2_x_pos = 0.0f;
	float panel3_x_pos = 0.0f;
	float panel4_x_pos = 0.0f;
	float panel5_x_pos = 0.0f;

	void initMenu();

	Menu menu;
};
