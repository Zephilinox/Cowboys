#pragma once

//STD
#include <functional>
#include <string>

//LIB
#include <Engine/Colours.h>

//SELF
#include "../GameData.hpp"
#include "../Signals/Signal.hpp"
#include "../Constants.hpp"

class Button
{
public:
	Button() noexcept;

	void render(GameData* game_data, int z_order = Z_ORDER_LAYER::OVERLAY_TEXT) const;

	bool isSelected() const noexcept;
	void setSelected(bool selected) noexcept;

	int getPosX() const noexcept;
	int getPosY() const noexcept;

	void setPos(int x, int y) noexcept;
	void setName(std::string name);
	void setColour(ASGE::Colour colour) noexcept;
	void setSelectedColour(ASGE::Colour colour) noexcept;

	Signal<> on_click;

private:
	bool selected = false;
	
	int pos_x = 0;
	int pos_y = 0;
	std::string name = "Default";
	ASGE::Colour colour = ASGE::COLOURS::RED;
	ASGE::Colour selected_colour = ASGE::COLOURS::BLUE;
};