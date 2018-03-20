#pragma once
#include <cmath>



class Grid
{
public:

	Grid(int x, int y);
	~Grid();
	float GetF();

private:
	int xCoord;
	int yCoord;

	int id;

	float G;
	float H;

	int gridSize = 100;

	char grid[10][10] = {
	{'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'w', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'w', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'w', 'g', 'g', 'w'},
	{'w', 'w', 'w', 'w', 'g', 'g', 'w', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'w', 'w', 'w', 'w'},
	{'w', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'g', 'w'},
	{'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},
	};
};