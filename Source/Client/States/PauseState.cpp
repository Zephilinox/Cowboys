#include "PauseState.h"

//SELF
#include "../../Architecture/State.hpp"
#include "../../Architecture/Timer.hpp"
#include "../../Architecture/Constants.hpp"

PauseState::PauseState(GameData* game_data, std::string current_music_path)
	: State(game_data, true)
	, menu(game_data)
	, dim_background(game_data->getRenderer()->createRawSprite())
	, pause_image(game_data->getRenderer()->createRawSprite())
	, current_music_path(current_music_path)
{
	if(!dim_background->loadTexture("../../Resources/Textures/BlackScreen.png"))
	{
		throw "Failed to load BlackScreen.png";
	}

	if(!pause_image->loadTexture("../../Resources/Textures/UI/Pause.png"))
	{
		throw "Failed to load Pause.png";
	}

	const float c[3] = { 0.817f, 0.668f, 0.515f };
	menu.addButton(game_data->getWindowWidth() / 2.0f - 40.0f, game_data->getWindowHeight() / 2.0f - 60.0f, "CONTINUE", c, ASGE::COLOURS::ANTIQUEWHITE);
	//menu.addButton(game_data->getWindowWidth() / 2 - 33, game_data->getWindowHeight() / 2 - 20, "RESTART", c, ASGE::COLOURS::ANTIQUEWHITE);
	//menu.addButton(game_data->getWindowWidth() / 2 - 60, game_data->getWindowHeight() / 2 + 20, "TOGGLE AUDIO", c, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 90.0f, game_data->getWindowHeight() / 2.0f + 60.0f, "TOGGLE FULLSCREEN", c, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 20.0f, game_data->getWindowHeight() / 2.0f + 100.0f, "EXIT", c, ASGE::COLOURS::ANTIQUEWHITE);

	menu.getButton(0).on_click.connect([gd = game_data]()
	{
		gd->getStateManager()->pop();
	});

	menu.getButton(1).on_click.connect([game_data]()
	{
		game_data->getStateManager()->pop();
		//game_data->getStateManager()->push<GameState>();
	});

	//menu.getButton(2).on_click.connect([this, game_data]()
	//{
	//	if(game_data->getAudioManager()->getEngineType() == AudioManager::Engine::None)
	//	{
	//		game_data->getAudioManager()->setEngineType(AudioManager::Engine::IrrKlang);
	//		game_data->getAudioManager()->play(this->current_music_path, true);
	//	}
	//	else
	//	{
	//		game_data->getAudioManager()->setEngineType(AudioManager::Engine::None);
	//	}
	//});

	menu.getButton(2).on_click.connect([game_data]()
	{
		if(game_data->getRenderer()->getWindowMode() == ASGE::Renderer::WindowMode::WINDOWED)
		{
			game_data->getRenderer()->setWindowedMode(ASGE::Renderer::WindowMode::FULLSCREEN);
		}
		else
		{
			game_data->getRenderer()->setWindowedMode(ASGE::Renderer::WindowMode::WINDOWED);
		}
	});

	menu.getButton(3).on_click.connect([game_data]()
	{
		//TODO notify other user of 2nd player's exit.
		game_data->exit = true;
	});
}

void PauseState::update(const ASGE::GameTime &)
{
	menu.update();

	if(game_data->getInputManager()->isActionPressed("escape"))
	{
		game_data->getStateManager()->pop();
	}
}

void PauseState::render() const
{
	menu.render(Z_ORDER_LAYER::OVERLAY_TEXT);
	game_data->getRenderer()->renderSprite(*dim_background, Z_ORDER_LAYER::OVERLAY);
	game_data->getRenderer()->renderSprite(*pause_image, Z_ORDER_LAYER::OVERLAY + 100);

}

void PauseState::onActive()
{
}

void PauseState::onInactive()
{
}
