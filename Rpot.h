#ifndef Rpot_h
#define Rpot_h

#include "Arduino.h"

/// @brief Class for storing information about an output channel
class RpotClass
{
  public:
    RpotClass();
    ~RpotClass();
    void init();
  private:
    void softwareReset();
    void disconnectPots();
};

extern RpotClass RPOT;

#endif

