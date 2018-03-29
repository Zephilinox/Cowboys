#pragma once

#include <Engine\Renderer.h>
#include "../Architecture/UI/Menu.hpp"

class GameData;

class UnitSelectPanel
{
public:
	UnitSelectPanel(GameData* data, float x_pos);
	~UnitSelectPanel() = default;

	void update();
	void render(ASGE::Renderer* rend) const;

	void renderStatsText(ASGE::Renderer* rend) const;

	void incrementSelectedUnit();
	void decrementSelectedUnit();

	int getSelectedUnitID();
	void setSelectedUnitID(int id);

	void setNewUnitTexture(std::string tex);
	void pullStatsJson();

	int getNumberOfUnitTypes();



private:
	void loadNumberOfUnitTypes();

	float x_position = 0.0f;
	float y_position = 0.0f;

	std::unique_ptr<ASGE::Sprite> panel_image;
	std::unique_ptr<ASGE::Sprite> unit_image;

	int selected_unit = 0;
	int previous_unit = 0;

	int number_of_unit_types = 0;

	std::string title;
	std::string health;
	std::string view_distance;
	std::string time_units;
	std::string stamina;
	std::string bravery;
	std::string reactions;
	std::string firing_accuracy;
	std::string strength;
	std::string initiative;
	std::string base_move_speed;

	std::string textureSource;
};