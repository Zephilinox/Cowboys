#include "Menu.hpp"

//STD
#include <iostream>

//SELF
#include "../GameData.hpp"
#include "../Managers/InputManager.hpp"
#include "../Constants.hpp"
#include "../Audio/AudioLocator.hpp"

Menu::Menu(GameData* game_data, bool vertical)
	: game_data(game_data)
{
	assert(game_data);
	buttons.reserve(4);
	selected_button_id = 0;

	if (vertical)
	{
		next_selection_action = hash("Down");
		prev_selection_action = hash("Up");
	}
	else
	{
		next_selection_action = hash("Right");
		prev_selection_action = hash("Left");
	}
}

void Menu::update()
{
	if (buttons.empty()) return;

	if (game_data->getInputManager()->isActionPressed(next_selection_action))
	{
		if (selected_button_id == buttons.size() - 1)
		{
			selectButton(0);
		}
		else
		{
			selectButton(selected_button_id + 1);
		}
	}
	
	if (game_data->getInputManager()->isActionPressed(prev_selection_action))
	{
		if (selected_button_id == 0)
		{
			selectButton(buttons.size() - 1);
		}
		else
		{
			selectButton(selected_button_id - 1);
		}
	}

	if (game_data->getInputManager()->isMouseButtonPressed(0))
	{
		double mouse_x, mouse_y;
		game_data->getInputManager()->getMousePosition(mouse_x, mouse_y);

		for (size_t i = 0; i < buttons.size(); ++i)
		{
			auto& button = buttons[i];
			auto[width, height] = button.getSize();

			if (button.getPosX() < mouse_x &&
				button.getPosX() + width > mouse_x &&
				button.getPosY() < mouse_y &&
				button.getPosY() + height > mouse_y)
			{
				selected_button_id = i;
				break;
			}
		}
	}

	if (game_data->getInputManager()->isActionPressed(hash("Enter")))
	{
		AudioLocator::get()->play("button_click.wav");

		if (buttons.size())
		{
			buttons[selected_button_id].on_click.emit();
		}
	}

}

void Menu::render(int z_order) const
{
	for (const Button& b : buttons)
	{
		b.render(game_data, z_order);

		if (b.isSelected() && selection_image)
		{
			selection_image->xPos(b.getPosX() - 8.0f - selection_image->width());
			selection_image->yPos(b.getPosY() + 2.0f - selection_image->height());
			game_data->getRenderer()->renderSprite(*selection_image, (float)z_order);
		}
	}
}

void Menu::reset()
{
	buttons.clear();
	selected_button_id = 0;
}

int Menu::addButton(float x, float y, std::string name, ASGE::Colour colour, ASGE::Colour selected_colour)
{
	Button b(game_data->getRenderer());
	b.setPos(x, y);
	b.setName(name);
	b.setColour(colour);
	b.setSelectedColour(selected_colour);
	buttons.push_back(std::move(b));

	if (buttons.size() == 1)
	{
		buttons[selected_button_id].setSelected(true);
	}

	return buttons.size() - 1;
}

int Menu::addButton(float x, float y, std::string name, ASGE::Colour colour, ASGE::Colour selected_colour, float width, float height, std::string texture)
{
	auto id = addButton(x, y, name, colour, selected_colour);
	auto& button = getButton(id);
	button.loadTexture("../../Resources/Textures/" + texture + ".png");
	button.setSize(width, height);

	return id;
}

Button& Menu::getButton(int button_id)
{
	return buttons.at(button_id);
}

void Menu::selectButton(int button_id)
{
	buttons[selected_button_id].setSelected(false);
	selected_button_id = button_id;
	buttons[selected_button_id].setSelected(true);

	AudioLocator::get()->play("button_select.wav");
}
