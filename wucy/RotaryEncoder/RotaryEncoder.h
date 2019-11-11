// -----
// RotaryEncoder.h - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 16.06.2019 pin initialization using INPUT_PULLUP
// -----

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include <cstdio>



#define LATCHSTATE 3

class RotaryEncoder
{
public:
  // ----- Constructor -----
  RotaryEncoder(uint32_t pin1, uint32_t pin2);

  // retrieve the current position
  uint32_t  getPosition();
  
  // simple retrieve of the direction the knob was rotated at. 0 = No rotation, 1 = Clockwise, -1 = Counter Clockwise
  int8_t getDirection();

  // adjust the current position
  void setPosition(uint32_t newPosition);

  // call this function every some milliseconds or by using an interrupt for handling state changes of the rotary encoder.
  void tick(void);

private:

  uint32_t _pin1, _pin2; // Arduino pins used for the encoder.


  volatile int8_t _oldState;
  
  volatile uint32_t _position;         // Internal position (4 times _positionExt)
  volatile uint32_t _positionExt;      // External position
  volatile uint32_t _positionExtPrev;  // External position (used only for direction checking)

};

#endif

// End
