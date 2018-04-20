#pragma once
#pragma once

#include <Engine\Renderer.h>
#include "../Architecture/UI/Menu.hpp"

class GameData;
class Unit;

class EnemyPanel
{
public:
	EnemyPanel(GameData* data);
	~EnemyPanel() = default;

	void update(float dt);
	void render() const;

	void renderStatsText(ASGE::Renderer* rend) const;

	void setEnemyInPanel(std::string texture_source, Unit* current_unit);
	void updateUnitHealth();

	void setIsActive(bool new_val) { isActive = new_val; }
	bool getIsActive() { return isActive; }

private:
	bool isActive = false;
	float x_position = 0.0f;
	float y_position = 0.0f;

	float panel_move_speed = 180.0f;

	float unit_image_offset_x = 40.0f;
	float unit_image_offset_y = 10.0f;
	float text_offset_x = 40.0f;
	float text_offset_y = 160.0f;
	std::unique_ptr<ASGE::Sprite> panel_sprite;
	std::unique_ptr<ASGE::Sprite> unit_image;

	Unit* cur_unit = nullptr;

	std::string health;
	std::string initiative;

	GameData* game_data = nullptr;
};













