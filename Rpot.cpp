#include <Wire.h>
#include "Rpot.h"
#include "Debug.h"
#define DEBUG_OUTPUT 1

// Memory map:
// 0x00 Volatile Wiper 0 
// 0x01 Volatile Wiper 1
// 0x04 Volatile TCON (terminal control) register

// Commands are:
// 0x00 Write data
// 0x01 Increment
// 0x02 Decrement
// 0x03 Read data

// First byte is 0b0101A2A1A0 where <A2:A0> is the physical pull-ups, which are all pulled low. 0b0101000
// Second byte is the 4-bit memory address, 2-bit command, and bits <9:8> of the data byte
// Third byte are bits <7:0> of the data byte

// Zero scale wiper 0 and wiper 1 example.  [ is a start bit, ] is a stop bit
// 0b[<0101><000>0
// 0b<0000><00><00
// 0b00000000>]

#define RPOT_ADDRESS         0x50  // RPOT has a fixed address of 0b0101A2A1A0; where <A2:A0> are the hardware address select pins, currently 000
#define RPOT_SOFTWARE_RESET  0x1FF //

#define RPOT_FULL_SCALE   0x100
#define RPOT_ZERO_SCALE   0x000
#define RPOT_MID_SCALE    0x080
#define RPOT_TCON_DISABLE 0x000
#define RPOT_TCON_ENABLE  0x1FF

RpotClass RPOT;

RpotClass::RpotClass()
{
}

RpotClass::~RpotClass()
{
}

void RpotClass::init()
{
  // Start I2C module as a master device
  Wire.begin();
  
  softwareReset();
  disconnectPots();
  // Initialize Rpot to zero scale (0x000) on both POTs (R_AB)
  // Set Wiper to B to get maximum resistance
}
void RpotClass::softwareReset()
{
  // I2C software reset issue star bit, 9 bits of '1' followed by another start bit, then a stop bit
  // Command should be [0x51, 0x1FF[]
  DEBUGLN("Resetting RPOT...");
  
  Wire.beginTransmission(RPOT_ADDRESS);
  Wire.write(0);
  Wire.write(RPOT_SOFTWARE_RESET);      /// \todo Does Wire allow for 9 bits to be send?
  Wire.endTransmission(false);          // Resend start bit
  Wire.endTransmission();
  
  DEBUGLN("Reset complete!");
}

void RpotClass::disconnectPots()
{
  // On POR/BOR, register is loaded with a 0x1FF (everything enabled)
  // Write a 0x000 to the TCON register to turn everything off, including general address calls
  DEBUGLN("Disconnecting RPOTs...");
  
  Wire.beginTransmission(RPOT_ADDRESS);
  Wire.write(RPOT_TCON_DISABLE);
  Serial.println(Wire.endTransmission());
}

