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

float TerrainTile::getMoveDifficultyModifier() const
{
	return time_units_cost;
}

bool TerrainTile::getIsBlocked() const
{
	return isBlocked;
}

void TerrainTile::setIsBlocked(bool new_val)
{
	isBlocked = new_val;
}

void TerrainTile::render(ASGE::Renderer* rend) const
{
	rend->renderSprite(*ground_sprite, Z_ORDER_LAYER::BACK_GROUND);
	//rend->renderSprite(*overlay_sprite);
}

void TerrainTile::initialise(char type, ASGE::Renderer* rend, float xCo, float yCo)
{
	ground_sprite = std::move(rend->createUniqueSprite());

	xCoord = xCo;
	yCoord = yCo;

	switch(type)
	{
		case 'w':
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/waterTile.png");
			time_units_cost = 20.0f;
			isBlocked = true;
			break;
		}
		case 'g':
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/dirtTile.png");
			time_units_cost = 5.0f;
			isBlocked = false;
			break;
		}
		case 'f':
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/grassTile.png");
			time_units_cost = 10.0f;
			isBlocked = true;
			break;
		}
		case 'r':
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/rockTile.png");
			//overlay_sprite->loadTexture("../Resources/Textures/rockTile.png");
			time_units_cost = 300.0f;
			isBlocked = true;
			break;
		}
		default:
		{
			ground_sprite->loadTexture("../../Resources/Textures/Tiles/rockTile.png");
			//overlay_sprite->loadTexture("../Resources/Textures/rockTile.png");
			time_units_cost = 300.0f;
			isBlocked = true;
			break;
		}
	}
}

float TerrainTile::getCombinedCost()
{
	return dist_from_start + dist_to_goal + time_units_cost;
}
