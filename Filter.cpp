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
