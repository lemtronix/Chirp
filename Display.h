#ifndef Display_h
#define Display_h

#include "Arduino.h"

class DisplayClass
{
  public:
    DisplayClass();
    ~DisplayClass();
    void mainMenu();
    void testMenu();
    void resetDevice();
    void bootLoaderMode();
    void frequencyMenu();
    void amplitudeMenu();
    void phaseMenu();
    void outputOff();
    void outputOn();
    void invalidSelection();
  private:
};

extern DisplayClass Display;

#endif
