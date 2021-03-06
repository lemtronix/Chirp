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
#include <string.h>
#include <SPI.h> // used in DDS.cpp
#include <Wire.h> // used in Amplifier.cpp
#include "Display.h"
#include "OutputChannel.h"
#include "DDS.h" // used by OutputChannel.cpp; TODO move this into output channel only
#include "Amplifier.h" // used by OutputChannel.cpp
#include "Filter.h"
#include "Debug.h"

#define DEBUG_OUTPUT 0

#define ASCII_NUL      0x00
#define ASCII_BEL      0x07
#define ASCII_BS       0x08
#define ASCII_SPACE    0x20
#define ASCII_DEL      0x7F
#define ASCII_LF       0x0A
#define ASCII_CR       0x0D
#define ASCII_ESC      0x1B

#define MAX_STRING_LENGTH  12

/// @brief Keeps the location of the display menu
typedef enum
{
    MENU_MAIN,
    MENU_SUB_FREQUENCY,
    MENU_SUB_AMPLITUDE,
    MENU_SUB_PHASE,
    MENU_SUB_WAVEFORM,
    MENU_SUB_TEST
} MENU_STATE_T;

typedef enum
{
    MENU_RESULT_ERROR,
    MENU_RESULT_SUCCESS,
    MENU_RESULT_EXIT
} MENU_RESULT_T;

void printVerboseStatus(void);
void printStatusLine(void);
MENU_RESULT_T setWaveformMenu(char* waveformToSet);

MENU_STATE_T menuState;

// Create 5 different output channels
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

// TODO rename this variable?
boolean useQuickCommandsOnly = false;   // This suppresses the menu after an invalid selection and does not allow for sub-menus.. Mainly used for advanced users and to simplify the GUI application

// TODO move this into flash
const char* errorSelectionInMenuString = "An error has occurred, please try again";

