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

	void incrementSelectedUnit();
	void decrementSelectedUnit();

	int getSelectedUnitID();
	void setSelectedUnitID(int id);

	void setNewUnitTexture(std::string tex);

private:

	float x_position = 0.0f;
	float y_position = 0.0f;

	std::unique_ptr<ASGE::Sprite> panel_image;
	std::unique_ptr<ASGE::Sprite> unit_image;

	int selected_unit = 0;
	int previous_unit = 0;
};