#include "Button.hpp"

//SELF
#include "../GameData.hpp"
#include "../Managers/FontManager.hpp"
#include "../Constants.hpp"

Button::Button() noexcept
{
}

void Button::render(GameData* game_data, int z_order) const
{
	game_data->getFontManager()->setFont("Dialogue");

	if (selected)
	{
		game_data->getRenderer()->renderText(name.c_str(), pos_x, pos_y, 1.0f, selected_colour, z_order);
	}
	else
	{
		game_data->getRenderer()->renderText(name.c_str(), pos_x, pos_y, 1.0f, colour, z_order);
	}
}

bool Button::isSelected() const noexcept
{
	return selected;
}

void Button::setSelected(bool s) noexcept
{
	selected = s;
}

float Button::getPosX() const noexcept
{
	return pos_x;
}

float Button::getPosY() const noexcept
{
	return pos_y;
}

void Button::setPos(float x, float y) noexcept
{
	pos_x = x;
	pos_y = y;
}

void Button::setName(std::string n)
{
	name = n;
}

void Button::setColour(ASGE::Colour c) noexcept
{
	colour = c;
}

void Button::setSelectedColour(ASGE::Colour c) noexcept
{
	selected_colour = c;
}
