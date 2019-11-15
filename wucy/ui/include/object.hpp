/*
 * object.hpp
 *
 *  Created on: 2019-06-17
 *      Author: lukas.neverauskis
 */
/*
	Software for project WUCY - wearable open source personal computer.
	based on ESP32 running FreeRTOS on custom Therram kernel.
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


#ifndef LUWALIB_LUWA_DISPLAY_LUWA_GUI_OBJECTS_H_
#define LUWALIB_LUWA_DISPLAY_LUWA_GUI_OBJECTS_H_

#include <cstdio>
#include <vector>
#include <iterator>
#include <string>
#include "window.hpp"


/*======= WINDOW DEFINITIONS ========*/

/* todo might be unnecessary */
#define OPTION_TITLE_SIZE 			40
#define OPT_CONFIG_END 				0xFFFFFFFF
#define OPT_ERROR 					0xFFFFFFFE

typedef int16_t coord_t;
typedef uint16_t position_t;
typedef uint32_t opt_id_t;

typedef enum {TRIGGER_RESET_FALSE = 0, TRIGGER_RESET_TRUE = 1 }trigger_reset_e;
typedef enum { HIDE_FALSE = 0, HIDE_TRUE }option_hide_e;
typedef enum { SKIP_HIDDEN_FALSE = 0, SKIP_HIDDEN_TRUE }skip_hid_e;

constexpr opt_id_t id_ILLEGAL() { return (((uint64_t)1 << sizeof(opt_id_t) * 8) - 1); }; /* Max value 0xFFF...F */


class Option {

private:

protected:

public:

	opt_id_t 		_id; /* option unit id number returned for external program interaction todo:needs better definition */
	std::string  	_title; /* option unit title */
	void 			(*_callback)(void * p); /* callback function for custom interaction */
	option_hide_e 	_hide; /* hides option from user visible list */

	Option(opt_id_t id, const std::string title, void (*callback)(void * p)) :

		_id(id),
		_callback(callback),
		_hide(HIDE_FALSE)

	{
		_title.assign(title);
	}


	~Option() {



	}

};



class OptionList : public Window {

private:

	std::vector<Option *> _optVec;

	bool 			_active,
					_showSelectMark,
					_selectTriggered; /* flag indicating if selection has been triggered */;

	option_hide_e 	_hideTitle,
					_hideFrame;

	c_hex_t 		_colorWndBg,
					_colorWndFrame,
					_colorOptTxt,
					_colorOptFocusTxt,
					_colorOptFocusTxtHighlight,
					_colorOptSelectMark;

	const GFXfont * _font;

	opt_id_t 		_optCursor,/* focus cursor option on the list */
					_optSelected, /* Selected option */
					_optScrollBias; /* bias option from top of the linked list. Enables list scrolling */

	std::string 	_title; /* title char array */


	uint8_t 		_maxVisibleOpts;

	static void redraw(void * p);

	uint8_t indexOfOption(opt_id_t id, uint32_t& index);

	int8_t nextOption(opt_id_t thisid, opt_id_t& nextid, skip_hid_e skip);
	int8_t prevOption(opt_id_t thisid, opt_id_t& previd, skip_hid_e skip);

protected:

public:

	OptionList(const GFXfont * f, coord_t xPos, coord_t yPos, coord_t width, coord_t height);
	~OptionList();

	void checkControl();
	void resetList();

	int8_t addOption(position_t pos, opt_id_t id, const std::string, void (*callback)(void * p));
	uint8_t deleteOption(uint32_t id);
	uint8_t execute(opt_id_t id);
	uint8_t findOption(opt_id_t id, Option*& foundOption);


	uint8_t setOptionTitle(opt_id_t id, const std::string);
	uint8_t setCursorAt(opt_id_t id);

	int8_t hideOption(opt_id_t id, option_hide_e state = HIDE_TRUE);
//	uint8_t gotSelected(opt_id_t& selectedOption);

	void setTitle(const std::string title) { _title.assign(title); };
	void hideTitle(option_hide_e state)  { _hideTitle = state; };
	void hideFrame(option_hide_e state)  { _hideFrame = state; };
	void showSelectMark(bool state = true)  { _showSelectMark = state; };

};


#endif /* LUWALIB_LUWA_DISPLAY_LUWA_GUI_OBJECTS_H_ */
