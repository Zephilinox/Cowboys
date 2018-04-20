#pragma once
#include <Engine\Sprite.h>
#include <Engine\Renderer.h>

class TerrainTile
{
public:

	enum TerrainType
	{
		GRASS,
		WATER,
		ROCK,
		FENCE,
	};

	TerrainTile() = default;
	~TerrainTile() = default;

	void render(ASGE::Renderer* rend) const;

	ASGE::Sprite* getTerrainSprite() const;
	ASGE::Sprite* getOverlaySprite() const;
	ASGE::Sprite* getPathingSprite() const;
	int getMoveDifficultyModifier() const;
	bool getIsBlocked() const;
	void setIsBlocked(bool new_val);
	bool getIsOccupied() const;
	void setIsOccupied(bool new_val);

	void initialise(char type, ASGE::Renderer* rend, float xCo, float yCo);

	//TODO move some of these to private and make accessors
	//Pathfinding
	void setRenderPathing(bool new_val);

	float getCombinedCost();
	void setDistanceFromStart(float new_val)		{ dist_from_start = new_val; }
	void setDistanceToGoal(float new_val)			{ dist_to_goal = new_val; }
	void setParent(TerrainTile* par)				{ parent = par; }
	TerrainTile* parent;
	int id;
	float dist_from_start;
	float dist_to_goal;

	void setIsVisible(bool new_val) { is_visible = new_val; }
	bool getIsVisible() { return is_visible; }
	int xCoord = 0;
	int yCoord = 0;

private:
	//Use an overlay sprite for fences, can then change direction.
	std::unique_ptr<ASGE::Sprite> ground_sprite;
	std::unique_ptr<ASGE::Sprite> pathing_sprite;
	std::unique_ptr<ASGE::Sprite> overlay_sprite;
	int time_units_cost = 0;
	bool render_pathing_sprite = false;
	bool is_blocked = false;
	bool is_occupied = false;
	bool is_visible = false;
};
