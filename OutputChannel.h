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
#ifndef OutputChannel_h
#define OutputChannel_h

#include "Arduino.h"

typedef enum
{
    OFF = 0,
    ON = 1
} OUTPUT_STATUS_T;

typedef enum
{
  SUCCESS=0,
  ERROR_MESSAGE_UNKNOWN,
  ERROR_MESSAGE_VALUE_TOO_SMALL,
  ERROR_MESSAGE_VALUE_TOO_LARGE,
  ERROR_MESSAGE_OTHER
} ERROR_MESSAGE_T;

typedef enum
{
  WAVEFORM_SINE=0,
  WAVEFORM_TRIANGLE,
  WAVEFORM_SQUARE,
  WAVEFORM_SQUARE_DIV_2
} WAVEFORM_T;

/// @brief Class for storing information about an output channel
class OutputChannelClass
{
  public:
    OutputChannelClass(uint8_t cNumber);
    ~OutputChannelClass();
    void init(void);
    uint8_t getChannelNumber(void);
    uint32_t getFrequencyHz(void);
    uint16_t getAmplitudeMV(void);
    uint16_t getPhaseDegrees(void);
    const char* getWaveform(void);
    OUTPUT_STATUS_T getOutputStatus(void);
    ERROR_MESSAGE_T setFrequencyHz(uint32_t);
    ERROR_MESSAGE_T setAmplitudeMV();
    ERROR_MESSAGE_T setAmplitudeMV(uint16_t);
    ERROR_MESSAGE_T setPhaseDegrees(uint16_t);
    ERROR_MESSAGE_T setWaveform(WAVEFORM_T);
    ERROR_MESSAGE_T setOutputStatus(OUTPUT_STATUS_T);
    ERROR_MESSAGE_T reset();
  private:
    uint8_t channelNumber; //!< channel number {1..5}
    uint32_t frequencyHz;   //!< frequency in hz {0..8MHz}
    uint16_t amplitudeMV;    //!< magnitude of output in mV {100..4000mV}
    uint16_t phaseDegrees;   //!< phase angle {0..359degrees}
    WAVEFORM_T waveform;
    OUTPUT_STATUS_T outputStatus;        //!< off or on
};

#endif

