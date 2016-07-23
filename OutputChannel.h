#ifndef OutputChannel_h
#define OutputChannel_h

#include "Arduino.h"

#define ON  1
#define OFF 0

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
    boolean getOutputStatus(void);
    ERROR_MESSAGE_T setFrequencyHz(uint32_t);
    ERROR_MESSAGE_T setAmplitudeMV();
    ERROR_MESSAGE_T setAmplitudeMV(uint16_t);
    ERROR_MESSAGE_T setPhaseDegrees(uint16_t);
    ERROR_MESSAGE_T setWaveform(WAVEFORM_T);
    ERROR_MESSAGE_T setOutputStatus(boolean);
  private:
    uint8_t channelNumber; //!< channel number {1..5}
    uint32_t frequencyHz;   //!< frequency in hz {0..8MHz}
    uint16_t amplitudeMV;    //!< magnitude of output in mV {100..4000mV}
    uint16_t phaseDegrees;   //!< phase angle {0..359degrees}
    WAVEFORM_T waveform;
    boolean outputStatus;        //!< off or on
};

#endif

