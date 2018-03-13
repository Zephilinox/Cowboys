#pragma once

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/UI/Menu.hpp"
#include "../../Architecture/Dialogues/DialogueTree.hpp"

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
	Menu menu;
};
