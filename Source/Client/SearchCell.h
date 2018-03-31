#pragma once
#include <cmath>

constexpr int grid_size = 100;

class SearchCell
{
public:
	int xCoord;
	int yCoord;
	SearchCell *parent;
	int id;

	float dist_from_start;
	float dist_from_goal;
	float time_units_cost;
	float combined_cost;


	SearchCell(int x, int y, SearchCell *_parent = 0) : xCoord(x), yCoord(y), id(y * grid_size + y), dist_from_start(0), dist_from_goal(0) {};
	~SearchCell() = default;

	float getCombinedCost() { return dist_from_start + dist_from_goal + time_units_cost; };

	float ManHattanDistance(SearchCell *nodeEnd)
	{
		float x = (float)(fabs((float)(this->xCoord - nodeEnd->xCoord)) );
		float y = (float)(fabs((this->yCoord - nodeEnd->yCoord)) );

		return x + y;
	};

};