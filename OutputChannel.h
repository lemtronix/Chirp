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

/// @brief Class for storing information about an output channel
class OutputChannelClass
{
  public:
    OutputChannelClass(unsigned char cNumber);
    ~OutputChannelClass();
    void init(void);
    unsigned char getChannelNumber(void);
    unsigned long getFrequencyHz(void);
    unsigned int getAmplitudeMV(void);
    unsigned int getPhaseDegrees(void);
    boolean getOutputStatus(void);
    ERROR_MESSAGE_T setFrequencyHz(unsigned long);
    ERROR_MESSAGE_T setAmplitudeMV(unsigned int);
    ERROR_MESSAGE_T setPhaseDegrees(unsigned int);
    ERROR_MESSAGE_T setOutputStatus(boolean);
  private:
    unsigned char channelNumber; //!< channel number {1..5}
    unsigned long frequencyHz;   //!< frequency in hz {0..8MHz}
    unsigned int amplitudeMV;    //!< magnitude of output in mV {100..4000mV}
    unsigned int phaseDegrees;   //!< phase angle {0..359degrees}
    boolean outputStatus;        //!< off or on
};

#endif

