#include <string.h>
#include <SPI.h> // used in DDS.cpp
#include <Wire.h> // used in Rpot.cpp
#include "Display.h"
#include "OutputChannel.h"
#include "DDS.h" // used by OutputChannel.cpp
#include "Rpot.h" // used by OutputChannel.cpp
//#include <stdlib> // used with atoi, atol
#include "Debug.h"
#define DEBUG_OUTPUT 1

#define ASCII_NUL      0x00
#define ASCII_BEL      0x07
#define ASCII_BS       0x08
#define ASCII_SPACE    0x20
#define ASCII_DEL      0x7F
#define ASCII_LF       0x0A
#define ASCII_CR       0x0D
#define ASCII_ESC      0x1B

#define MAX_STRING_LENGTH  8

/// @brief Keeps the location of the display menu
typedef enum
{
  MENU_MAIN,
  MENU_SUB_FREQUENCY,
  MENU_SUB_AMPLITUDE,
  MENU_SUB_PHASE,
  MENU_SUB_TEST
} MENU_STATE_T;

MENU_STATE_T menuState;

// Create 5 different outputchannels
OutputChannelClass outputChannel1(1);

#ifdef MULTICHANNEL
  OutputChannelClass outputChannel2(2);
  OutputChannelClass outputChannel3(3);
  OutputChannelClass outputChannel4(4);
  OutputChannelClass outputChannel5(5);
#endif // MULTICHANNEL

char inputString[MAX_STRING_LENGTH];
byte stringLength = 0;
boolean stringComplete = false;
unsigned long userInputUL;

const uint8_t muxPin = 3;

OutputChannelClass* p_currentChannel;

struct
{
  // version
} chripObject;


void setup()
{
  pinMode(muxPin, OUTPUT);
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial)
  {
    // Wait here if something else is already using the serial connection
  }
  
  // Start with channel1 as the default channel
  p_currentChannel = &outputChannel1;
  
  // Load the main menu by default
  menuState = MENU_MAIN;
  
  // Could display mainMenu here
  // Display.mainMenu();
  
  // This only needs to be called once, even if there are more than one output channel
  p_currentChannel->init();
}

