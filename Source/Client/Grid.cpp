#include "Grid.h"

Grid::Grid(GameData* data) : game_data(data)
{
}

void Grid::generateCharGrid(int seed)
{
	game_data->getRandomNumberGenerator()->setSeed(seed);
	
	for(int x = 0; x < mapWidth; x++)
	{
		for(int y = 0; y < mapHeight; y++)
		{
			char new_char;
			int val = game_data->getRandomNumberGenerator()->getRandomInt(0, 9, true);

			switch(val)
			{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				{
					new_char = 'g';
					break;
				}
				case 9:
				{
					new_char = 'r';
					break;
				}

			}
			grid[x][y] = new_char;
		}
	}
}

float Grid::getTileXPosAtArrayPos(int x, int y) const
{
	return map[x][y].getTerrainSprite()->xPos();
}

float Grid::getTileYPosAtArrayPos(int x, int y) const
{
	return map[x][y].getTerrainSprite()->yPos();
}

void Grid::applyOffset(float x, float y)
{
	offset_x = x;
	offset_y = y;
}

void Grid::render() const
{
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			auto* sprite = map[x][y].getTerrainSprite();
			if (withinView(sprite))
			{
				sprite->xPos(sprite->xPos() - offset_x);
				sprite->yPos(sprite->yPos() - offset_y);
				map[x][y].render(game_data->getRenderer());
				sprite->xPos(sprite->xPos() + offset_x);
				sprite->yPos(sprite->yPos() + offset_y);
			}
		}
	}
}

void Grid::loadHardCodedMap()
{
	for(int x = 0; x < mapWidth; x++)
	{
		for(int y = 0; y < mapHeight; y++)
		{
			map[x][y].initialise(grid[x][y], game_data->getRenderer());
			map[x][y].getTerrainSprite()->xPos(x * map[x][y].getTerrainSprite()->width());
			map[x][y].getTerrainSprite()->yPos(y * map[x][y].getTerrainSprite()->height());
		}
	}
}

bool Grid::withinView(ASGE::Sprite* sprite) const
{
	float x = sprite->xPos() - offset_x;
	float y = sprite->yPos() - offset_y;

	if (x + sprite->width() > 0 &&
		x  < game_data->getWindowWidth() &&
		y + sprite->height() > 0 &&
		y < game_data->getWindowHeight())
	{
		return true;
	}

	return false;
}
