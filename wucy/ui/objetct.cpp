/*
 * objetct.cpp
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

#include "include/object.hpp"
#include "ui.hpp"
#include "wucy_api.h"


/* ================== OPTION LIST START ================== */

#define GUI_OL_MG 1 	/* minimum GUI pixel gap */
#define GUI_OL_TS 20 	/* GUI OPTLIST TITLE SIZE */

/*
 * @brief Finds option from option list by given id.
 *
 * @param id 				wanted option id.
 * @param foundOption 		(output) found option pointer.
 * @param index 			(output) found option index (position in linked list).
 *
 * @return -1: 				Option list does not exist or it is empty.
 * 			0: 				Option was not found after full scan.
 * 			1:				Option found successfully.
 */


OptionList::OptionList(
	const GFXfont * f,
	coord_t xPos,
	coord_t yPos,
	coord_t width,
	coord_t height) :

	Window(width, height, DISP_PIXEL_SIZE, redraw),
	_active(1),
	_showSelectMark(0),
	_selectTriggered(0),

	_hideTitle(HIDE_FALSE),
	_hideFrame(HIDE_FALSE),

	_colorWndBg(COLOR_GREEN_VERY_DARK),
	_colorWndFrame(COLOR_GREEN_DARK),
	_colorOptTxt(COLOR_LIME),
	_colorOptFocusTxt(COLOR_LIME_BRIGHT),
	_colorOptFocusTxtHighlight(COLOR_GREEN),
	_colorOptSelectMark(COLOR_GREEN_DARK),

	_font(f),

	_optCursor(id_ILLEGAL()),
	_optSelected(id_ILLEGAL()),
	_optScrollBias(id_ILLEGAL()),

	_title("\0"),
	_maxVisibleOpts(0)

{

}


OptionList::~OptionList() {

	resetList();

}



void OptionList::checkControl() {

	uint32_t index_cursor, index_bias;

	switch(ROTEN.getDirection()) {

	case -1:

		if(prevOption(_optCursor, _optCursor, SKIP_HIDDEN_TRUE) > -1 ) {

				setRedrawRequest();

		}

		break;

	case 1:

		if(nextOption(_optCursor, _optCursor, SKIP_HIDDEN_TRUE) > -1) {

				setRedrawRequest();

		}

		break;
	}


	if (Button_IsTriggered(&ROTEN_OK, BTN_PRIOR_2)) {

		_optSelected = _optCursor;
		_selectTriggered = true;

		setRedrawRequest();

		execute(_optSelected);

	}

	indexOfOption(_optCursor, index_cursor);
	indexOfOption(_optScrollBias, index_bias);

	/*shift displayed options if cursor higher than optDisplayed*/
	if ((index_cursor < index_bias) &&
			(prevOption(_optScrollBias, _optScrollBias, SKIP_HIDDEN_TRUE) > -1)) {

			setRedrawRequest();

	}
	else if ((index_cursor >= index_bias + _maxVisibleOpts - 3) &&
			(nextOption(_optScrollBias, _optScrollBias, SKIP_HIDDEN_TRUE) > -1)) {

		setRedrawRequest();

	}
}





/*
 * @brief Hides option from the visible list.
 *
 * @param id 				id of option to hide.
 * @param state 			0: show.
 * 							1: hide.
 *
 * @return  0: 				Hidden successfully.
 * 			1: 				Option not found or does not exist.
 */

int8_t OptionList::hideOption(opt_id_t id, option_hide_e state)
{

	Option * hideOpt;

	if(findOption(id, hideOpt)){

		hideOpt->_hide = state;

		return 1;
	}

	return 0;

}

/*
 * @brief Tells if option from option list got selected.
 *
 * @param selectedOption 	(output) selected option id.
 *
 * @return 	0: 				Was not selected.
 * 			1: 				Was selected.
 */

//uint8_t OptionList::gotSelected(opt_id_t& id)
//{
//
//	if(_selectTriggered) {
//
//		_selectTriggered = 0;
//
//		id = _optSelected;
//
//		return true;
//	}
//
//	return false;
//}




void OptionList::resetList(){

	for(auto optCur = _optVec.end(); optCur != _optVec.begin(); --optCur) {

		delete *optCur;

	}

	_optVec.clear();

}



/*
 * retval:
 * -2 - id already exists.
 * -1 - error: unable to alloacate memory for option.
 * 0 - added to given option linked list position successfully
 * 1 - first option added successfuly.
 * 2 - option was successfully pushed back at the end of the list.
 * */
