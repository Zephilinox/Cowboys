#include "Grid.h"

Grid::Grid(GameData* data)
	: game_data(data)
{
}

void Grid::generateCharGrid(int seed)
{
	game_data->getRandomNumberGenerator()->setSeed(seed);
	
	//use seed to load JSON into buildings container
	loadJSONBuildings(seed);

	std::vector<std::pair<int, int>> rocks;

	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			char new_char = 'g';
			float val = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);

			if (val < 0.02f)
			{
				new_char = 'r';
				rocks.push_back(std::make_pair(x, y));
			}

			grid[x][y] = new_char;
		}
	}

	std::vector<std::pair<int, int>> rocks2;

	for (auto rock_pos : rocks)
	{
		float rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);

		if (rand < 0.1f)
		{
			continue;
		}

		auto[x, y] = rock_pos;

		if (x < mapWidth - 1 && y < mapHeight - 1)
		{
			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x + 1][y] = 'r';
				rocks2.push_back(std::make_pair(x + 1, y));
			}

			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x][y + 1] = 'r';
				rocks2.push_back(std::make_pair(x, y + 1));
			}

			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x + 1][y + 1] = 'r';
				rocks2.push_back(std::make_pair(x + 1, y + 1));
			}
		}
	}
	
	for (auto rock_pos : rocks2)
	{
		float rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);

		if (rand < 0.1f)
		{
			continue;
		}

		auto[x, y] = rock_pos;

		if (x < mapWidth - 1 && y < mapHeight - 1)
		{
			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x + 1][y] = 'r';
			}

			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.6f)
			{
				grid[x][y + 1] = 'r';
			}

			rand = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);
			if (rand < 0.6f)
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

void Grid::addBuildingToMap(Building& building)
{
	int buildingStartPosX = building.arrayPosX;
	int buildingStartPosY = building.arrayPosY;
	int buildingHeight = building.buildingHeight;
	int buildingWidth = building.buildingWidth;
	std::string texture_source = building.buildingTextureSource;
	
	//block access to all tiles EXCEPT right in front of the building
	//TODO confirm this loop doesn't need to be buildingHeight - 1?
	for(int x = buildingStartPosX; x < buildingHeight; x++)
	{
		for(int y = buildingStartPosY; y < buildingWidth; y++)
		{
			map[x][y].setIsBlocked(true);
		}
	}

	map[buildingStartPosX][buildingStartPosY].getTerrainSprite()->loadTexture(texture_source);
}

void Grid::loadJSONBuildings(int seed)
{
	std::ifstream file("../../Resources/mapBuildings.json");
	jsoncons::json layout_JSON_obj;
	file >> layout_JSON_obj;

	std::string layoutStr = "layout" + std::to_string(seed);

	int buildingsCount = layout_JSON_obj[layoutStr]["numberOfBuildings"].as_int();
	int counter = 1;

	while(counter <= buildingsCount)
	{
		std::string building_no = "building" + std::to_string(counter);

		Building new_build;
		try
		{
			//STATS
			new_build.buildingHeight = layout_JSON_obj[layoutStr][building_no]["height"].as_int();
			new_build.buildingWidth = layout_JSON_obj[layoutStr][building_no]["width"].as_int();
			new_build.arrayPosX = layout_JSON_obj[layoutStr][building_no]["arrayX"].as_int();
			new_build.arrayPosY = layout_JSON_obj[layoutStr][building_no]["arrayY"].as_int();
			new_build.buildingTextureSource = layout_JSON_obj[layoutStr][building_no]["texture"].as_string();
			buildings.push_back(std::move(new_build));
		}
		catch(std::runtime_error& e)
		{
			std::cout << "ERROR INFO: " << e.what() << "\n";
		}

		counter++;
	}
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

	for(auto& building : buildings)
	{
		addBuildingToMap(building);
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
