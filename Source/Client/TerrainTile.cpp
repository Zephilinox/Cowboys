#include "stdafx.h"
#include "TerrainTile.h"
#include "../Architecture/Constants.hpp"

//TODO entity info pointer - updated when unit moves to the position, made null when unit leaves.
//On click -
/*
if entity info != null
	get unit hit
		if friendly, select unit
			if enemy, attempt attack
			
else
	get tile position
	if unit selected
		pathfind
		trigger move to list
*/

ASGE::Sprite* TerrainTile::getTerrainSprite() const
{
	return ground_sprite.get();
}

ASGE::Sprite * TerrainTile::getOverlaySprite() const
{
	return overlay_sprite.get();
}

int TerrainTile::getMoveDifficultyModifier() const
{
	return time_units_cost;
}

bool TerrainTile::getIsBlocked() const
{
	return is_blocked;
}

void TerrainTile::setIsBlocked(bool new_val)
{
	is_blocked = new_val;
}

bool TerrainTile::getIsOccupied() const
{
	return is_occupied;
}

void TerrainTile::setIsOccupied(bool new_val)
{
	is_occupied = new_val;
}

void TerrainTile::render(ASGE::Renderer* rend) const
{
	if(is_visible == true)
	{
		rend->renderSprite(*ground_sprite, Z_ORDER_LAYER::BACK_GROUND);
	}
	else
	{
		rend->renderSprite(*ground_sprite, Z_ORDER_LAYER::BACK_GROUND);
		rend->renderSprite(*overlay_sprite, Z_ORDER_LAYER::BACK_GROUND + 10);
	}
}

void TerrainTile::initialise(char type, ASGE::Renderer* rend, float xCo, float yCo)
{
	ground_sprite = std::move(rend->createUniqueSprite());
	overlay_sprite = std::move(rend->createUniqueSprite());
	overlay_sprite->loadTexture("../../Resources/Textures/greyOverlay.png");
	xCoord = xCo;
	yCoord = yCo;


	switch(type)
	{
		case 'w':
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/waterTile.png");
			time_units_cost = 20;
			is_blocked = true;
			break;
		}
		case 'g':
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/dirtTile.png");
			time_units_cost = 5;
			is_blocked = false;
			break;
		}
		case 'f':
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/grassTile.png");
			time_units_cost = 10;
			is_blocked = true;
			break;
		}
		case 'r':
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/rockTile.png");
			//overlay_sprite->loadTexture("../Resources/Textures/rockTile.png");
			time_units_cost = 300;
			is_blocked = true;
			break;
		}
		default:
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/rockTile.png");
			//overlay_sprite->loadTexture("../Resources/Textures/rockTile.png");
			time_units_cost = 300;
			is_blocked = true;
			break;
		}
	}
}

float TerrainTile::getCombinedCost()
{
	return dist_from_start + dist_to_goal + time_units_cost;
}
