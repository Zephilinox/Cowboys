#include <Engine\InputEvents.h>

#include "MyNetGame.h"

#include "States/StateMenu.hpp"

namespace {
	const float BACKGROUND_LAYER = 0.0f;
	const float FOREGROUND_LAYER = 1.0f;
};

MyNetGame::~MyNetGame()
{
	this->inputs->unregisterCallback(key_handler_id);
}

bool MyNetGame::init()
{
	game_width = 1280;
	game_height = 720;

	if (!initAPI(ASGE::Renderer::WindowMode::WINDOWED))
	{
		return false;
	}

	renderer->setWindowTitle("The Game With Fun Hats");
	const float dark_grey[] = { 0.08f, 0.08f, 0.08f };
	renderer->setClearColour(dark_grey);
	renderer->setSpriteMode(ASGE::SpriteSortMode::FRONT_TO_BACK);
	toggleFPS();

	this->inputs->use_threads = true;

	//todo: do this in input manager directly
	key_handler_id = this->inputs->addCallbackFnc(ASGE::EventType::E_KEY, &MyNetGame::keyHandler, this);

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

	game_data->getStateManager()->push<StateMenu>();

	game_data->getMessageQueue()->addListener([](Message* msg)
	{
		std::cout << "Processed\t" << msg->id << "\n";
	});

	game_data->getMessageQueue()->addListener([](Message* msg)
	{
		if (msg->id == FunctionMessage::ID)
		{
			std::cout << "Executed\t" << msg->id << "\n";
			FunctionMessage* func = static_cast<FunctionMessage*>(msg);
			func->execute();
		}
	});
	
	game_data->getNetworkManager()->packet_received.connect([&](Packet p)
	{
		if (p.getID() == hash("Message"))
		{
			std::string msg;
			p >> msg;
			std::cout << msg << " from client " << p.senderID << "\n";
		}
	});

	return true;
}

void MyNetGame::update(const ASGE::GameTime& gt)
{
	if (capFPS && 5 - gt.delta_time.count() > 0)
	{
		std::this_thread::sleep_for(
			std::chrono::duration<double, std::milli>(5 - gt.delta_time.count())
		);
	}

	game_data->getMessageQueue()->processMessages(3ms);
	game_data->getNetworkManager()->update();
	game_data->getInputManager()->update();
	game_data->getStateManager()->update(gt);

	if (game_data->getInputManager()->isKeyPressed(ASGE::KEYS::KEY_F1))
	{
		if (renderer->getWindowMode() == ASGE::Renderer::WindowMode::WINDOWED)
		{
			renderer->setWindowedMode(ASGE::Renderer::WindowMode::FULLSCREEN);
		}
		else
		{
			renderer->setWindowedMode(ASGE::Renderer::WindowMode::WINDOWED);
		}
	}

	if (game_data->getInputManager()->isKeyPressed(ASGE::KEYS::KEY_F2))
	{
		toggleFPS();
	}

	if (game_data->getInputManager()->isKeyPressed(ASGE::KEYS::KEY_F3))
	{
		capFPS = !capFPS;
		std::cout << "capFPS " << std::boolalpha << capFPS << "\n";
	}

	if (networkHello.getElapsedTime() > 2 && game_data->getNetworkManager()->network)
	{
		networkHello.restart();
		auto network = game_data->getNetworkManager()->network.get();

		if (network->isConnected())
		{
			Packet p;
			p.setID(hash("Message"));
			p << "Hello";
			network->sendPacket(0, &p);
		}
	}

	if (renderer->exit() || this->exit || game_data->exit || game_data->getStateManager()->empty())
	{
		signalExit();
	}
}

void MyNetGame::render(const ASGE::GameTime& gt)
{
	game_data->getStateManager()->render();
}

void MyNetGame::keyHandler(const ASGE::SharedEventData data)
{
	const ASGE::KeyEvent* key_event =
		static_cast<const ASGE::KeyEvent*>(data.get());

	auto key = key_event->key;
	auto action = key_event->action;

	game_data->getInputManager()->handleInput(key, action);
}
