#include <string.h>
#include "Display.h"

#define DEBUG

#define ASCII_NUL      0x00
#define ASCII_BEL      0x07
#define ASCII_BS       0x08
#define ASCII_SPACE    0x20
#define ASCII_DEL      0x7F
#define ASCII_LF       0x0A
#define ASCII_CR       0x0D
#define ASCII_ESC      0x1B

#define MAX_STRING_LENGTH  8

/// \brief Keeps the location of the display menu
typedef enum
{
  MENU_MAIN,
  MENU_SUB_FREQUENCY,
  MENU_SUB_AMPLITUDE,
  MENU_SUB_PHASE,
  MENU_SUB_TEST
} MENU_STATE_T;

MENU_STATE_T menuState;

char inputString[MAX_STRING_LENGTH];
byte stringLength = 0;
boolean stringComplete = false;

/// \brief object for storing information about an output channel
typedef struct
{
  unsigned char channelNumber; //!< channel number {1..5}
  unsigned long frequencyHz;   //!< frequency in hz {1..8MHz}
  unsigned int amplitudeMV;   //!< magnitude of output in mV {100..4000mV}
  unsigned int phaseDegrees;  //!< phase angle {0..359degrees}
  boolean outputStatus;  //!< off or on
} OUTPUT_T;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial)
  {
    // Wait here if something else is already using the serial connection
  }
  
  // Load the main menu by default
  menuState = MENU_MAIN;
  // Display.mainMenu(); // Could display mainMenu here
}

void loop()
{

  if (stringComplete == true)
  {
    // Only executes this when a new string is received from the terminal
#ifdef DEBUG
    Serial.print("\nCommand Received: ");
    Serial.println(inputString);
#else
    Serial.println();
#endif

    if (menuState == MENU_MAIN)
    {
      if (strcmp(inputString, "?") == 0)
      {
        Display.mainMenu();
      }
      else if (strcmp(inputString, "#") == 0)
      {
        Display.resetDevice();
      }
      else if (strcmp(inputString, "$") == 0)
      {
        Display.bootLoaderMode();
      }
      else if (strcmp(inputString, "F") == 0)
      {
        Display.frequencyMenu();
        menuState = MENU_SUB_FREQUENCY;
      }
      else if (strcmp(inputString, "A") == 0)
      {
        Display.amplitudeMenu();
        menuState = MENU_SUB_AMPLITUDE;
      }
      else if (strcmp(inputString, "P") == 0)
      {
        Display.phaseMenu();
        menuState = MENU_SUB_PHASE;
      }
      else if (strcmp(inputString, "o") == 0)
      {
        Display.outputOff();
      }
      else if (strcmp(inputString, "O") == 0)
      {
        Display.outputOn();
      }
      else if (strcmp(inputString, "t") == 0)
      {
        Display.testMenu();
        menuState = MENU_SUB_TEST;
      }
      else
      {
        Display.invalidSelection();
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
    }
    else if (menuState == MENU_SUB_FREQUENCY)
    {
      if (strcmp(inputString, "w") == 0)
      {
        Serial.println("You in are in the frequency menu");
      }
      else if (strcmp(inputString, "x") == 0)
      {
        Serial.println("Main Menu:");
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
      else
      {
        Display.invalidSelection();
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
    }
    else if (menuState == MENU_SUB_AMPLITUDE)
    {
      if (strcmp(inputString, "w") == 0)
      {
        Serial.println("You in are in the amplitude menu");
      }
      else if (strcmp(inputString, "x") == 0)
      {
        Serial.println("Main Menu:");
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
      else
      {
        Display.invalidSelection();
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
    }
    else if (menuState == MENU_SUB_PHASE)
    {
      if (strcmp(inputString, "w") == 0)
      {
        Serial.println("You in are in the phase menu");
      }
      else if (strcmp(inputString, "x") == 0)
      {
        Serial.println("Main Menu:");
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
      else
      {
        Display.invalidSelection();
        Display.mainMenu();
      }
    }
    else if (menuState == MENU_SUB_TEST)
    {
      if (strcmp(inputString, "w") == 0)
      {
        Serial.println("Test menu, bro.");
      }
      else if (strcmp(inputString, "s") == 0)
      {
        Serial.println("SPI test activated!");
      }
      else if (strcmp(inputString, "i") == 0)
      {
        Serial.println("I2C test activated!");
      }
      else if (strcmp(inputString, "x") == 0)
      {
        Serial.println("Main Menu:");
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
      else
      {
        Display.invalidSelection();
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
    }
    
    Serial.println(">");
    // Clear the string and reset the counters
    memset(&inputString, ASCII_NUL, MAX_STRING_LENGTH);
    stringLength = 0;
    stringComplete = false;
  }
}

void serialEvent()
{
  while (Serial.available())
  {
    // get the new char:
    char incomingChar = (char)Serial.read();
    
    if ((incomingChar == ASCII_CR) || (incomingChar == ASCII_LF))
    {
      stringComplete = true;
    }
    else if ((incomingChar == ASCII_BS) || (incomingChar == ASCII_DEL))
    {
      if (stringLength)
      {
        stringLength--;
        inputString[stringLength] = ASCII_NUL;
        Serial.write(ASCII_DEL);
      }
      else
      {
        // If no data in the buffer, make the bell sound
        Serial.write(ASCII_BEL);
      }
    }
    else if (incomingChar == ASCII_ESC)
    {
      // ESC key means the user wants to back up right now
      inputString[stringLength] = incomingChar;
      stringComplete = true;
    }
    else
    {
      if (stringLength < MAX_STRING_LENGTH)
      {
        // All other characters by adding it to the string, incrementing the counter, and echoing the character
        inputString[stringLength] = incomingChar;
        stringLength++;
        Serial.write(incomingChar);
      }
      else
      {
        // Buffer at limit, sound the bell
        Serial.write(ASCII_BEL);
      }
    }
  }
}


