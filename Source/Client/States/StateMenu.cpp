#include "StateMenu.hpp"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../../Architecture/Constants.hpp"

#include "StateLobby.hpp"

StateMenu::StateMenu(GameData* game_data)
	: State(game_data)
	, menu(game_data)
	, grid(game_data)
{
	grid.generateCharGrid(game_data->getRandomNumberGenerator()->getRandomInt(1, 2));
	grid.loadHardCodedMap();
	dir_x = game_data->getRandomNumberGenerator()->getRandomFloat(-1, 1);
	dir_y = game_data->getRandomNumberGenerator()->getRandomFloat(-1, 1);
	speed = game_data->getRandomNumberGenerator()->getRandomFloat(100, 300);
	delay = game_data->getRandomNumberGenerator()->getRandomFloat(2.0f, 4.0f);

	int max_right_tile = (mapWidth - 32);
	int max_downtile = (mapHeight - 18);

	auto map_left = grid.getTileXPosAtArrayPos(0, 0);
	auto map_right = grid.getTileXPosAtArrayPos(max_right_tile, 0);
	auto map_top = grid.getTileYPosAtArrayPos(0, 0);
	auto map_bottom = grid.getTileYPosAtArrayPos(0, max_downtile);

	offset_x = game_data->getRandomNumberGenerator()->getRandomFloat(map_left, map_right);
	offset_y = game_data->getRandomNumberGenerator()->getRandomFloat(map_top, map_bottom);

	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f - 40.0f, "PLAY", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);
	menu.addButton(game_data->getWindowWidth() / 2.0f - 80.0f, game_data->getWindowHeight() / 2.0f, "EXIT", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE, 60.0f, 20.0f);
	
	menu.getButton(0).on_click.connect([game_data]()
	{
		game_data->getStateManager()->push<StateLobby>();
	});

	menu.getButton(1).on_click.connect([game_data]()
	{
		game_data->getStateManager()->pop();
	});

	game_data->getMusicPlayer()->play("FF7");
}

void StateMenu::update(const ASGE::GameTime& gt)
{
	menu.update();
	if (randomiseTimer.getElapsedTime() > delay)
	{
		delay = game_data->getRandomNumberGenerator()->getRandomFloat(2.0f, 4.0f);
		randomiseTimer.restart();
		randomiseCameraMovement();
	}

	int max_right_tile = (mapWidth - 32);
	int max_downtile = (mapHeight - 18);

	auto map_left = grid.getTileXPosAtArrayPos(0, 0);
	auto map_right = grid.getTileXPosAtArrayPos(max_right_tile, 0);
	auto map_top = grid.getTileYPosAtArrayPos(0, 0);
	auto map_bottom = grid.getTileYPosAtArrayPos(0, max_downtile);
	
	if (offset_x > map_left && offset_x < map_right)
	{
		offset_x += dir_x * speed * (gt.delta_time.count() / 1000.0f);

		if (offset_x < map_left)
		{
			std::cout << ":o1\n";
			offset_x = map_left;
			dir_x = -dir_x;
		}
		else if (offset_x > map_right)
		{
			std::cout << ":o2\n";
			offset_x = map_right;
			dir_x = -dir_x;
		}
	}

	if (offset_y > map_top && offset_y < map_bottom)
	{
		offset_y += dir_y * speed * (gt.delta_time.count() / 1000.0f);

		if (offset_y < map_top)
		{
			std::cout << ":o3\n";
			offset_y = map_top;
			dir_y = -dir_y;
		}
		else if (offset_y > map_bottom)
		{
			std::cout << ":o4\n";
			offset_y = map_bottom;
			dir_y = -dir_y;
		}
	}

	grid.applyOffset(offset_x, offset_y);
}

void StateMenu::render() const
{
	grid.render();
	menu.render();
}

void StateMenu::onActive()
{
	game_data->getMusicPlayer()->play("FF7");
}

void StateMenu::onInactive()
{
}

void StateMenu::randomiseCameraMovement()
{
	auto val = game_data->getRandomNumberGenerator()->getRandomFloat(0, 1);

	dir_x = game_data->getRandomNumberGenerator()->getRandomFloat(-1, 1);
	dir_y = game_data->getRandomNumberGenerator()->getRandomFloat(-1, 1);

	if (val < 0.5f)
	{
		speed = game_data->getRandomNumberGenerator()->getRandomFloat(150, 250);
	}

	std::cout << std::setprecision(10) << "x = " << dir_x << "y = " << dir_y << " speed = " << speed << "\n";
}
