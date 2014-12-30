#include "Mux.h"
#include "Arduino.h"
#include "Debug.h"

#define DEBUG_OUTPUT 1

int muxSelect = 2;

MuxClass Mux;

MuxClass::MuxClass()
{
  // Enable the line to the mux as an output
  pinMode(muxSelect, OUTPUT);
}

MuxClass::~MuxClass()
{
}

void MuxClass::select(MUX_T select)
{
  switch (select)
  {
    case MUX_FILTERED:
      digitalWrite(muxSelect, LOW);
    break;
    
    case MUX_UNFILTERED:
      digitalWrite(muxSelect, HIGH);
    break;
  }
  
}
