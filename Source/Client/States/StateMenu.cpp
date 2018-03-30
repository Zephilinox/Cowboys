#include "StateMenu.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../../Architecture/Constants.hpp"

#include "StateLobby.hpp"

StateMenu::StateMenu(GameData* game_data)
	: State(game_data)
	, menu(game_data)
{
	//menu buttons
	//this is rough and just for prototyping, might need something nicer for the actual game
	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f - 40.0f, "PLAY", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f, "EXIT", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);
	
	menu.getButton(0).on_click.connect([game_data]()
	{
		game_data->getStateManager()->push<StateLobby>();
	});

	menu.getButton(1).on_click.connect([game_data]()
	{
		game_data->getStateManager()->pop();
	});

	game_data->getMusicPlayer()->play("FF7");
}

void StateMenu::update(const ASGE::GameTime&)
{
	menu.update();
}

void StateMenu::render() const
{
	menu.render();
}

void StateMenu::onActive()
{
	game_data->getMusicPlayer()->play("FF7");
}

void StateMenu::onInactive()
{
}