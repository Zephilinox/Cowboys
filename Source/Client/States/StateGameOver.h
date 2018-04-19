#pragma once

//STD
#include <string>

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/UI/Menu.hpp"

class GameData;

/**
*  See State for overriden functions
*/
class StateGameOver : public State
{
public:
	StateGameOver(GameData* game_data, bool isWinner);

	void update(const ASGE::GameTime&) override final;
	void render() const override final;
	void onActive() override final;
	void onInactive() override final;

private:
	bool hasWon = false;

	Menu menu;
	std::unique_ptr<ASGE::Sprite> dim_background;
	std::unique_ptr<ASGE::Sprite> win_splash;
	std::unique_ptr<ASGE::Sprite> lose_splash;

	ManagedConnection managed_con;
	std::string current_music_path;
};
