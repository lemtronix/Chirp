/** @todo Change this class to abstract out the fact that Chirp uses an pot to control the output voltage, perhaps AmplifierClass?
*/

#ifndef Amplifier_h
#define Amplifier_h

#include "Arduino.h"

typedef enum
{
  RPOT_MEMORY_MAP_VOLATILE_WIPER_0 = 0x00,
  RPOT_MEMORY_MAP_VOLATILE_WIPER_1 = 0x01,
  RPOT_MEMORY_MAP_NON_VOLATILE_WIPER_0 = 0x02,
  RPOT_MEMORY_MAP_NON_VOLATILE_WIPER_1 = 0x03,
  RPOT_MEMORY_MAP_VOLATILE_TCON = 0x04,        ///< (Default 0xFF) - b8: General Call Enable; <b7-b4> R1 settings; <b3-b0> R0 settings
  RPOT_MEMORY_MAP_STATUS_REGISTER = 0x05,      ///< <b7-b4>: Reserved; <b3>: EEPROM write active bit; <b2>: WiperLock status for R1; <b1>: WiperLock status for R0; <b0>: EEPROM write protect status bit
  RPOT_MEMORY_MAP_INVALID_VALUE,               ///< Invalid value
} RPOT_MEMORY_MAP_T;

typedef enum
{
  RPOT_CMD_WRITE_DATA = 0,
  RPOT_CMD_INCREMENT = 1,
  RPOT_CMD_DECREMENT = 2,
  RPOT_CMD_READ_DATA = 3,
  RPOT_CMD_INVALID,
} RPOT_CMD_T;

/// @brief Class for storing information about an output channel
class AmplifierClass
{
  public:
    AmplifierClass();
    ~AmplifierClass();
    void init();
    uint8_t set(uint16_t VoltageInMvRms);
    void printStatus();
    void printTcon();
    void printPotValue(uint8_t RpotNumber);
  private:
    uint8_t write(RPOT_MEMORY_MAP_T MemoryAddress, RPOT_CMD_T Command, uint16_t Data);
    uint8_t read(RPOT_MEMORY_MAP_T MemoryAddress, uint16_t* pData);
};

extern AmplifierClass Amplifier;

#endif

