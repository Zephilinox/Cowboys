#include "Grid.h"

Grid::Grid(GameData* data) : game_data(data)
{
}

void Grid::generateCharGrid(int seed)
{
	game_data->getRandomNumberGenerator()->setSeed(seed);
	
	std::vector<std::pair<int, int>> rocks;

	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			char new_char = 'g';
			float val = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);

			if (val < 0.05f)
			{
				new_char = 'r';
				rocks.push_back(std::make_pair(x, y));
			}

			grid[x][y] = new_char;
		}
	}

	for (auto rock_pos : rocks)
	{
		float rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);

		if (rand < 0.3f)
		{
			continue;
		}

		auto[x, y] = rock_pos;

		if (x < mapWidth - 1 && y < mapHeight - 1)
		{
			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.8f)
			{
				grid[x + 1][y] = 'r';
			}

			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.8f)
			{
				grid[x][y + 1] = 'r';
			}

			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.8f)
			{
				grid[x + 1][y + 1] = 'r';
			}
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
