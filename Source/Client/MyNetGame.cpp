#include <Engine\InputEvents.h>

#include "MyNetGame.h"

namespace {
	const float BACKGROUND_LAYER = 0.0f;
	const float FOREGROUND_LAYER = 1.0f;
};

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
	renderer->setClearColour(ASGE::COLOURS::CORNFLOWERBLUE);
	renderer->setSpriteMode(ASGE::SpriteSortMode::BACK_TO_FRONT);
	toggleFPS();

	this->inputs->use_threads = true;
	
	key_handler_id = this->inputs->addCallbackFnc(
		ASGE::EventType::E_KEY, &MyNetGame::keyHandler, this);
		
	//network.initialize();
	//th = std::thread(&ClientComponent::consumeEvents, &network);
	//th.detach();

	return true;
}

void MyNetGame::update(const ASGE::GameTime& ms)
{

}

void MyNetGame::render(const ASGE::GameTime& ms)
{
	/*if (network.isConnected())
		renderer->renderText("CONNECTED", 250, 100, ASGE::COLOURS::WHITE);

	else if (network.isConnecting())
		renderer->renderText("CONNECTING", 250, 100, ASGE::COLOURS::WHITE);

	else
		renderer->renderText("DISCONNECTED", 250, 100, ASGE::COLOURS::WHITE);*/
}

void MyNetGame::keyHandler(const ASGE::SharedEventData data)
{
	const ASGE::KeyEvent* key_event =
		static_cast<const ASGE::KeyEvent*>(data.get());

	auto key = key_event->key;
	auto action = key_event->action;

	if (key == ASGE::KEYS::KEY_ESCAPE)
	{
		signalExit();
	}
}
