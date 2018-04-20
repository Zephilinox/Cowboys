#pragma once

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/UI/Menu.hpp"
#include "../../Architecture/Dialogues/DialogueTree.hpp"
#include "../UnitSelectPanel.hpp"

class GameData;

/**
*  See State for overriden functions
*/
class StateLobby : public State
{
public:
	StateLobby(GameData* game_data);

	void update(const ASGE::GameTime&) override final;
	void render() const override final;
	void onActive() override final;
	void onInactive() override final;

private:
	void keyHandler(const ASGE::SharedEventData data);

	Menu menu;
	Menu lobby;

	bool ready = false;
	bool other_ready = false;

	ManagedConnection mc1;
	ManagedConnection mc2;

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

	int callback;
	std::deque<std::string> chatlog;
	std::string input;
	bool capslockOn = false;
};
