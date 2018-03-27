#include "../States/WarbandSelectionState.h"

//SELF
#include "../../Architecture/GameData.hpp"
#include "../../Architecture/Constants.hpp"
#include "GameState.h"

#include "../PortraitTextureStrings.h"

WarbandSelectionState::WarbandSelectionState(GameData* game_data)
	: State(game_data),
	panel1(game_data, 0.0f),
	panel2(game_data, 256.0f),
	panel3(game_data, 512.0f),
	panel4(game_data, 768.0f),
	panel5(game_data, 1024.0f),
	menu(game_data)
{
	portraits.reserve(number_of_portraits);

	//TODO streamline this OR JSON it
	portraits.push_back(one);
	portraits.push_back(two);
	portraits.push_back(three);
	portraits.push_back(four);
	portraits.push_back(five);
	portraits.push_back(six);
	portraits.push_back(seven);
	portraits.push_back(eight);

	initMenu();
}

void WarbandSelectionState::update(const ASGE::GameTime&)
{
	panel1.update();
	panel2.update();
	panel3.update();
	panel4.update();
	panel5.update();
	menu.update();
}

void WarbandSelectionState::render() const
{
	panel1.render(game_data->getRenderer());
	panel2.render(game_data->getRenderer());
	panel3.render(game_data->getRenderer());
	panel4.render(game_data->getRenderer());
	panel5.render(game_data->getRenderer());
	menu.render();
}

void WarbandSelectionState::onActive()
{
}

void WarbandSelectionState::onInactive()
{
}

std::string WarbandSelectionState::getTextureStringAtPositon(int index)
{
	//TODO review this to catch errors correctly
	if(index < portraits.size())
	{
		return portraits[index];
	}
	else
	{
		return "bad";
	}
}


void WarbandSelectionState::initMenu()
{
	//TODO refactor this entire initialisation into a menu init function, cleans up constructor

	menu.addButton(0.0f + 25, 640, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(0.0f + 175, 640, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);

	menu.addButton(256.0f + 25, 640, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(256.0f + 175, 640, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);

	menu.addButton(512.0f + 25, 640, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(512.0f + 175, 640, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);

	menu.addButton(768.0f + 25, 640, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(768.0f + 175, 640, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);

	menu.addButton(1024.0f + 25, 640, "Previous", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);
	menu.addButton(1024.0f + 175, 640, "Next", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);

	menu.addButton(game_data->getWindowWidth() - 100, game_data->getWindowHeight() * 0.5f, "DONE!", ASGE::COLOURS::BLACK, ASGE::COLOURS::ANTIQUEWHITE);

	menu.getButton(10).on_click.connect([this]()
	{
		game_data->getStateManager()->pop();

		game_data->getStateManager()->push<GameState>
			(panel1.getSelectedUnitID(),
			panel2.getSelectedUnitID(),
			panel3.getSelectedUnitID(),
			panel4.getSelectedUnitID(),
			panel5.getSelectedUnitID());
	});


	menu.getButton(0).on_click.connect([this]()
	{
		panel1.decrementSelectedUnit();
		buttonAction(panel1);
	});

	menu.getButton(1).on_click.connect([this]()
	{
		panel1.incrementSelectedUnit();
		buttonAction(panel1);

	});

	menu.getButton(2).on_click.connect([this]()
	{
		panel2.decrementSelectedUnit();
		buttonAction(panel2);
	});

	menu.getButton(3).on_click.connect([this]()
	{
		panel2.incrementSelectedUnit();
		buttonAction(panel2);
	});

	menu.getButton(4).on_click.connect([this]()
	{
		panel3.decrementSelectedUnit();
		buttonAction(panel3);
	});

	menu.getButton(5).on_click.connect([this]()
	{
		panel3.incrementSelectedUnit();
		buttonAction(panel3);
	});

	menu.getButton(6).on_click.connect([this]()
	{
		panel4.decrementSelectedUnit();
		buttonAction(panel4);
	});

	menu.getButton(7).on_click.connect([this]()
	{
		panel4.incrementSelectedUnit();
		buttonAction(panel4);

	});

	menu.getButton(8).on_click.connect([this]()
	{
		panel5.decrementSelectedUnit();
		buttonAction(panel5);
	});

	menu.getButton(9).on_click.connect([this]()
	{
		panel5.incrementSelectedUnit();
		buttonAction(panel5);
	});

}

void WarbandSelectionState::buttonAction(UnitSelectPanel& panel)
{
	if(panel.getSelectedUnitID() < 0)
	{
		panel.setSelectedUnitID(portraits.size() - 1);
	}
	else if(panel.getSelectedUnitID() > portraits.size() - 1)
	{
		panel.setSelectedUnitID(0);
	}
	panel.setNewUnitTexture(portraits[panel.getSelectedUnitID()]);
	//std::cout << "new unit: " << panel1.getSelectedUnitID() << std::endl;
}