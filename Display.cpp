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
#include "Display.h"
#include "Arduino.h"
#include <avr/pgmspace.h>

DisplayClass Display;

#define HELP_MENU_ROW_MAX  12

const char stringHelpMenu_1[] PROGMEM   = "Chirp - A lightweight function generator";
const char stringHelpMenu_2[] PROGMEM   = "------------------------------";
const char stringHelpMenu_3[] PROGMEM   = "?   Help Menu";
const char stringHelpMenu_4[] PROGMEM   = "@   Version information";
const char stringHelpMenu_5[] PROGMEM   = "#   Reset Device";
const char stringHelpMenu_6[] PROGMEM   = "v   View current output settings";
const char stringHelpMenu_7[] PROGMEM   = "f   Set Frequency";
const char stringHelpMenu_8[] PROGMEM  = "a   Set Amplitude";
const char stringHelpMenu_9[] PROGMEM  = "p   Set Phase";
const char stringHelpMenu_10[] PROGMEM  = "w   Set Waveform";
const char stringHelpMenu_11[] PROGMEM  = "o/O Turn output (o)ff or (O)n";
const char stringHelpMenu_12[] PROGMEM  = "d/D Turn DAC filter off or on (dep)";

//const char stringHelpMenu_7[] PROGMEM   = "c#  Select an output channel {1..5}";
PGM_P const helpMenu[] PROGMEM = 
{
  stringHelpMenu_1,
  stringHelpMenu_2,
  stringHelpMenu_3,
  stringHelpMenu_4,
  stringHelpMenu_5,
  stringHelpMenu_6,
  stringHelpMenu_7,
  stringHelpMenu_8,
  stringHelpMenu_9,
  stringHelpMenu_10,
  stringHelpMenu_11,
  stringHelpMenu_12,
};

char buffer[48];

DisplayClass::DisplayClass(void)
{ 
}

DisplayClass::~DisplayClass()
{
}

void DisplayClass::mainMenu()
{
  // Display menu
  Serial.println();
  for (byte row=0; row<HELP_MENU_ROW_MAX; row++)
  {
    strcpy_P(buffer, (char*)pgm_read_word(&(helpMenu[row])));
    Serial.println(buffer);    // From data memory //Serial.println(helpMenu[row]);
  }
}

void DisplayClass::resetDevice()
{
  print_P(PSTR("Resetting the device..."));
}
void DisplayClass::frequencyMenu()
{
  print_P(PSTR("Enter frequency in Hz {0 to 8000000}:"));
}
void DisplayClass::amplitudeMenu()
{
  print_P(PSTR("Enter an amplitude in mV {100 to 4000}:"));
}
void DisplayClass::phaseMenu()
{
  print_P(PSTR("Enter a phase angle in degrees {0 to 359}"));
}
void DisplayClass::waveformMenu()
{
	print_P(PSTR("Enter a waveform {sine, triangle, square, squarediv2}"));
}
void DisplayClass::outputOff()
{
  print_P(PSTR("Output Off"));
}
void DisplayClass::outputOn()
{
  print_P(PSTR("Output On"));
}
void DisplayClass::displayVersionInfo()
{
  print_P(PSTR("Version ##.##"));
}
void DisplayClass::invalidSelection()
{
  print_P(PSTR("Invalid Selection"));
}

void DisplayClass::print_P(const char* pstr)
{
  uint8_t val;
  
  while (true)
  {
    val = pgm_read_byte(pstr);
    if (!val) break;
    Serial.write(val);
    pstr++;
  }
  
  Serial.write("\r\n");
}

