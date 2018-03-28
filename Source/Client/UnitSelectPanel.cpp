#include "UnitSelectPanel.h"
#include "../Client/States/WarbandSelectionState.h"
//LIB
#include <jsoncons/json.hpp>

UnitSelectPanel::UnitSelectPanel(GameData* data, float x_pos) 
	: panel_image(data->getRenderer()->createUniqueSprite()),
	unit_image(data->getRenderer()->createUniqueSprite())
{
	x_position = x_pos;
	//y_position = y_pos;

	pullStatsJson();
	loadNumberOfUnitTypes();

	panel_image->loadTexture("../../Resources/Textures/UI/Panels/UnitSelectPanel.png");
	panel_image->xPos(x_position);
	panel_image->yPos(y_position);
	panel_image->height(data->getWindowHeight() * 0.7f);

	unit_image->xPos(x_position + 23);
	unit_image->yPos(y_position + 23);
}


//TODO pull texture location from JSON here, instead of in Warband Selection State
void UnitSelectPanel::pullStatsJson()
{
	std::ifstream file("../../Resources/unitStats.json");
	jsoncons::json unitStats;
	file >> unitStats;

	std::string unit_JSON = "unit" + std::to_string(selected_unit);

	try
	{
		title = (unitStats[unit_JSON]["Title"].as_string());
		health = "Health: " + (unitStats[unit_JSON]["HP"].as_string());
		view_distance = "View Distance: " + (unitStats[unit_JSON]["viewDistance"].as_string());
		time_units = "Time Units: " + (unitStats[unit_JSON]["timeUnits"].as_string());
		stamina = "Stamina: " + (unitStats[unit_JSON]["stamina"].as_string());
		bravery = "Bravery: " + (unitStats[unit_JSON]["bravery"].as_string());
		reactions = "Reactions: " + (unitStats[unit_JSON]["reactions"].as_string());
		firing_accuracy = "Accuracy: " +(unitStats[unit_JSON]["firing_accuracy"].as_string());
		strength = "Strength: " + (unitStats[unit_JSON]["strength"].as_string());
		initiative = "Initiative: " + (unitStats[unit_JSON]["initiative"].as_string());
		base_move_speed = "Speed: " + (unitStats[unit_JSON]["base_move_speed"].as_string());
		textureSource = (unitStats[unit_JSON]["PortraitSource"].as_string());
		unit_image->loadTexture(textureSource);
	}
	catch(std::runtime_error& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}
}

int UnitSelectPanel::getNumberOfUnitTypes()
{
	return number_of_unit_types;
}

void UnitSelectPanel::loadNumberOfUnitTypes()
{
	std::ifstream file("../../Resources/unitStats.json");
	jsoncons::json unitStats;
	file >> unitStats;
	number_of_unit_types = (unitStats["constants"]["NumberOfUnitTypes"].as_int());
}

void UnitSelectPanel::update()
{
}

void UnitSelectPanel::render(ASGE::Renderer* rend) const
{
	rend->renderSprite(*panel_image, 0);
	rend->renderSprite(*unit_image, 1);
	renderStatsText(rend);
}

void UnitSelectPanel::renderStatsText(ASGE::Renderer* rend) const
{
	float x_pos = panel_image->xPos() + 25;

	const int baseHeight = 250;
	rend->renderText(title, x_pos, baseHeight, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(health, x_pos, baseHeight + 15, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(view_distance, x_pos, baseHeight + 30, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(time_units, x_pos, baseHeight + 45, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(stamina, x_pos, baseHeight + 60, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(bravery, x_pos, baseHeight + 75, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(reactions, x_pos, baseHeight + 90, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(firing_accuracy, x_pos, baseHeight + 105, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(strength, x_pos, baseHeight + 120, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(initiative, x_pos, baseHeight + 135, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
	rend->renderText(base_move_speed, x_pos, baseHeight + 150, 1.0f, ASGE::COLOURS::CRIMSON, Z_ORDER_LAYER::OVERLAY_TEXT + 1);
}

void UnitSelectPanel::incrementSelectedUnit()
{
	selected_unit++;
	if(selected_unit > number_of_unit_types - 1)
	{
		selected_unit = 0;
	}
	pullStatsJson();
}

void UnitSelectPanel::decrementSelectedUnit()
{
	selected_unit--;
	if(selected_unit < 0)
	{
		selected_unit = number_of_unit_types - 1;
	}
	pullStatsJson();
}

int UnitSelectPanel::getSelectedUnitID()
{
	return selected_unit;
}

void UnitSelectPanel::setSelectedUnitID(int id)
{
	selected_unit = id;
}

void UnitSelectPanel::setNewUnitTexture(std::string tex)
{
	unit_image->loadTexture(tex);
}
