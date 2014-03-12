#include "Display.h"
#include <avr/pgmspace.h>

DisplayClass Display;

char buffer[32];

char stringHelpMenu_1[] PROGMEM = "Chirp by Lemtronix, LLC.";
char stringHelpMenu_2[] PROGMEM = "------------------------------";
char stringHelpMenu_3[] PROGMEM = "?   Help Menu";
char stringHelpMenu_4[] PROGMEM = "#   Reset Device";
char stringHelpMenu_5[] PROGMEM = "$   Bootloader Mode";
char stringHelpMenu_6[] PROGMEM = "F   Set Frequency";
char stringHelpMenu_7[] PROGMEM = "A   Set Amplitude";
char stringHelpMenu_8[] PROGMEM = "P   Phase Menu";
char stringHelpMenu_9[] PROGMEM = "O/o Turn output (o)ff or (O)n";

PGM_P helpMenu[] PROGMEM = 
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

char stringTestMenu_1[] PROGMEM = "Test Menu:";
char stringTestMenu_2[] PROGMEM = "------------------------------";
char stringTestMenu_3[] PROGMEM = "s   SPI test";
char stringTestMenu_4[] PROGMEM = "i   I2C test";

PGM_P testMenu[] PROGMEM =
{
  stringTestMenu_1,
  stringTestMenu_2,
  stringTestMenu_3,
  stringTestMenu_4
};

const char* testMenu2[] =
{
  "Test Menu",
  "Line 1",
  "Line 2",
  "Line 3"
};
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
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(helpMenu[row])));
    Serial.println(buffer);    // From data memory //Serial.println(helpMenu[row]);
  }
}

void DisplayClass::testMenu()
{
  Serial.println();
  for (byte row=0; row<4; row++)
  {
    //strcpy_P(buffer, (PGM_P)pgm_read_word(&(testMenu[row])));
    //Serial.println(buffer);
    Serial.println(testMenu2[row]);
  }
}

void DisplayClass::resetDevice()
{
  Serial.println("Resetting Device");
}
void DisplayClass::bootLoaderMode()
{
  Serial.println("Entering boot loader mode");
}
void DisplayClass::frequencyMenu()
{
  Serial.println("Frequency Menu");
}
void DisplayClass::amplitudeMenu()
{
  Serial.println("Amplitude Menu");
}
void DisplayClass::phaseMenu()
{
  Serial.println("Phase Menu");
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
