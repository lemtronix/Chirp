#include <string.h>
#include "Display.h"
#include "OutputChannel.h"
//#include <stdlib>

//#define DEBUG

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

// Create 5 different outputchannels
OutputChannelClass outputChannel1(1);
OutputChannelClass outputChannel2(2);
OutputChannelClass outputChannel3(3);
OutputChannelClass outputChannel4(4);
OutputChannelClass outputChannel5(5);
  
char inputString[MAX_STRING_LENGTH];
byte stringLength = 0;
boolean stringComplete = false;
unsigned long userInputUL;

#ifdef DEBUG
char debugBuffer[8];
#endif

OutputChannelClass* p_currentChannel;
struct
{
  // version
  
} chripObject;
void setup()
{
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
      else if (strcmp(inputString, "@") == 0)
      {
        Display.displayVersionInfo();
      }
      else if (strcmp(inputString, "#") == 0)
      {
        Display.resetDevice();
      }
      else if (strcmp(inputString, "$") == 0)
      {
        Display.bootLoaderMode();
      }
      else if (strcmp(inputString, "c1") == 0)
      {
#ifdef DEBUG
        Serial.println("Channel 1!");
#endif
        p_currentChannel = &outputChannel1;
      }
      else if (strcmp(inputString, "c2") == 0)
      {
#ifdef DEBUG
        Serial.println("Channel 2!");
#endif
        p_currentChannel = &outputChannel2;
      }
      else if (strcmp(inputString, "c3") == 0)
      {
#ifdef DEBUG
        Serial.println("Channel 3!");
#endif
        p_currentChannel = &outputChannel3;
      }
      else if (strcmp(inputString, "c4") == 0)
      {
#ifdef DEBUG
        Serial.println("Channel 4!");
#endif
        p_currentChannel = &outputChannel4;
      }
      else if (strcmp(inputString, "c5") == 0)
      {
#ifdef DEBUG
        Serial.println("Channel 5!");
#endif        
        p_currentChannel = &outputChannel5;
      }
      else if (strcmp(inputString, "v") == 0)
      {
        Serial.print("Current channel is: ");
        Serial.println(p_currentChannel->getChannelNumber());
      }
      else if (strcmp(inputString, "s") == 0)
      {
        Serial.print("Saving channel ");
        Serial.println(p_currentChannel->getChannelNumber());
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
      else if (strcmp(inputString, "T1") == 0)
      {
        Serial.println("I2C Test Pattern!");
      }
      else if (strcmp(inputString, "T2") == 0)
      {
        Serial.println("SPI Test Pattern!");
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
#ifdef DEBUG
          Serial.print("Channel ");
          Serial.print(p_currentChannel->getChannelNumber());
          Serial.print(" frequency is now ");
          Serial.println(p_currentChannel->getFrequencyHz());
#endif
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
#ifdef DEBUG
          Serial.print("Channel ");
          Serial.print(p_currentChannel->getChannelNumber());
          Serial.print(" amplitude is now ");
          Serial.println(p_currentChannel->getAmplitudeMV());
#endif
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
#ifdef DEBUG
          Serial.print("Channel ");
          Serial.print(p_currentChannel->getChannelNumber());
          Serial.print(" phase is now ");
          Serial.println(p_currentChannel->getPhaseDegrees());
#endif
          menuState = MENU_MAIN;
        }
      }
    }
    
    // Print the command prompt
    Serial.print("c");
    Serial.print(p_currentChannel->getChannelNumber());
    Serial.print("freq");
    Serial.print(p_currentChannel->getFrequencyHz());
    Serial.print("amp");
    Serial.print(p_currentChannel->getAmplitudeMV());
    Serial.print("phase");
    Serial.print(p_currentChannel->getPhaseDegrees());
    p_currentChannel->getOutputStatus() == 1 ? Serial.print("on") : Serial.print("off");
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

// My notes
// DEBUG OUTPUT: ltoa(userInputUL, debugBuffer, 10);
// Serial.println(debugBuffer);