OutputChannelClass* p_currentChannel;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(57600);
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
    static char firstCharacter[2];
    char* remainingCharacters;

    if (stringComplete == true)
    {
        // Only executes this when a new string is received from the terminal

        if (menuState == MENU_MAIN)
        {
            // Initialize new input string
            remainingCharacters = NULL;
            memset(firstCharacter, ASCII_NUL, sizeof(firstCharacter));

            // Look at the first character from the input string to determine if this is a quick command
            memcpy(firstCharacter, inputString, 1);
            // TODO look into only executing this if the string length is over a certain limit ?2?
            remainingCharacters = strtok(inputString, firstCharacter);

            if ((strcmp(firstCharacter, "?") == 0) || (strstr(inputString, "help") != NULL))
            {
                Display.mainMenu();
            }
            else if (strcmp(firstCharacter, "%") == 0)
            {
                useQuickCommandsOnly = !useQuickCommandsOnly;
            }
            else if (strcmp(firstCharacter, "@") == 0)
            {
                Display.displayVersionInfo();
            }
            else if (strcmp(firstCharacter, "#") == 0)
            {
                Display.resetDevice();
                // TODO update DDS.reset to take a reference to OutputChannelClass* to the reset to clear out the console status
                // or issue a reset to the output channel directly via p_currentChannel->reset())
                DDS.reset();
                p_currentChannel->reset();
            }
            else if (strcmp(firstCharacter, "v") == 0)
            {
                printVerboseStatus();
            }
            else if (strcmp(firstCharacter, "f") == 0)
            {
                if (remainingCharacters != NULL)
                {
                    // Try to send the command as a single string
                    uint32_t frequency = (uint32_t)atol(remainingCharacters);

                    if (p_currentChannel->setFrequencyHz(frequency))
                    {
                        //indicate ERROR and display retry message
                        Serial.println(errorSelectionInMenuString);
                    }
                }
                else if (useQuickCommandsOnly)
                {
                    // The user requested quick commands but did not provide a full command, do NOT show the submenu and error silently
                }
                else
                {
                    // Not a quick command, display the submenu
                    Display.frequencyMenu();
                    menuState = MENU_SUB_FREQUENCY;
                }
            }
            else if (strcmp(firstCharacter, "a") == 0)
            {
                if (remainingCharacters != NULL)
                {
                    // Try to send the command as a single string
                    uint32_t amplitude = (uint32_t)atol(remainingCharacters);

                    if (p_currentChannel->setAmplitudeMV(amplitude))
                    {
                        //indicate ERROR and display retry message
                        Serial.println(errorSelectionInMenuString);
                    }
                }
                else if (useQuickCommandsOnly)
                {
                    // The user requested quick commands but did not provide a full command, do NOT show the submenu and error silently
                }
                else
                {
                    Display.amplitudeMenu();
                    menuState = MENU_SUB_AMPLITUDE;
                }
            }
            else if (strcmp(firstCharacter, "p") == 0)
            {
                if (remainingCharacters != NULL)
                {
                    // Try to send the command as a single string
                    uint32_t phase = (uint32_t)atol(remainingCharacters);

                    if (p_currentChannel->setPhaseDegrees(phase))
                    {
                        //indicate ERROR and display retry message
                        Serial.println(errorSelectionInMenuString);
                    }
                }
                else if (useQuickCommandsOnly)
                {
                    // The user requested quick commands but did not provide a full command, do NOT show the submenu and error silently
                }
                else
                {
                    Display.phaseMenu();
                    menuState = MENU_SUB_PHASE;
                }
            }
            else if (strcmp(firstCharacter, "w") == 0)
            {
                if (remainingCharacters != NULL)
                {
                    if (setWaveformMenu(remainingCharacters) != MENU_RESULT_SUCCESS)
                    {
                        // If there was an error with the quick command, then bring up the waveform menu
                        Display.waveformMenu();
                        menuState = MENU_SUB_WAVEFORM;
                    }
                }
                else if (useQuickCommandsOnly)
                {
                    // The user requested quick commands but did not provide a full command, do NOT show the submenu and error silently
                }
                else
                {
                    // Otherwise, only one character typed
                    Display.waveformMenu();
                    menuState = MENU_SUB_WAVEFORM;
                }
            }
            else if (strcmp(firstCharacter, "o") == 0)
            {
                p_currentChannel->setOutputStatus(OFF);
            }
            else if (strcmp(firstCharacter, "O") == 0)
            {
                p_currentChannel->setOutputStatus(ON);
            }
            else if (strcmp(firstCharacter, "d") == 0)
            {
                Serial.println(F("DAC filter disabled"));
                Filter.off();
            }
            else if (strcmp(firstCharacter, "D") == 0)
            {
                Serial.println(F("DAC filter enabled"));
                Filter.on();
            }
            else if (strcmp(inputString, "rs") == 0)
            {
                Serial.println(F("Amplifier Status:"));
                Amplifier.printStatus();
            }
            else if (strcmp(inputString, "rt") == 0)
            {
                Serial.println(F("Amplifier Tcon Status:"));
                Amplifier.printTcon();
            }
            else if (strcmp(inputString, "r0") == 0)
            {
                Serial.println(F("Amplifier1 Value:"));
                Amplifier.printPotValue(0);
            }
            else if (strcmp(inputString, "r1") == 0)
            {
                Serial.println(F("Amplifier2 Value:"));
                Amplifier.printPotValue(1);
            }
            else
            {
                // Invalid selection
                Display.invalidSelection();
                menuState = MENU_MAIN;

                if (useQuickCommandsOnly == false)
                {
                    // Only display the mainmenu after an invalid selection if use quick commands is off
                    Display.mainMenu();
                }
            }
        }
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
                uint32_t userInputUL = (uint32_t) atol(inputString);

                // 2. pass the value to the output channel
                if (p_currentChannel->setFrequencyHz(userInputUL))
                {
                    //indicate ERROR and display retry message
                    Serial.println(errorSelectionInMenuString);
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
                uint32_t userInputUL = (uint32_t) atol(inputString);

                // 2. pass the value to the output channel
                if (p_currentChannel->setAmplitudeMV(userInputUL))
                {
                    //indicate ERROR and display retry message
                    Serial.println(errorSelectionInMenuString);
                }
                else
                {
                    // success, amplitude is already set
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
                uint32_t userInputUL = (uint32_t) atoi(inputString);

                // 2. pass the value to the output channel
                if (p_currentChannel->setPhaseDegrees(userInputUL))
                {
                    //indicate ERROR and display retry message
                    Serial.println(errorSelectionInMenuString);
                }
                else
                {
                    // success
                    DEBUG(F("Channel "));
                    DEBUG(p_currentChannel->getChannelNumber());
                    DEBUG(F(" phase is now "));
                    DEBUGLN(p_currentChannel->getPhaseDegrees());

                    menuState = MENU_MAIN;
                }
            }
        }
        else if (menuState == MENU_SUB_WAVEFORM)
        {
            // See if this was a quick character
            MENU_RESULT_T menuResult = setWaveformMenu(inputString);

            switch (menuResult)
            {
                case MENU_RESULT_SUCCESS:
                    menuState = MENU_MAIN;
                break;
                case MENU_RESULT_EXIT:
                    menuState = MENU_MAIN;
                break;
                case MENU_RESULT_ERROR:
                    Serial.println(errorSelectionInMenuString);
                    Display.waveformMenu();
                    menuState = MENU_SUB_WAVEFORM;
                break;
                default:
                    DEBUGLN(F("Option does not exist!"));
                break;
            }
        }
        else
        {
            // No item found
        }

        // Print the command prompt
        printStatusLine();

        // Clear the string and reset the counters
        memset(&inputString, ASCII_NUL, MAX_STRING_LENGTH);
        stringLength = 0;
        stringComplete = false;
    }
}

