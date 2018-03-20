#include "Grid.h"

Grid::Grid(int x, int y)
{
	xCoord = x;
	yCoord = y;
	id = (y * gridSize + y);
	G = 0;
	H = 0;



}

Grid::~Grid()
{

}

float Grid::GetF()
{
	return G + H;
}