int8_t OptionList::addOption(position_t pos, opt_id_t id, const std::string title, void (*callback)(void * p)){

	/* todo adding new options with higher pos than previous,
	 * doesnt push away them down the list, need to do some kind of neighbour update... */

	auto newOpt = new Option(id, title, callback);

	/* continue if successfully allocated memory for option structure member. */
	if(newOpt != NULL) {

		/* set option as first option if no options already exists. */
		if(_optVec.empty()) {

			_optVec.push_back(newOpt);

			_optCursor = newOpt->_id;
			_optScrollBias = newOpt->_id;

			return 1;

		}
		else if(pos >= _optVec.size()) {

			_optVec.push_back(newOpt);

			return 2;
		}
		else{

			/* check if id does not already exist in the list */
			for(auto optCur = _optVec.cbegin(); optCur != _optVec.cend(); ++optCur) {

				if(newOpt->_id  == (*optCur)->_id) {

					delete newOpt;

					return -2;
				}
			}

			/* place option in the given option list position */

			_optVec.insert(_optVec.begin() + pos, newOpt);
			return 0;
		}
	}

	return -1;

}

uint8_t OptionList::deleteOption(opt_id_t id) {

	Option * opt;
	auto optCur = _optVec.cbegin();
	uint32_t index;

	if(findOption(id, opt)) {

		indexOfOption(id, index);

		_optVec.erase(optCur + index);

		delete opt;

		return 1; /* successfully deleted */

	}
	return 0; /* error: option not found */
}




uint8_t OptionList::execute(opt_id_t id){

	Option * opt;

	if(findOption(id, opt) && opt->_callback != NULL) {

		opt->_callback(this);

		return 1;

	}
	return 0;
}



uint8_t OptionList::setOptionTitle(opt_id_t id, const std::string title) {

	Option * opt;

	if(findOption(id, opt)) {

		opt->_title.assign(title);

		return 1;
	}
	return 0;
}


uint8_t OptionList::setCursorAt(opt_id_t id) {

	Option * opt;

	/* if _id exists */
	if(findOption(id, opt)) {

		_optCursor = id;
		return 1;
	}
	return 0;
}



#define FRAME_THICKNESS (GUI_OL_MG * 2)
void OptionList::redraw(void * p) {

	OptionList * ol = (OptionList*)p;

	int16_t x = FRAME_THICKNESS, y = FRAME_THICKNESS;
	uint16_t w = ol->GetW() - FRAME_THICKNESS * 2 * GUI_OL_MG,
			h = ol->GetH() - FRAME_THICKNESS * 2 * GUI_OL_MG;

	uint8_t listLine = 0; /* visible options' vertical cursor, used to limit drawing inside window. */

/* render window */

	ol->fillAll(ol->_colorWndBg);

	/* calculate how many options can fit in window
	 * at the moment (depends on font's and window's dimensions) */

	ol->setFont(ol->_font);
	ol->setTextSize(1);
	ol->setTextWrap(0);

	/* title height */
	uint8_t heightTitle = ol->_hideTitle ? 0 : ol->getCharMaxHeight();
	ol->_maxVisibleOpts = 2 + (h - heightTitle) / (ol->getCharMaxHeight());

	/* render title */
	if (!ol->_hideTitle) {

		/* draw title box */
		ol->setDrawColor(ol->_colorWndFrame);
		ol->fillRect(x, y, w, ol->getCharMaxHeight());

		/* print title in title bow */
		ol->setTextColor(ol->_colorOptTxt);
		ol->setCursor(x + 2 * GUI_OL_MG, y + heightTitle - 2 * GUI_OL_MG);
		ol->print(ol->_title);

		y += heightTitle - 2 * GUI_OL_MG;

	}

	/* draw option if it is in the visible list range and list has elements*/

	uint32_t scrollbiasIndex;
	ol->indexOfOption(ol->_optScrollBias, scrollbiasIndex);

	for (auto optCur = ol->_optVec.cbegin() + scrollbiasIndex;
			(optCur != ol->_optVec.end()) && (listLine < ol->_maxVisibleOpts);
			++optCur) {

		if((*optCur)->_hide == HIDE_FALSE) {

			/* draw cursor box */
			if ((*optCur)->_id == ol->_optCursor) {

				ol->setDrawColor(ol->_colorOptFocusTxtHighlight);
				ol->fillRect(x, y + 2 * GUI_OL_MG, w, ol->getCharMaxHeight());
			}
			/* draw select marker */

			if (ol->_showSelectMark && (*optCur)->_id == ol->_optSelected) {

				ol->setDrawColor(ol->_colorOptSelectMark);
				ol->fillRect(w - ol->getCharMaxHeight()/3 + 3 * GUI_OL_MG,
					y + 3 * GUI_OL_MG,
					ol->getCharMaxHeight()/3 - 2 * GUI_OL_MG,
					ol->getCharMaxHeight() - 2 * GUI_OL_MG);
			}

			/* draw option title */
			y += ol->getCharMaxHeight();

			ol->setTextColor(ol->_colorOptFocusTxt);
			ol->setCursor(x + 1, y + 1);
			ol->print((*optCur)->_title);



			listLine++;

		}
	}

	ol->setDrawColor(ol->_colorWndFrame);
	ol->drawFrame(FRAME_THICKNESS);

}



