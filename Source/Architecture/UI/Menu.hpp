#pragma once

//STD
#include <vector>

//LIB
#include <Engine/Sprite.h>

#include "../Constants.hpp"

//SELF
#include "Button.hpp"

class GameData;

class Menu
{
public:
	Menu(GameData* game_data, bool vertical = true);

	void update();
	void render(int z_order = Z_ORDER_LAYER::OVERLAY_TEXT) const;
	void reset();

	int addButton(int x, int y, std::string name, ASGE::Colour colour, ASGE::Colour selected_colour);
	Button& getButton(int button_id);

	std::unique_ptr<ASGE::Sprite> selection_image = nullptr;
private:
	void selectButton(int button_id);

	GameData* game_data;
	std::vector<Button> buttons;

	unsigned selected_button_id;
};