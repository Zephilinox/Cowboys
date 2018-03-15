#pragma once

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/UI/Menu.hpp"
#include "../../Architecture/Dialogues/DialogueTree.hpp"

class GameData;

/**
*  See State for overriden functions
*/
class StateLobby : public State
{
public:
	StateLobby(GameData* game_data);

	void update(const ASGE::GameTime& /*unused*/) final;
	void render() const final;
	void onActive() final;
	void onInactive() final;

private:
	Menu menu;
	Menu lobby;

	bool ready = false;
};
