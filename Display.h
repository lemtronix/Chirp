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
    void displayVersionInfo();
    void invalidSelection();

  private:
    void print_P(const char*);
};

extern DisplayClass Display;

#endif

