#include "Grid.h"



Grid::Grid(GameData* data) : game_data(data)
{
}

void Grid::randomiseCharGrid(int seed)
{
	game_data->getRandomNumberGenerator()->setSeed(seed);
	
	for(int x = 0; x < mapWidth; x++)
	{
		for(int y = 0; y < mapHeight; y++)
		{
			char new_char;
			int val = game_data->getRandomNumberGenerator()->getRandomInt(0, 3, true);
			switch(val)
			{
			case 0:
			{
				new_char = 'g';
				break;
			}
			case 1:
			{
				new_char = 'r';
				break;
			}
			case 2:
			{
				new_char = 'w';
				break;
			}
			case 3:
			{
				new_char = 'f';
				break;
			}
			}
			grid[x][y] = new_char;
		}
	}
}

float Grid::GetF()
{
	return G + H;
}

void Grid::render() const
{
	for(int x = 0; x < mapWidth; x++)
	{
		for(int y = 0; y < mapHeight; y++)
		{
			map[x][y].render(game_data->getRenderer());
		}
	}
}

void Grid::loadFromJSON(int map_to_load)
{
	std::string id = "map" + map_to_load;
	std::ifstream file("../../Resources/maps.json");
	jsoncons::json maps;
	file >> maps;

	try
	{
		//load in map

		//create tiles cased on chars

	}
	catch(std::runtime_error& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
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