uint8_t OptionList::indexOfOption(opt_id_t id, uint32_t& index) {

	if(_optVec.empty()) {

		return -2; /* error: option list is empty */
	}

	auto optCur = _optVec.cbegin();

		for (; optCur != _optVec.cend(); ++optCur) {

			if((*optCur)->_id == id) {

				index = std::distance(_optVec.cbegin(), optCur);

				return 1; /* success: option found */
			}
		}

		index = std::distance(_optVec.cbegin(), optCur); /* returns end of the list (for debugging) */

		return 0; /* error: option not found */

}




uint8_t OptionList::findOption(opt_id_t id, Option*& foundOption) {

	uint32_t index;

	if(indexOfOption(id, index)) {

		auto optCur = _optVec.cbegin();
		optCur += index;

		foundOption = *optCur;

		return 1; /* success: option found */

	}

	foundOption = NULL;

	return 0; /* error: option not found */

}


/*
 * @brief 						Returns next option from the referring option.
 * 								Skips hidden options if parameter "skip" is set to SKIP_HidDEN_TRUE.
 *
 * @param thisid[in] 			reference id.
 * @param nextid[out]			id variable where next id gets placed.
 * @param skip[in] 				configuration flag. Skips hidden options if set to SKIP_HidDEN_TRUE.
 *
 * @return 	-2: 				error:		id deoes not exist.
 * 			-1: 				error: 		there is no previous available option.
 * 			0: 					success: 	previous option.
 */
int8_t OptionList::nextOption(opt_id_t thisid, opt_id_t& nextid, skip_hid_e skip) {

	uint32_t index;

	/* if _id exists */
	if(indexOfOption(thisid, index)) {

		/* increment (to previous) position, check if it is not the end, else cycle forward */
		for (auto optCur = _optVec.cbegin() + ++index; optCur != _optVec.cend(); ++optCur) {

			/* return id of closest forward neighbour that is not hidden */

			if(skip == SKIP_HIDDEN_FALSE || (skip == SKIP_HIDDEN_TRUE && (*optCur)->_hide == false )) {

				nextid = (*optCur)->_id;
				return 0;

			}
		}

		return -1; /* error: there is no previous available option */

	}

	return -2; /* error: _id deoes not exist */

}

/*
 * @brief 						Returns previous option from the referring option.
 * 								Skips hidden options if parameter "skip" is set to SKIP_HidDEN_TRUE.
 *
 * @param thisid[in] 			reference id.
 * @param previd[out]			id variable where previous id gets placed.
 * @param skip[in] 				configuration flag. Skips hidden options if set to SKIP_HidDEN_TRUE.
 *
 * @return 	-2: 				error:		id deoes not exist.
 * 			-1: 				error: 		there is no previous available option.
 * 			0: 					success: 	previous option.
 */
int8_t OptionList::prevOption(opt_id_t thisid, opt_id_t& previd, skip_hid_e skip) {

	uint32_t index;

	/* if _id exists */
	if(indexOfOption(thisid, index)) {

		/* increment (to previous) position, check if it is not the end, else cycle forward */
		for (auto optCur = _optVec.crend() - index; optCur != _optVec.crend(); ++optCur) {

			/* return id of closest forward neighbour that is not hidden */

			if(skip == SKIP_HIDDEN_FALSE || (skip == SKIP_HIDDEN_TRUE && (*optCur)->_hide == false )) {

				previd = (*optCur)->_id;
				return 0;

			}
		}

		return -1; /* error: there is no previous available option */

	}

	return -2; /* error: _id deoes not exist */

}



/* ================== OPTION LIST END ================== */
