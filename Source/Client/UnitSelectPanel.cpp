#include "UnitSelectPanel.h"
#include "../Client/States/WarbandSelectionState.h"

UnitSelectPanel::UnitSelectPanel(GameData* data, float x_pos) 
	: panel_image(data->getRenderer()->createUniqueSprite()),
	unit_image(data->getRenderer()->createUniqueSprite())
{
	x_position = x_pos;
	//y_position = y_pos;

	panel_image->loadTexture("../../Resources/Textures/UI/Panels/UnitSelectPanel.png");
	panel_image->xPos(x_position);
	panel_image->yPos(y_position);

	//TODO rework this to load default unit forward facing sprite
	unit_image->loadTexture("../../Resources/Textures/ppl1.png");
	unit_image->xPos(x_position);
	unit_image->yPos(y_position);


}

void UnitSelectPanel::update()
{
}

void UnitSelectPanel::render(ASGE::Renderer* rend) const
{
	rend->renderSprite(*panel_image, Z_ORDER_LAYER::OVERLAY);
	rend->renderSprite(*unit_image, Z_ORDER_LAYER::OVERLAY_TEXT);
}

void UnitSelectPanel::incrementSelectedUnit()
{
	selected_unit++;
}

void UnitSelectPanel::decrementSelectedUnit()
{
	selected_unit--;
}

int UnitSelectPanel::getSelectedUnitID()
{
	return selected_unit;
}

void UnitSelectPanel::setSelectedUnitID(int id)
{
	selected_unit = id;
}

void UnitSelectPanel::setNewUnitTexture(std::string tex)
{
	unit_image->loadTexture(tex);
}
