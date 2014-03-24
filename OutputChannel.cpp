#include "OutputChannel.h"
#include "DDS.h" // used by OutputChannel.cpp
#include "Rpot.h" // used by OutputChannel.cpp

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
  RPOT.init();
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
    // @todo Turn output off while changing phase or frequency register
    DDS.setOutput(DDS_OFF);
    
    // Set the new frequency
    DDS.sendFrequency(newFrequencyHz);
    
    // Turn the output back on if it was previous enabled
    outputStatus == ON ? DDS.setOutput(DDS_ON) : DDS.setOutput(DDS_OFF);
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
    // @todo Turn output off while changing phase or frequency register
    DDS.setOutput(DDS_OFF);
    
    // Set the phase
    DDS.sendPhase(newPhaseDegrees);
    
    // Turn the output back on if it was previous enabled
    outputStatus == ON ? DDS.setOutput(DDS_ON) : DDS.setOutput(DDS_OFF);
    
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
    DDS.setOutput(DDS_ON);
  }
  else if (newOutputStatus == OFF)
  {
    outputStatus = OFF;
    DDS.setOutput(DDS_OFF);
  }
  else
  {
    error = ERROR_MESSAGE_OTHER;
  }
  return error;
}

