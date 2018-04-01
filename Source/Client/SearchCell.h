#pragma once
#include <cmath>

constexpr int grid_size = 100;

struct SearchCell
{
public:

	
	int xCoord;
	int yCoord;
	SearchCell *parent;
	int id;

	float G;
	float H;

	SearchCell(int x, int y, SearchCell *_parent = 0) : xCoord(x), yCoord(y), id(y * grid_size + y), G(0), H(0) {};

	float GetF() { return G + H; };

	float ManHattanDistance(SearchCell *nodeEnd)
	{
		float x = (float)(fabs(this->xCoord - nodeEnd->xCoord));
		float y = (float)(fabs(this->yCoord - nodeEnd->yCoord));

		return x + y;
	}

	
};