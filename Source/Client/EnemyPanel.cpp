#include "stdafx.h"
#include "EnemyPanel.h"
#include "Unit.hpp"

EnemyPanel::EnemyPanel(GameData * data)
{
	game_data = data;
	panel_sprite = data->getRenderer()->createUniqueSprite();
	panel_sprite->loadTexture("../../Resources/Textures/UI/EnemyPanel.png");
	panel_sprite->xPos(game_data->getWindowWidth());
	panel_sprite->yPos(60.0f);
	unit_image = data->getRenderer()->createUniqueSprite();
	unit_image->loadTexture("../../Resources/Textures/transparent pixel.png");
}

void EnemyPanel::update(float dt)
{
	if(cur_unit != nullptr && cur_unit->getInitialised())
	{
		updateUnitHealth();
	}

	if(isActive && panel_sprite->xPos() > x_position)
	{
		float x_val = panel_sprite->xPos() - (panel_move_speed * dt);
		panel_sprite->xPos(x_val);
		if(panel_sprite->xPos() < (game_data->getWindowWidth() - panel_sprite->width() ))
		{
			panel_sprite->xPos(game_data->getWindowWidth() - panel_sprite->width());
		}
		unit_image->xPos(panel_sprite->xPos() + unit_image_offset_x);
		unit_image->yPos(panel_sprite->yPos() + unit_image_offset_y);
	}

	if(!isActive && panel_sprite->xPos() <= game_data->getWindowWidth())
	{
		float x_val = panel_sprite->xPos() + (panel_move_speed * dt);
		panel_sprite->xPos(x_val);
		if(panel_sprite->xPos() > game_data->getWindowWidth())
		{
			panel_sprite->xPos(game_data->getWindowWidth());
		}
		unit_image->xPos(panel_sprite->xPos() + unit_image_offset_x);
		unit_image->yPos(panel_sprite->yPos() + unit_image_offset_y);
	}
}

void EnemyPanel::render() const
{
	game_data->getRenderer()->renderSprite(*panel_sprite, Z_ORDER_LAYER::OVERLAY + 1);
	game_data->getRenderer()->renderSprite(*unit_image, Z_ORDER_LAYER::OVERLAY + 2);
	renderStatsText(game_data->getRenderer());
}

void EnemyPanel::renderStatsText(ASGE::Renderer * rend) const
{
	rend->renderText(health, panel_sprite->xPos() + text_offset_x, panel_sprite->yPos() + text_offset_y, 1.0f, ASGE::COLOURS::AZURE, Z_ORDER_LAYER::OVERLAY_TEXT);
	rend->renderText(initiative, panel_sprite->xPos() + text_offset_x, panel_sprite->yPos() + text_offset_y + 20.0f, 1.0f, ASGE::COLOURS::AZURE, Z_ORDER_LAYER::OVERLAY_TEXT);
}

void EnemyPanel::setEnemyInPanel(std::string texture_source, Unit* current_unit)
{
	cur_unit = current_unit;

	if(current_unit != nullptr && cur_unit->getInitialised())
	{
		unit_image->loadTexture(texture_source);
		unit_image->width(120.0f);
		unit_image->height(120.0f);

		int hp = (int)cur_unit->getHealth();
		int initi = (int)cur_unit->getInitiative();

		health = "Health: " + std::to_string(hp);
		initiative = "Initiative: " + std::to_string(initi);
		return;
	}
	isActive = false;
}

void EnemyPanel::updateUnitHealth()
{
	if(cur_unit != nullptr  && cur_unit->getInitialised())
	{
		int hp = (int)cur_unit->getHealth();
		health = "Health: " + std::to_string(hp);
	}
}
