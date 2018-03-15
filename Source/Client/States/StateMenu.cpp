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
	menu.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2 - 40, "PLAY", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2, "EXIT", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(game_data->getWindowWidth() / 2 - 80, game_data->getWindowHeight() / 2 + 40, "PING PONG", ASGE::COLOURS::DIMGRAY, ASGE::COLOURS::ANTIQUEWHITE);

	//register actions to button clicks
	//button lives as long as menu, which lives as long as this menu state
	//so no need to keep track of Connection lifetime
	//Take at look at Signal.hpp for more info on how that works.
	menu.getButton(0).on_click.connect([game_data]()
	{
		/*//here we're using the fade out state to transition to another state
		game_data->getStateManager()->push<FadeOutState>([game)data]()
		{
			game_data->getStateManager()->push<GameState>();
		});*/
	});

	menu.getButton(1).on_click.connect([game_data]()
	{
		game_data->getStateManager()->pop();
	});

	menu.getButton(2).on_click.connect([game_data]()
	{
		game_data->getStateManager()->push<StateLobby>();
	});
}

void StateMenu::update(const ASGE::GameTime& /*unused*/)
{
	menu.update();
}

void StateMenu::render() const
{
	menu.render();
}

void StateMenu::onActive()
{
}

void StateMenu::onInactive()
{
}