#include "MyNetGame.hpp"

//LIB
#include <Engine\InputEvents.h>

//SELF
#include "States/StateMenu.hpp"
#include "../Architecture/Networking/Packet.hpp"
#include "../Architecture/Networking/Client.hpp"

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

	game_data->getInputManager()->addAction(hash("Enter"), ASGE::KEYS::KEY_ENTER);
	game_data->getInputManager()->addAction(hash("Enter"), game_data->getInputManager()->gamepad_button_enter);

	game_data->getInputManager()->addAction(hash("Escape"), ASGE::KEYS::KEY_ESCAPE);
	game_data->getInputManager()->addAction(hash("Escape"), game_data->getInputManager()->gamepad_button_escape);

	game_data->getInputManager()->addAction(hash("Up"), ASGE::KEYS::KEY_UP);
	game_data->getInputManager()->addAction(hash("Up"), ASGE::KEYS::KEY_W);
	game_data->getInputManager()->addAction(hash("Up"), game_data->getInputManager()->gamepad_button_up);

	game_data->getInputManager()->addAction(hash("Down"), ASGE::KEYS::KEY_DOWN);
	game_data->getInputManager()->addAction(hash("Down"), ASGE::KEYS::KEY_S);
	game_data->getInputManager()->addAction(hash("Down"), game_data->getInputManager()->gamepad_button_down);

	game_data->getInputManager()->addAction(hash("Right"), ASGE::KEYS::KEY_RIGHT);
	game_data->getInputManager()->addAction(hash("Right"), ASGE::KEYS::KEY_D);
	//game_data->getInputManager()->addAction("right", game_data->getInputManager()->gamepad_button_right;

	game_data->getInputManager()->addAction(hash("Left"), ASGE::KEYS::KEY_LEFT);
	game_data->getInputManager()->addAction(hash("Left"), ASGE::KEYS::KEY_A);
	//game_data->getInputManager()->addAction("left", game_data->getInputManager()->gamepad_button_left);

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

	return true;
}

void MyNetGame::update(const ASGE::GameTime& gt)
{
	if (capFPS && 10 - gt.delta_time.count() > 0)
	{
		std::this_thread::sleep_for(
			std::chrono::duration<double, std::milli>(10 - gt.delta_time.count())
		);
	}

	game_data->getMessageQueue()->processMessages(3ms);
	game_data->getNetworkManager()->update();
	game_data->getInputManager()->update();
	game_data->getStateManager()->update(gt);

	if (game_data->getInputManager()->isMouseButtonPressed(0) ||
		game_data->getInputManager()->isMouseButtonDown(1))
	{
		double xpos, ypos;
		game_data->getInputManager()->getMouseScreenPosition(xpos, ypos);
		std::cout << "Mouse Screen = (" << xpos << "), (" << ypos << ")\n";

		game_data->getInputManager()->getMouseWorldPosition(xpos, ypos);
		std::cout << "Mouse World = (" << xpos << "), (" << ypos << ")\n";
	}

	if (game_data->getInputManager()->isKeyPressed(ASGE::KEYS::KEY_F1))
	{
		if (renderer->getWindowMode() == ASGE::Renderer::WindowMode::WINDOWED)
		{
			renderer->setWindowedMode(ASGE::Renderer::WindowMode::BORDERLESS);
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
