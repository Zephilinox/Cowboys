#pragma once

/*! \file Constants.h
@brief   Constants that are used throughout the game. 
@details Add any new constants to this file. 
*/

constexpr float PI = 3.14159f;
constexpr float ANGLE_UP = 0;
constexpr float ANGLE_RIGHT = PI / 2;
constexpr float ANGLE_DOWN = PI;
constexpr float ANGLE_LEFT = -PI / 2;
constexpr float dark[3] = { 0.4, 0.349, 0.317 };

enum Z_ORDER_LAYER
{
	STAGE = 0,
	PROPS = 1000,
	CHARACTERS = 2000,
	PANELS = 5000,
	PANELS_TEXT = 6000,
	PANELS_HIGHER = 7000,
	PANELS_HIGHER_TEXT = 8000,
	CURTAINS = 9000,
};