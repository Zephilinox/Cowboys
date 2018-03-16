#pragma once

#include <Engine\Sprite.h>
#include <iostream>

constexpr unsigned int MAX_TEXTURES = 100;

//TODO Ricardo - Does a central place for all of the texture paths make sense?

class SpritePaths
{
public:
	char charTextureStrings[MAX_TEXTURES];
};