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

enum Z_ORDER_LAYER
{
	BACK_GROUND = 0,
	MIDDLE_GROUND = 1000,
	FORE_GROUND = 2000,
	CHARACTERS = 3000,
	OVERLAY = 4000
};