/** Power up sequence
*  Notes:
*  -On power up, the TCON register is set to 0xFF.  This is responsible for "connecting" the pots.  In Chirp, We use P0 as a rheostat and P1 as a potentiometer (disconnect P0B <b0>)
*  -I2C command byte format is A3A2A1A0C1C0D9D8 where <A3-A0> is the I2C address, <C1-C0> is the command byte, <D9-D8> are the MSB of the data
*/

#include <Wire.h>
#include "Rpot.h"
#include "Debug.h"
#define DEBUG_OUTPUT 1

// @todo mjl check <b3> (EEWA) of status register prior to sending a new value

// First byte is 0b0101A2A1A0 where <A2:A0> is the physical pull-ups, which are all pulled low. 0b0101000
// Second byte is the 4-bit memory address, 2-bit command, and bits <9:8> of the data byte
// Third byte are bits <7:0> of the data byte

#define RPOT_ADDRESS         0x28  // RPOT has a fixed address of 0b0101A2A1A0; where <A2:A0> are the hardware address select pins, currently 000
#define RPOT_SOFTWARE_RESET  0x1FF //

#define RPOT_FULL_SCALE   0x100
#define RPOT_ZERO_SCALE   0x000
#define RPOT_MID_SCALE    0x080
#define RPOT_TCON_DISABLE 0x000
#define RPOT_TCON_ENABLE  0x1FF
#define RPOT_MAX_DATA_VALUE RPOT_FULL_SCALE

#define DATA_MSB_MASK    0b0000001100000000

#define I2C_WRITE 0
#define I2C_READ  1

RpotClass Rpot;

RpotClass::RpotClass()
{
}

RpotClass::~RpotClass()
{
}

void RpotClass::init()
{
  uint16_t Data;
  // Start I2C module as a master device
  Wire.begin();
  
  // P0 is a rheostat, disconnect P0B <b0> in TCON register using read modify write
  DEBUGLN(F("Rpot: Rheostate mode: disconnecting R0B"));
//  read(RPOT_MEMORY_MAP_VOLATILE_TCON, &Data);
//  Data &= ~(1<<0);
  write(RPOT_MEMORY_MAP_VOLATILE_TCON, RPOT_CMD_WRITE_DATA, 0x00FE);
}

void RpotClass::printStatus()
{
  uint16_t Data;
  read(RPOT_MEMORY_MAP_STATUS_REGISTER, &Data);
  
  Serial.println(Data, HEX);
}

void RpotClass::printTcon()
{
  uint16_t Data;
  read(RPOT_MEMORY_MAP_VOLATILE_TCON, &Data);
  
  Serial.println(Data, HEX);
}

void RpotClass::printPotValue(uint8_t PotNumber)
{
  uint16_t Data;

  switch (PotNumber)
  {
    case 0:
      read(RPOT_MEMORY_MAP_VOLATILE_WIPER_0, &Data);
    break;
    case 1:
      read(RPOT_MEMORY_MAP_VOLATILE_WIPER_1, &Data);
    break;
    default:
      Serial.println(F("Rpot invalid value"));
    break;
  }

  Serial.println(Data, HEX);
}
/** @brief Sends an I2C command to the RPOT
 *
 *  @details 
 *  1. First byte is the I2C address <b7-b1> and <b0> is the read/write bit
 *  2. Second byte is the command byte in the format A3A2A1A0C1C0D9D8 where <a3-a0> is the I2C address, <c1-c0> is the command byte, <d9-d8> are the MSB of the data
 *  3. Third byte is the LSB of the data <d7-d0> (only for read and write commands)
 *
 *  @param MemoryAddress Memory address to be written to
 *  @param Data Data to be send to the RPOT
 *
 *  @returns 0 if successful, 1 if an error
 */
uint8_t RpotClass::write(RPOT_MEMORY_MAP_T MemoryAddress, RPOT_CMD_T Command, uint16_t Data)
{
  uint8_t Byte2;  // Contains the 4 bits of the address, a two bit command, and two data bits
  uint8_t Byte3;  // Contains all data (optional based on command used)
  uint8_t DataMSB; // Contains the upper 2 bits of the data and is sent in Byte2
  
  // Check inputs
  if (MemoryAddress > RPOT_MEMORY_MAP_INVALID_VALUE)
  {
    // Invalid address
    DEBUGLN(F("Rpot: Invalid Address"));
    return 1;
  }
  else if (Command >= RPOT_CMD_INVALID)
  {
    // Invalid command
    DEBUGLN(F("Rpot: Invalid Command"));
    return 1;
  }
  else if (Data > RPOT_MAX_DATA_VALUE)
  {
    // Invalid data
    DEBUGLN(F("Rpot: Invalid data"));
    return 1;
  }
    
  Byte2 = (MemoryAddress << 4);    // <b7:b4>
  Byte2 |= (Command << 2);         // <b3:b2>
  
  // Send data if it's passed in and the command isn't a read command
  if (Data && (Command != RPOT_CMD_READ_DATA))
  {
    // Finish off Byte2 with the upper two bits if data is available
    DataMSB = (Data & DATA_MSB_MASK);      // Only getting bits 9 and 10 of the data
    Byte2 |= (DataMSB >> 8);               // <b1:b0>
    
    // Byte3 contains a full 8-bits of data
    Byte3 = (uint8_t)Data & 0xFF;
  }
  
  Wire.beginTransmission(RPOT_ADDRESS);
  Wire.write(Byte2);
  
  if (Data)
  {
    Wire.write(Byte3);
  }
  
  Wire.endTransmission(true);  // Send the stop bit

  return 0;
}

/** @brief Read two bytes of data from the RPOT
 *
 *  @param MemoryAddress Memory address to be read from
 *  @param pData Data read from the RPOT
 *
 *  @returns 0 if successful, 1 if an error
 */
uint8_t RpotClass::read(RPOT_MEMORY_MAP_T MemoryAddress, uint16_t* pData)
{
  uint8_t NumberOfBytesRead;
  uint8_t Byte2;
  uint8_t Byte3;
  
  // Check inputs
  if (MemoryAddress > RPOT_MEMORY_MAP_INVALID_VALUE)
  {
    // Invalid address
    DEBUGLN(F("Rpot: Invalid address"));
    return 1;
  }
  else if (pData == NULL)
  {
    DEBUGLN(F("Rpot: pointer is null"));
    return 1;
  }
  
  // First, setup the read by writing a read command to the memory register
  if(write(MemoryAddress, RPOT_CMD_READ_DATA, 0))
  {
    // unable to command the device to read
    DEBUGLN(F("Rpot: Unable to read"));
    return 1;
  }
  
  // Request 2 bytes from the RPOT and send a stop command
  Wire.requestFrom(RPOT_ADDRESS, 2, true);
  
  Byte2 = Wire.read();
  Byte3 = Wire.read();
    
  *pData = (Byte2 << 8);  //MSB
  *pData |= (Byte3);      //LSB
  
  return 0;
}

