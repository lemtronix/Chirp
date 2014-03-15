#include "OutputChannel.h"

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

unsigned char OutputChannelClass::getChannelNumber(void)
{
  return channelNumber;
}
unsigned long OutputChannelClass::getFrequencyHz(void)
{
  return frequencyHz;
}
unsigned int OutputChannelClass::getAmplitudeMV(void)
{
  return amplitudeMV;
}
unsigned int OutputChannelClass::getPhaseDegrees(void)
{
  return phaseDegrees;
}
boolean OutputChannelClass::getOutputStatus(void)
{
  return outputStatus;
}

ERROR_MESSAGE_T OutputChannelClass::setFrequencyHz(unsigned long newFrequencyHz)
{
  ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;
  
  if (newFrequencyHz <= 8000000)
  {
    frequencyHz = newFrequencyHz;
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

ERROR_MESSAGE_T OutputChannelClass::setAmplitudeMV(unsigned int newAmplitudeMV)
{
  ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;

  // Value must be between less than 4000mV
  if (newAmplitudeMV <= 4000)
  {
    amplitudeMV = newAmplitudeMV;
    error = SUCCESS;
  }
  else if (newAmplitudeMV > 4000)
  {
    error = ERROR_MESSAGE_VALUE_TOO_LARGE;
  }
  else
  {
    error = ERROR_MESSAGE_OTHER;
  }
  
  return error;
}
ERROR_MESSAGE_T OutputChannelClass::setPhaseDegrees(unsigned int newPhaseDegrees)
{
  ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;
  if (newPhaseDegrees <= 360)
  {
    phaseDegrees = newPhaseDegrees;
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
ERROR_MESSAGE_T OutputChannelClass::setOutputStatus(boolean newOutputStatus)
{
  ERROR_MESSAGE_T error = ERROR_MESSAGE_UNKNOWN;
  if (newOutputStatus == ON)
  {
    outputStatus = ON;
  }
  else if (newOutputStatus == OFF)
  {
    outputStatus = OFF;
  }
  else
  {
    error = ERROR_MESSAGE_OTHER;
  }
  return error;
}
