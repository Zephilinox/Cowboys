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
	float getMoveDifficultyModifier() const;
	bool getIsBlocked() const;
	void setIsBlocked(bool new_val);

	void initialise(char type, ASGE::Renderer* rend);

private:
	//Use an overlay sprite for fences, can then change direction.
	std::unique_ptr<ASGE::Sprite> ground_sprite;
	std::unique_ptr<ASGE::Sprite> overlay_sprite;
	float move_difficulty_modifier = 0.0f;
	bool isBlocked = false;
};