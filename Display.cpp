#include "Display.h"
#include <avr/pgmspace.h>

DisplayClass Display;

// typedef char PROGMEM prog_char
// #define PGM_P const prog_char *
prog_char stringHelpMenu_1[] PROGMEM = "Chirp by Lemtronix, LLC.";
prog_char stringHelpMenu_2[] PROGMEM = "------------------------------";
prog_char stringHelpMenu_3[] PROGMEM = "?   Help Menu";
prog_char stringHelpMenu_4[] PROGMEM = "#   Reset Device";
prog_char stringHelpMenu_5[] PROGMEM = "$   Bootloader Mode";
prog_char stringHelpMenu_6[] PROGMEM = "F   Set Frequency";
prog_char stringHelpMenu_7[] PROGMEM = "A   Set Amplitude";
prog_char stringHelpMenu_8[] PROGMEM = "P   Set Phase";
prog_char stringHelpMenu_9[] PROGMEM = "O/o Turn output (o)ff or (O)n";

PROGMEM const char* helpMenu[] = 
{
  stringHelpMenu_1,
  stringHelpMenu_2,
  stringHelpMenu_3,
  stringHelpMenu_4,
  stringHelpMenu_5,
  stringHelpMenu_6,
  stringHelpMenu_7,
  stringHelpMenu_8,
  stringHelpMenu_9
};

prog_char stringTestMenu_1[] PROGMEM = "i   I2C test";
prog_char stringTestMenu_2[] PROGMEM = "s   SPI test";
prog_char stringTestMenu_3[] PROGMEM = "x   exit (no change)";

PROGMEM const char* subMenuTest[] =
{
  stringTestMenu_1,
  stringTestMenu_2,
  stringTestMenu_3
};

char buffer[32];

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
  for (byte row=0; row<9; row++)
  {
    strcpy_P(buffer, (char*)pgm_read_word(&(helpMenu[row])));
    Serial.println(buffer);    // From data memory //Serial.println(helpMenu[row]);
  }
}

void DisplayClass::testMenu()
{
  Serial.println();
  for (byte row=0; row<3; row++)
  {
    strcpy_P(buffer, (char*)pgm_read_word(&(subMenuTest[row])));
    Serial.println(buffer);    // From data memory //Serial.println(helpMenu[row]);
  }
}

void DisplayClass::resetDevice()
{
  Serial.println("Resetting the device");
}
void DisplayClass::bootLoaderMode()
{
  Serial.println("Entering boot loader mode");
}
void DisplayClass::frequencyMenu()
{
  Serial.println("Enter frequency in Hz {0 to 8000000}:");
}
void DisplayClass::amplitudeMenu()
{
  Serial.println("Enter an amplitude in mV {100 to 4000}:");
}
void DisplayClass::phaseMenu()
{
  Serial.println("Enter a phase angle in degrees {0 to 359}");
}
void DisplayClass::outputOff()
{
  Serial.println("Output Off");
}
void DisplayClass::outputOn()
{
  Serial.println("Output On");
}
void DisplayClass::invalidSelection()
{
  Serial.println("Invalid Selection");
}
