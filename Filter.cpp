/*
    This file is part of Chirp.

    Chirp is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    Chirp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Chirp.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2016 Mike Lemberger
*/
#include "Filter.h"
#include "Arduino.h"
#include "Debug.h"

#define DEBUG_OUTPUT 1

const uint8_t muxSelectLine = 2;

FilterClass Filter;

FilterClass::FilterClass()
{
  // Enable the line to the Filter as an output and turn the filter off
  pinMode(muxSelectLine, OUTPUT);
  off();
}

FilterClass::~FilterClass()
{
}

void FilterClass::on()
{
  digitalWrite(muxSelectLine, LOW);
}

void FilterClass::off()
{
  digitalWrite(muxSelectLine, HIGH);
}
