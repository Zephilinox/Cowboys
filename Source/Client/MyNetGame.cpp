#include <Engine\InputEvents.h>

#include "MyNetGame.h"

namespace {
	const float BACKGROUND_LAYER = 0.0f;
	const float FOREGROUND_LAYER = 1.0f;
};

MyNetGame::MyNetGame()
{
}

MyNetGame::~MyNetGame()
{
	this->inputs->unregisterCallback(key_handler_id);
	//network.deinitialize();
}

bool MyNetGame::init()
{
	game_width = 1280;
	game_height = 720;

	if (!initAPI(ASGE::Renderer::WindowMode::WINDOWED))
	{
		return false;
	}

	renderer->setWindowTitle("My Network Game");
	const float dark_grey[3] = { 0.1f, 0.1f, 0.1f };
	renderer->setClearColour(dark_grey);
	renderer->setSpriteMode(ASGE::SpriteSortMode::BACK_TO_FRONT);
	toggleFPS();

	this->inputs->use_threads = false;

	key_handler_id = this->inputs->addCallbackFnc(
		ASGE::EventType::E_KEY, &MyNetGame::keyHandler, this);

	game_data = std::make_unique<GameData>(renderer.get(), inputs.get(), game_width, game_height);

	game_data->getInputManager()->addAction("enter", ASGE::KEYS::KEY_ENTER);
	game_data->getInputManager()->addAction("enter", game_data->getInputManager()->gamepad_button_enter);

	game_data->getInputManager()->addAction("escape", ASGE::KEYS::KEY_ESCAPE);
	game_data->getInputManager()->addAction("escape", game_data->getInputManager()->gamepad_button_escape);

	game_data->getInputManager()->addAction("up", ASGE::KEYS::KEY_UP);
	game_data->getInputManager()->addAction("up", ASGE::KEYS::KEY_W);
	game_data->getInputManager()->addAction("up", game_data->getInputManager()->gamepad_button_up);

	game_data->getInputManager()->addAction("down", ASGE::KEYS::KEY_DOWN);
	game_data->getInputManager()->addAction("down", ASGE::KEYS::KEY_S);
	game_data->getInputManager()->addAction("down", game_data->getInputManager()->gamepad_button_down);

	game_data->getFontManager()->addFont("../../Resources/Fonts/Zorque.ttf", "Default", 24);

	menu = std::make_unique<Menu>(game_data.get());
	menu->addButton(500, 300, "SERVER", ASGE::COLOURS::GREY, ASGE::COLOURS::WHITE);
	menu->addButton(500, 400, "CLIENT", ASGE::COLOURS::GREY, ASGE::COLOURS::WHITE);

	menu->getButton(0).on_click.connect([&]()
	{
		game_data->getNetworkManager()->initialize(true);
	});

	menu->getButton(1).on_click.connect([&]()
	{
		game_data->getNetworkManager()->initialize(false);
	});
		
	return true;
}

void MyNetGame::update(const ASGE::GameTime& ms)
{
	game_data->getInputManager()->update();
	menu->update();
}

void MyNetGame::render(const ASGE::GameTime& ms)
{
	if (game_data->getNetworkManager()->network)
	{
		if (game_data->getNetworkManager()->network->isConnected())
		{
			renderer->renderText("CONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}
		else
		{
			renderer->renderText("DISCONNECTED", 250, 100, ASGE::COLOURS::WHITE);
		}

		if (game_data->getNetworkManager()->network->isServer())
		{
			renderer->renderText("SERVER", 250, 50, ASGE::COLOURS::WHITE);
		}
		else
		{
			renderer->renderText("CLIENT", 250, 50, ASGE::COLOURS::WHITE);
		}
	}
	else
	{
		menu->render();
	}
}

void MyNetGame::keyHandler(const ASGE::SharedEventData data)
{
	const ASGE::KeyEvent* key_event =
		static_cast<const ASGE::KeyEvent*>(data.get());

	auto key = key_event->key;
	auto action = key_event->action;

	game_data->getInputManager()->handleInput(key, action);

	if (key == ASGE::KEYS::KEY_ESCAPE)
	{
		signalExit();
	}
}
