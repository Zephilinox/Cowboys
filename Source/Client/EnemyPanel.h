#pragma once
#pragma once

#include <Engine\Renderer.h>
#include "../Architecture/UI/Menu.hpp"

class GameData;

class EnemyPanel
{
public:
	EnemyPanel(GameData* data);
	~EnemyPanel() = default;

	void update();
	void render() const;

	void renderStatsText(ASGE::Renderer* rend) const;

	void setNewUnitTexture(std::string tex);
	void pullStatsJson();

	void setIsActive(bool new_val) { isActive = new_val; }
	bool getIsActive() { return isActive; }

private:
	bool isActive = false;
	float x_position = 0.0f;
	float y_position = 0.0f;

	float unit_image_offset_x = 40.0f;
	float unit_image_offset_y = 40.0f;
	float text_offset_x = 40.0f;
	float text_offset_y = 180.0f;
	std::unique_ptr<ASGE::Sprite> panel_sprite;
	std::unique_ptr<ASGE::Sprite> unit_image;

	std::string health;
	std::string initiative;

	GameData* game_data = nullptr;
};













