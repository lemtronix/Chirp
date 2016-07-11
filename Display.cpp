#include "Display.h"
#include "Arduino.h"
#include <avr/pgmspace.h>

DisplayClass Display;

#define HELP_MENU_ROW_MAX  14

const char stringHelpMenu_1[] PROGMEM   = "Chirp by Lemtronix, LLC.";
const char stringHelpMenu_2[] PROGMEM   = "------------------------------";
const char stringHelpMenu_3[] PROGMEM   = "?   Help Menu";
const char stringHelpMenu_4[] PROGMEM   = "@   Version information";
const char stringHelpMenu_5[] PROGMEM   = "#   Reset Device";
const char stringHelpMenu_6[] PROGMEM   = "$   Bootloader Mode (dep)";
const char stringHelpMenu_7[] PROGMEM   = "v   View current output settings";
const char stringHelpMenu_8[] PROGMEM   = "s   Save output channel (dep)";
const char stringHelpMenu_9[] PROGMEM   = "f   Set Frequency";
const char stringHelpMenu_10[] PROGMEM  = "a   Set Amplitude";
const char stringHelpMenu_11[] PROGMEM  = "p   Set Phase";
const char stringHelpMenu_12[] PROGMEM  = "w   Set Waveform";
const char stringHelpMenu_13[] PROGMEM  = "o/O Turn output (o)ff or (O)n";
const char stringHelpMenu_14[] PROGMEM  = "d/D Turn DAC filter off or on (dep)";

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
  stringHelpMenu_13,
  stringHelpMenu_14,
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
void DisplayClass::bootLoaderMode()
{
  print_P(PSTR("Entering boot loader mode"));
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

