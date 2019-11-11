// -----
// RotaryEncoder.cpp - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 17.06.2015 minor updates.
// -----

#include "RotaryEncoder.h"
#include "wucy_api.h"


// The array holds the values �1 for the entries where a position was decremented,
// a 1 for the entries where the position was incremented
// and 0 in all the other (no change or not valid) cases.

const int8_t KNOBDIR[] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
  -1,  0,  0,  1,
0,  1, -1,  0  };


// positions: [3] 1 0 2 [3] 1 0 2 [3]
// [3] is the positions where my rotary switch detends
// ==> right, count up
// <== left,  count down


// ----- Initialization and Default Values -----

RotaryEncoder::RotaryEncoder(uint32_t pin1, uint32_t pin2) :

		_pin1(pin1),
		_pin2(pin2)

{

  // Setup the input pins and turn on pullup resistor

  wucy_hal_PinInit(_pin1, W_PIN_DIR_INPUT, W_PIN_PULL_UP);
  wucy_hal_PinInit(_pin2, W_PIN_DIR_INPUT, W_PIN_PULL_UP);


  // when not started in motion, the current state of the encoder should be 3
  _oldState = 3;

  // start with position 0;
  _position = 0;
  _positionExt = 0;
  _positionExtPrev = 0;
} // RotaryEncoder()



uint32_t  RotaryEncoder::getPosition() {
  return _positionExt;
} // getPosition()


int8_t  RotaryEncoder::getDirection() {

    int8_t ret = 0;
    
    if( _positionExtPrev > _positionExt )
    {
        ret = -1;
        _positionExtPrev = _positionExt;
    }
    else if( _positionExtPrev < _positionExt )
    {
        ret = 1;
        _positionExtPrev = _positionExt;
    }
    else 
    {
        ret = 0;
        _positionExtPrev = _positionExt;
    }        
    
    return ret;
}



void RotaryEncoder::setPosition(uint32_t newPosition) {
  // only adjust the external part of the position.
  _position = ((newPosition<<2) | (_position & 0x03L));
  _positionExt = newPosition;
  _positionExtPrev = newPosition;
} // setPosition()



void RotaryEncoder::tick(void)
{

  uint8_t sig1 = wucy_hal_PinRead(_pin1);
  uint8_t sig2 = wucy_hal_PinRead(_pin2);


  int8_t thisState = sig1 | (sig2 << 1);

  if (_oldState != thisState) {
    _position += KNOBDIR[thisState | (_oldState<<2)];
    
    if (thisState == LATCHSTATE)
      _positionExt = _position >> 2;
    
    _oldState = thisState;
  } // if
} // tick()

// End
