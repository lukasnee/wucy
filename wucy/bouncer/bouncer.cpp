/*
 * bouncer.cpp
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

#include "bouncer.hpp"
#include "wucy_api.h"

/*
 * @brief calculates single axis position by given step regarding edges of screen.
 *
 * @param coord[in] 	current axis position.
 * @param delta[in] 	distance to advance regarding if it's positive or negative.
 * @param width[in] 	width of bouncer block. Needed for proper
 * 						calculation keeping bouncer in screen.
 *
 *
 * @return 				New axis position of given coord.
 * */
void BouncerBox::deltaAdvance(Bouncer * bnc, float * coord, float delta) {


}



static void wnd_draw_bounceBox(void * p) {

	BouncerBox * bnc = (BouncerBox*)p; /* todo not sure if so*/

	bnc->redraw(1);

}


/* BounceBox class */

BouncerBox::BouncerBox(uint16_t w, uint16_t h, uint16_t x, uint16_t y) :

Window(w, h, DISP_PIXEL_SIZE, wnd_draw_bounceBox),
	_totalBouncers(0),
	_time(0),
	_ft(2)

{



}



BouncerBox::~BouncerBox() {



}



void BouncerBox::redraw(float timeStep) {

	float step, dx, dy, edge_up, edge_down, edge_left, edge_right;

	Bouncer * bnc;

	_time += timeStep;

	setDrawColor(COLOR_BLACK);
	fillRect(0, 0, GetW(), GetH());
	setDrawColor(COLOR_NAVY);
	drawFrame(2);

	for (auto bncCur = BouncerVec.cbegin(); bncCur != BouncerVec.cend(); ++bncCur) {

		bnc = *bncCur;


		/* calculate step which depends on speed of the bouncer block and time passed */
		step = timeStep * bnc->speed;

		/* step in x axis */
		dx = step/cos(bnc->angle);
		/* step in y axis */
		dy = step/sin(bnc->angle);

		edge_up = _ft;
		edge_down = GetH() - 1 - bnc->w;
		edge_left = _ft;
		edge_right = GetW() - 1 - bnc->w;

		while(dx != 0 || dy != 0) {

			if(bnc->x + dx > edge_right) {

				bnc->angle = (dy > 0.00 ? -1.00 : 1.00) * 2.00 * fmod(bnc->angle, (BNC_PI/2));

				dx -= edge_right - bnc->x;
				dx = -dx;
				bnc->x = edge_right - 1;

			}
			else if(bnc->y + dy > edge_down) {

				bnc->angle = (dx > 0.00  ? 1.00 : -1.00) * 2.00 * fmod(bnc->angle, (BNC_PI/2));

				dy -= edge_down - bnc->y;
				dy = -dy;
				bnc->y = edge_down -1;

			}

			else if(bnc->x + dx < edge_left) {

				bnc->angle = (dy > 0.00 ? 1.00 : -1.00) * 2.00 * fmod(bnc->angle, (BNC_PI/2));

				dx += bnc->x - edge_left;
				dx = -dx;
				bnc->x = edge_left +1 ;

			}

			else if(bnc->y + dy < edge_up) {

				bnc->angle = (dx > 0.00  ? -1.00 : 1.00) * 2.00 * fmod(bnc->angle, (BNC_PI/2));

				dy += bnc->y - edge_up;
				dy = -dy;
				bnc->y = edge_up +1;

			}
			else {

				bnc->x += dx;
				dx = 0;
				bnc->y += dy;
				dy = 0;
			}
		}

		/* draw box at calculated position */
		setDrawColor((c_hex_t)bnc->color);
		drawRect(bnc->x, bnc->y, bnc->w, bnc->w);
		setDrawColor((c_hex_t)(bnc->color^0xFFFFFF));
		fillRect(bnc->x+1, bnc->y+1, bnc->w-2, bnc->w-2);

	}
}



int8_t BouncerBox::addBouncers(

		uint8_t amount,
		uint8_t width,
		float speed,
		uint32_t color,
		int16_t x,
		int16_t y,
		float angle)
{

	Bouncer * bnc;

	for(uint8_t i = 0; i < amount; i++) {

		bnc = new Bouncer(width, speed, color, x, y , angle);

		if(bnc == NULL) {

			return -1; /* error: unable to allocate any more bouncers */

		}

		BouncerVec.push_back(bnc);
		_totalBouncers++;
	}

	return 1; /* success: added given amount of bouncer */
}



int8_t BouncerBox::removeBouncers(uint8_t amount) {

	for(uint8_t i = 0; i < amount; i++) {

		if(BouncerVec.empty()) {

			return 2;

		}

		BouncerVec.pop_back();
		_totalBouncers--;

	}

	return 1;

}





/* Bouncer class */


Bouncer::Bouncer(
		uint8_t width,
		float speed,
		uint32_t color,
		int16_t x,
		int16_t y,
		float angle
		)
{

	this->speed = 	speed ? speed : (float)BNC_RAND_RANGE(0, 100)/100.0;

	this->w = 		width ? width : BNC_RAND_RANGE(4, 6);
	this->color = 	color ? color : (0xFF000000 | BNC_RAND_RANGE(BNC_COLOR_FROM, BNC_COLOR_TO));

	this->x = 		x ? x : BNC_RAND_RANGE(0, BNC_POS_TO);
	this->y = 		y ? y : BNC_RAND_RANGE(0, BNC_POS_TO);

	this->angle = 	angle ? fmod(angle, 6.28) : (float)BNC_RAND_RANGE(0, 628)/100;

}



Bouncer::~Bouncer() {



}



