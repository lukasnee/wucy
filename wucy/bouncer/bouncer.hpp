/*
 * bouncer.hpp
 *
 *  Created on: 2019-11-07
 *      Author: Lukas
 */
/*
	Software for project "WUCY" - wearable open source general-purpose
	computer based on ESP32 running FreeRTOS on custom Therram kernel.
	<https://github.com/therram/wucy>

	License: GPL 3.0

	Copyright (C) 2019 Lukas Neverauskis

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef WUCY_UI_BOUNCER_HPP_
#define WUCY_UI_BOUNCER_HPP_

#include <stdlib.h>
#include <vector>
#include <cmath>


#include "window.hpp"


#define BNC_RAND_RANGE(from, to) 	(from + (rand() % (to + 1)))
#define BNC_COLOR_FROM 				0x000000
#define BNC_COLOR_TO 				0xFFFFFF

#define BNC_POS_TO 					127
#define BNC_LIMIT 					100

#define BNC_PI 						3.14159265



class Bouncer {

public:

	Bouncer(uint8_t width, float speed, uint32_t color, int16_t x, int16_t y, float angle);
	~Bouncer();

	float 			angle, x, y, speed;

	int16_t 		w;
	uint32_t 		color;

};


class BouncerBox : public Window {

public:

	/* add specified amount of bouncer with specic traits */
	int8_t addBouncers(uint8_t amount,
			uint8_t width = 0,
			float speed = 0,
			uint32_t color = 0,
			int16_t x = 0,
			int16_t y = 0,
			float angle = 0);

	/* remove specifiend amount of bouncers */
	int8_t removeBouncers(uint8_t amount);

	/* redraw future frame with given time step */
	void redraw(float timeStep);

	BouncerBox(uint16_t w, uint16_t h, uint16_t x = 0, uint16_t y = 0);
	~BouncerBox();

private:

	void deltaAdvance(Bouncer * bnc, float * coord, float delta);

	uint16_t 	_totalBouncers;
	float 		_time;
	uint8_t 	_ft; /* frame thickness */

	std::vector<Bouncer *> BouncerVec;

};




#endif /* WUCY_UI_BOUNCER_HPP_ */