void printVerboseStatus(void)
{
    Serial.print(F("Quick Commands Only: "));
    useQuickCommandsOnly == true ? Serial.println(F("Yes")) : Serial.println(F("No"));
    Serial.print(F("Frequency: "));
    Serial.println(p_currentChannel->getFrequencyHz());
    Serial.print(F("Amplitude: "));
    Serial.println(p_currentChannel->getAmplitudeMV());
    Serial.print(F("Phase: "));
    Serial.println(p_currentChannel->getPhaseDegrees());
    Serial.print(F("Output: "));
    (p_currentChannel->getOutputStatus() == ON) ? Serial.println(F("On")) : Serial.println(F("Off"));
}

// Print the command prompt in the form WAVEFORM:F#A#P#_OUTPUT>
void printStatusLine(void)
{
    if (useQuickCommandsOnly)
    {
        // Write a ! first if quick commands only is set
        Serial.write('Q');
        Serial.write('_');
    }

    Serial.print(p_currentChannel->getWaveform());
    Serial.write('_');
    Serial.write('F');
    Serial.print(p_currentChannel->getFrequencyHz());
    Serial.write('_');
    Serial.write('A');
    Serial.print(p_currentChannel->getAmplitudeMV());
    Serial.write('_');
    Serial.write('P');
    Serial.print(p_currentChannel->getPhaseDegrees());
    Serial.write('_');
    (p_currentChannel->getOutputStatus() == ON) ? Serial.print("ON") : Serial.print("OFF");
    Serial.write('>');
}

MENU_RESULT_T setWaveformMenu(char* waveformToSet)
{
    MENU_RESULT_T wasAbleToSetWaveform = MENU_RESULT_ERROR;

    if (strcmp(waveformToSet, "x") == 0 || strcmp(waveformToSet, "") == 0)
    {
        // Exit menu if an 'x' is sent or enter key only
        wasAbleToSetWaveform = MENU_RESULT_EXIT;
    }
    else if (strstr(waveformToSet, "sin") != NULL)
    {
        // Sine wave
        DEBUGLN(F("Chirp sine"));
        p_currentChannel->setWaveform(WAVEFORM_SINE);
        wasAbleToSetWaveform = MENU_RESULT_SUCCESS;
    }
    else if (strstr(waveformToSet, "tri") != NULL)
    {
        // Triangle wave
        DEBUGLN(F("Chirp triangle"));
        p_currentChannel->setWaveform(WAVEFORM_TRIANGLE);
        wasAbleToSetWaveform = MENU_RESULT_SUCCESS;
    }
    else if ((strstr(waveformToSet, "sq") != NULL) && (strstr(waveformToSet, "2") != NULL))
    {
        // Square wave divide by 2
        DEBUGLN(F("Chirp square div2"));
        p_currentChannel->setWaveform(WAVEFORM_SQUARE_DIV_2);
        wasAbleToSetWaveform = MENU_RESULT_SUCCESS;
    }
    else if (strstr(waveformToSet, "sq") != NULL)
    {
        // Square wave
        DEBUGLN(F("Chirp square"));
        p_currentChannel->setWaveform(WAVEFORM_SQUARE);
        wasAbleToSetWaveform = MENU_RESULT_SUCCESS;
    }
    else
    {
        wasAbleToSetWaveform = MENU_RESULT_ERROR;
    }

    return wasAbleToSetWaveform;
}
void serialEvent()
{
    while (Serial.available())
    {
        // get the new char:
        char incomingChar = (char) Serial.read();

        if ((incomingChar == ASCII_CR) || (incomingChar == ASCII_LF))
        {
            stringComplete = true;
            Serial.print("\n\r");
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

