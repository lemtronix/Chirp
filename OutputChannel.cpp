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
*/

#include "OutputChannel.h"
#include "DDS.h" // used by OutputChannel.cpp
#include "Amplifier.h" // used by OutputChannel.cpp

#include "Debug.h"
#define DEBUG_OUTPUT 1

const char waveformOffString[] = "OFF";
const char waveformSineString[] = "SIN";
const char waveformTriangleString[] = "TRI";
const char waveformSquareString[] = "SQ";
const char waveformSquare2String[] = "SQ2";

OutputChannelClass::OutputChannelClass(unsigned char cNumber)
{
    channelNumber = cNumber;
    frequencyHz = 0;
    amplitudeMV = 0;
    phaseDegrees = 0;
    outputStatus = OFF;
}

OutputChannelClass::~OutputChannelClass()
{
}

void OutputChannelClass::init(void)
{
    DDS.init();
    Amplifier.init();
}

uint8_t OutputChannelClass::getChannelNumber(void)
{
    return channelNumber;
}
uint32_t OutputChannelClass::getFrequencyHz(void)
{
    return frequencyHz;
}
uint16_t OutputChannelClass::getAmplitudeMV(void)
{
    return amplitudeMV;
}
uint16_t OutputChannelClass::getPhaseDegrees(void)
{
    return phaseDegrees;
}
const char* OutputChannelClass::getWaveform(void)
{
    const char* waveformName = NULL;

    // TODO use FLASH memory instead of RAM
    switch (waveform)
    {
        case WAVEFORM_SINE:
            waveformName = waveformSineString;
        break;
        case WAVEFORM_TRIANGLE:
            waveformName = waveformTriangleString;
        break;
        case WAVEFORM_SQUARE:
            waveformName = waveformSquareString;
        break;
        case WAVEFORM_SQUARE_DIV_2:
            waveformName = waveformSquare2String;
        break;
    }

    return waveformName;
}
OUTPUT_STATUS_T OutputChannelClass::getOutputStatus(void)
{
    return outputStatus;
}

ERROR_MESSAGE_T OutputChannelClass::setFrequencyHz(uint32_t newFrequencyHz)
{
    ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;

    if (newFrequencyHz <= 8000000)
    {
        frequencyHz = newFrequencyHz;
        // @todo Turn output off while changing phase or frequency register
        //DDS.setOutput(DDS_OFF);

        // Set the new frequency
        DDS.sendFrequency(newFrequencyHz);

        // Turn the output back on if it was previous enabled
        //outputStatus == ON ? DDS.setOutput(DDS_ON) : DDS.setOutput(DDS_OFF);
        error = SUCCESS;
    }
    else if (newFrequencyHz > 8000000)
    {
        error = ERROR_MESSAGE_VALUE_TOO_LARGE;
    }
    else
    {
        error = ERROR_MESSAGE_OTHER;
    }
    return error;
}

// If no parameters are provided, it resends the amplitude to the amplifier.
// This is useful when switching from sine/triangle to square wave and vice versa
ERROR_MESSAGE_T OutputChannelClass::setAmplitudeMV()
{
    ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;

    if (Amplifier.set(amplitudeMV, waveform) == 0)
    {
        error = SUCCESS;
    }
    else
    {
        error = ERROR_MESSAGE_OTHER;
    }

    return error;
}

ERROR_MESSAGE_T OutputChannelClass::setAmplitudeMV(uint16_t newAmplitudeMV)
{
    ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;

    // Value must be between less than 4000mV
    // TODO currently limited to 2050 based on testing
    if (newAmplitudeMV <= 4000)
    {
        if (Amplifier.set(newAmplitudeMV, waveform) == 0)
        {
            amplitudeMV = newAmplitudeMV;
            error = SUCCESS;
        }
    }
    else
    {
        DEBUGLN(F("Value exceeded 4000"));
        error = ERROR_MESSAGE_OTHER;
    }

    return error;
}
ERROR_MESSAGE_T OutputChannelClass::setPhaseDegrees(uint16_t newPhaseDegrees)
{
    ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;

    if (newPhaseDegrees <= 360)
    {
        phaseDegrees = newPhaseDegrees;
        // @todo Turn output off while changing phase or frequency register
        //DDS.setOutput(DDS_OFF);

        // Set the phase
        DDS.sendPhase(newPhaseDegrees);

        // Turn the output back on if it was previous enabled
        //outputStatus == ON ? DDS.setOutput(DDS_ON) : DDS.setOutput(DDS_OFF);

        error = SUCCESS;
    }
    else if (newPhaseDegrees > 360)
    {
        error = ERROR_MESSAGE_VALUE_TOO_LARGE;
    }
    else
    {
        error = ERROR_MESSAGE_OTHER;
    }
    return error;
}
ERROR_MESSAGE_T OutputChannelClass::setWaveform(WAVEFORM_T newWaveform)
{
    ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;
    OUTPUT_STATUS_T previousOutputStatus = getOutputStatus();

    // Toggle the waveform off, then set the amplitude, then
    switch (newWaveform)
    {
        case WAVEFORM_SINE:
            error = SUCCESS;
            waveform = WAVEFORM_SINE;
            setOutputStatus(OFF);
            DDS.setOutputMode(DDS_MODE_SINE);
            setAmplitudeMV();
            setOutputStatus(previousOutputStatus);
        break;
        case WAVEFORM_TRIANGLE:
            error = SUCCESS;
            waveform = WAVEFORM_TRIANGLE;
            setOutputStatus(OFF);
            DDS.setOutputMode(DDS_MODE_TRIANGLE);
            setAmplitudeMV();
            setOutputStatus(previousOutputStatus);
        break;
        case WAVEFORM_SQUARE:
            error = SUCCESS;
            waveform = WAVEFORM_SQUARE;
            setOutputStatus(OFF);
            DDS.setOutputMode(DDS_MODE_SQUARE);
            setAmplitudeMV();
            setOutputStatus(previousOutputStatus);
        break;
        case WAVEFORM_SQUARE_DIV_2:
            error = SUCCESS;
            waveform = WAVEFORM_SQUARE_DIV_2;
            setOutputStatus(OFF);
            DDS.setOutputMode(DDS_MODE_SQUARE_DIV2);
            setAmplitudeMV();
            setOutputStatus(previousOutputStatus);
        break;
    }
    return error;
}
ERROR_MESSAGE_T OutputChannelClass::setOutputStatus(OUTPUT_STATUS_T newOutputStatus)
{
    ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;

    if (newOutputStatus == ON)
    {
        outputStatus = ON;
        DDS.setOutput(DDS_ON);
        error = SUCCESS;
    }
    else if (newOutputStatus == OFF)
    {
        outputStatus = OFF;
        DDS.setOutput(DDS_OFF);
        error = SUCCESS;
    }
    else
    {
        error = ERROR_MESSAGE_OTHER;
    }

    return error;
}

// Called when a device reset is requested.  The DDS chip turns the frequency, amplitude, phase, waveform back to defaults
// and we need to make sure the channel reflects the state of the DDS chip
// TODO currently not handling any error conditions...
ERROR_MESSAGE_T OutputChannelClass::reset()
{
    ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;

    setOutputStatus(OFF);
    setFrequencyHz(0);
    setAmplitudeMV(0);
    setPhaseDegrees(0);
    setWaveform(WAVEFORM_SINE);

    error = SUCCESS;
    return error;
}