void loop()
{

  if (stringComplete == true)
  {
    // Only executes this when a new string is received from the terminal
    DEBUG("\nCommand Received: ");
    DEBUGLN(inputString);

    if (menuState == MENU_MAIN)
    {
      if (strcmp(inputString, "?") == 0)
      {
        Display.mainMenu();
      }
      else if (strcmp(inputString, "@") == 0)
      {
        Display.displayVersionInfo();
      }
      else if (strcmp(inputString, "#") == 0)
      {
        Display.resetDevice();
        DDS.reset();
      }
      else if (strcmp(inputString, "$") == 0)
      {
        Display.bootLoaderMode();
      }
      else if (strcmp(inputString, "s") == 0)
      {
        Serial.println("Saving channel...");
      }
      else if (strcmp(inputString, "v") == 0)
      {
        Serial.print("Frequency: ");
        Serial.println(p_currentChannel->getFrequencyHz());
        Serial.print("Amplitude: ");
        Serial.println(p_currentChannel->getAmplitudeMV());
        Serial.print("Phase: ");
        Serial.println(p_currentChannel->getPhaseDegrees());
        Serial.print("Output: ");
        p_currentChannel->getOutputStatus() == ON ? Serial.println("On") : Serial.println("Off");
      }
      else if (strcmp(inputString, "f") == 0)
      {
        Display.frequencyMenu();
        menuState = MENU_SUB_FREQUENCY;
      }
      else if (strcmp(inputString, "a") == 0)
      {
        Display.amplitudeMenu();
        menuState = MENU_SUB_AMPLITUDE;
      }
      else if (strcmp(inputString, "p") == 0)
      {
        Display.phaseMenu();
        menuState = MENU_SUB_PHASE;
      }
      else if (strcmp(inputString, "o") == 0)
      {
        p_currentChannel->setOutputStatus(OFF);
        Display.outputOff();
      }
      else if (strcmp(inputString, "O") == 0)
      {
        p_currentChannel->setOutputStatus(ON);
        Display.outputOn();
      }
      else if (strcmp(inputString, "ms") == 0)
      {
        Serial.println("Chirp sine");
        DDS.setOutputMode(DDS_MODE_SINE);
      }
      else if (strcmp(inputString, "mt") == 0)
      {
        Serial.println("Chirp triangle");
        DDS.setOutputMode(DDS_MODE_TRIANGLE);
      }
      else if (strcmp(inputString, "msq") == 0)
      {
        Serial.println("Chirp square");
        DDS.setOutputMode(DDS_MODE_SQUARE);
      }
      else if (strcmp(inputString, "msq2") == 0)
      {
        Serial.println("Chirp square div2");
        DDS.setOutputMode(DDS_MODE_SQUARE_DIV2);
      }
      else if (strcmp(inputString, "m1") == 0)
      {
        Serial.println("Chirp NC filtered");
        digitalWrite(muxPin, LOW);
      }
      else if (strcmp(inputString, "m2") == 0)
      {
        Serial.println("Chirp NO not filtered");
        digitalWrite(muxPin, HIGH);
      }
      else
      {
        Display.invalidSelection();
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
    } // IF
    else if (menuState == MENU_SUB_FREQUENCY)
    {
      if (strcmp(inputString, "x") == 0 || strcmp(inputString, "") == 0)
      {
        // Exit menu if an 'x' is sent or enter key only
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
      else
      {
        // 1. Convert ascii to integer
        userInputUL = (unsigned long)atol(inputString);
        
        // 2. pass the value to the output channel
        if (p_currentChannel->setFrequencyHz(userInputUL))
        {
          //indicate ERROR and display retry message
          Serial.println("An error has occurred, please try again");
        }
        else
        {
          // success, frequency is already set
          menuState = MENU_MAIN;
        }
      }
    }
    else if (menuState == MENU_SUB_AMPLITUDE)
    {
      if (strcmp(inputString, "x") == 0 || strcmp(inputString, "") == 0)
      {
        // Exit menu if an 'x' is sent or enter key only
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
      else
      {
        // 1. Convert ascii to integer
        userInputUL = (unsigned long)atol(inputString);
        
        // 2. pass the value to the output channel
        if (p_currentChannel->setAmplitudeMV(userInputUL))
        {
          //indicate ERROR and display retry message
          Serial.println("An error has occurred, please try again");
        }
        else
        {
          // success, amplitude is already set
          DEBUG("Channel ");
          DEBUG(p_currentChannel->getChannelNumber());
          DEBUG(" amplitude is now ");
          DEBUGLN(p_currentChannel->getAmplitudeMV());
          // @todo create RPOT.sendAmplitude() function
          menuState = MENU_MAIN;
        }
      }
    }
    else if (menuState == MENU_SUB_PHASE)
    {
      if (strcmp(inputString, "x") == 0 || strcmp(inputString, "") == 0)
      {
        // Exit menu if an 'x' is sent or enter key only
        Display.mainMenu();
        menuState = MENU_MAIN;
      }
      else
      {
        // 1. Convert ascii to integer
        userInputUL = (unsigned int)atoi(inputString);
        
        // 2. pass the value to the output channel
        if (p_currentChannel->setPhaseDegrees(userInputUL))
        {
          //indicate ERROR and display retry message
          Serial.println("An error has occurred, please try again");
        }
        else
        {
          // success
          DEBUG("Channel ");
          DEBUG(p_currentChannel->getChannelNumber());
          DEBUG(" phase is now ");
          DEBUGLN(p_currentChannel->getPhaseDegrees());

          menuState = MENU_MAIN;
        }
      }
    }
    
    // Print the command prompt
    /// @todo make it so that the command prompt doesn't print when setting 
    Serial.print("F");
    Serial.print(p_currentChannel->getFrequencyHz());
    Serial.print("A");
    Serial.print(p_currentChannel->getAmplitudeMV());
    Serial.print("P");
    Serial.print(p_currentChannel->getPhaseDegrees());
    p_currentChannel->getOutputStatus() == ON ? Serial.print("on") : Serial.print("off");
    Serial.print(">");
    
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

// Debug Notes
// char debugBuffer[8];
// DEBUG OUTPUT: ltoa(userInputUL, debugBuffer, 10);
// Serial.println(debugBuffer);

