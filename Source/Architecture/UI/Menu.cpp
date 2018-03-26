#include "Menu.hpp"

//SELF
#include "../GameData.hpp"
#include "../Managers/InputManager.hpp"
#include "../Constants.hpp"
#include "../Audio/AudioLocator.hpp"

Menu::Menu(GameData* game_data, bool vertical)
	: game_data(game_data)
{
	assert(game_data);
	gamepad_buttons.reserve(4);
	selected_button_id = 0;
}

void Menu::update()
{
	if (gamepad_buttons.empty()) return;

	if (game_data->getInputManager()->isActionPressed("down"))
	{
		if (selected_button_id == gamepad_buttons.size() - 1)
		{
			selectButton(0);
		}
		else
		{
			selectButton(selected_button_id + 1);
		}
	}
	
	if (game_data->getInputManager()->isActionPressed("up"))
	{
		if (selected_button_id == 0)
		{
			selectButton(gamepad_buttons.size() - 1);
		}
		else
		{
			selectButton(selected_button_id - 1);
		}
	}

	if (game_data->getInputManager()->isActionPressed("enter"))
	{
		AudioLocator::get()->play("button_click.wav");

		if (gamepad_buttons.size())
		{
			gamepad_buttons[selected_button_id].on_click.emit();
		}
	}
}

void Menu::render(int z_order) const
{
	for (const Button& b : gamepad_buttons)
	{
		b.render(game_data, z_order);

		if (b.isSelected() && selection_image)
		{
			selection_image->xPos(b.getPosX() - 8 - selection_image->width());
			selection_image->yPos(b.getPosY() + 2 - selection_image->height());
			game_data->getRenderer()->renderSprite(*selection_image, z_order);
		}
	}
}

void Menu::reset()
{
	gamepad_buttons.clear();
	selected_button_id = 0;
}

int Menu::addButton(int x, int y, std::string name, ASGE::Colour colour, ASGE::Colour selected_colour)
{
	Button b;
	b.setPos(x, y);
	b.setName(name);
	b.setColour(colour);
	b.setSelectedColour(selected_colour);
	gamepad_buttons.push_back(b);

	if (gamepad_buttons.size() == 1)
	{
		gamepad_buttons[selected_button_id].setSelected(true);
	}

	return gamepad_buttons.size() - 1;
}

Button& Menu::getButton(int button_id)
{
	return gamepad_buttons.at(button_id);
}

void Menu::selectButton(int button_id)
{
	gamepad_buttons[selected_button_id].setSelected(false);
	selected_button_id = button_id;
	gamepad_buttons[selected_button_id].setSelected(true);

	AudioLocator::get()->play("button_select.wav");
}
