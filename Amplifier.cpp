/** Power up sequence
 *  Notes:
 *  -On power up, the TCON register is set to 0xFF.  This is responsible for "connecting" the pots.  In Chirp, We use P0 as a rheostat and P1 as a potentiometer (disconnect P0B <b0>)
 *  -I2C command byte format is A3A2A1A0C1C0D9D8 where <A3-A0> is the I2C address, <C1-C0> is the command byte, <D9-D8> are the MSB of the data
 */

#include <Wire.h>
#include "Amplifier.h"
#include "OutputChannel.h"
#include "Debug.h"
#define DEBUG_OUTPUT 0

typedef int32_t q23_8_t; ///< resolution is 0.00390625

// @todo mjl check <b3> (EEWA) of status register prior to sending a new value

// First byte is 0b0101A2A1A0 where <A2:A0> is the physical pull-ups, which are all pulled low. 0b0101000
// Second byte is the 4-bit memory address, 2-bit command, and bits <9:8> of the data byte
// Third byte are bits <7:0> of the data byte

#define RPOT_ADDRESS         0x28  // RPOT has a fixed address of 0b0101A2A1A0; where <A2:A0> are the hardware address select pins, currently 000
#define RPOT_SOFTWARE_RESET  0x1FF //

#define RPOT_FULL_SCALE   0x0100
#define RPOT_ZERO_SCALE   0x0000
#define RPOT_MID_SCALE    0x0080
#define RPOT_TCON_DISABLE 0x0000
#define RPOT_TCON_ENABLE  0x01FF
#define RPOT_MAX_DATA_VALUE RPOT_FULL_SCALE

#define DATA_MSB_MASK    0b0000001100000000

#define I2C_WRITE 0
#define I2C_READ  1

AmplifierClass Amplifier;

AmplifierClass::AmplifierClass()
{
}

AmplifierClass::~AmplifierClass()
{
}

void AmplifierClass::init()
{
    uint16_t Data;
    // Start I2C module as a master device
    Wire.begin();

    // P0 is a rheostat, disconnect P0B <b0> in TCON register using read modify write
    DEBUGLN(F("Amplifier: Rheostate mode: disconnecting R0B"));
//  read(RPOT_MEMORY_MAP_VOLATILE_TCON, &Data);
//  Data &= ~(1<<0);
    write(RPOT_MEMORY_MAP_VOLATILE_TCON, RPOT_CMD_WRITE_DATA, 0x00FE);
}

/** @brief Sets the desired output voltage through the voltage amplfiier by controlling the I2C resistive potentiometer R0 and R1.
 *
 *  @details The DDS chip normally outputs a voltage of 400mV RMS while in sine and triangle mode and outputs >4V RMS if in square mode.
 *  The amplifier is a non-inverting op-amp with a potentiometer on the input to the amplifier for decreasing the output value 
 *  and a pot in the negative feedback path (i.e. going from the output to the inverting input) for amplifying the output value.
 *
 *  @param VoltageInMvRms
 *
 *  @returns 0 if successful, >=1 if an error occurred.
 *
 *  @pre Assumes VoltageInMvRms is already boundary checked before calling this function
 */
uint8_t AmplifierClass::set(uint16_t VoltageInMvRms, WAVEFORM_T waveform)
{
    // Max Resistance In Taps = 256, converted to Q23_8 (a.k.a. multiply by 256) = 10,000
    static const q23_8_t ResistanceInTapsConversionConstantQ23_8 = 10000;

    uint8_t ErrorCounter = 0;
    q23_8_t ResistanceInOhmsQ23_8;
    uint16_t R0ResistanceInTaps = 0;
    uint16_t R1ResistanceInTaps = 0;

    DEBUG(F("Amplifier: Input value is: "));
    DEBUG(VoltageInMvRms);
    DEBUGLN(F(" mV RMS"));

    // This is the case for Sine and Triangle Waveforms...
    if ((waveform == WAVEFORM_SINE) || (waveform == WAVEFORM_TRIANGLE))
    {
        if (VoltageInMvRms <= 350)
        {
            // if voltage less than 350mV, decrease the wiper on R1 (R1W) and make sure R0 is at 0ohms
            // The equation is (resistance = 27.732 * voltage + 270 ohms) with a maximum value of 10K and a minimum of 625 ohms
            R0ResistanceInTaps = RPOT_MAX_DATA_VALUE;

            ResistanceInOhmsQ23_8 = (7099 * (q23_8_t) VoltageInMvRms) + 69120;

            DEBUG(F("Amplifier <= 350: ResistanceInOhmsQ23_8 calculated is: "));
            DEBUGLN(ResistanceInOhmsQ23_8);

            R1ResistanceInTaps = ResistanceInOhmsQ23_8 / ResistanceInTapsConversionConstantQ23_8;

            DEBUG(F("Amplifier <= 350: R0ResistanceInTaps calculated is: 0x"));
            DEBUGLN(R0ResistanceInTaps, HEX);

            DEBUG(F("Amplifier <= 350: R1ResistanceInTaps calculated is: 0x"));
            DEBUGLN(R1ResistanceInTaps, HEX);

            if (R1ResistanceInTaps > RPOT_MAX_DATA_VALUE)
            {
                DEBUGLN(F("Amplifer <= 350: Upper limit reached"));
                R1ResistanceInTaps = RPOT_MAX_DATA_VALUE;
            }
            else if (R1ResistanceInTaps < 0x10)
            {
                // Below this value, the waveform looks pretty ugly
                DEBUGLN(F("Amplifer <= 350: Lower limit reached"));
                R1ResistanceInTaps = 0x10;
            }

            if (write(RPOT_MEMORY_MAP_VOLATILE_WIPER_0, RPOT_CMD_WRITE_DATA, R0ResistanceInTaps))
            {
                ErrorCounter++;
            }

            if (write(RPOT_MEMORY_MAP_VOLATILE_WIPER_1, RPOT_CMD_WRITE_DATA, R1ResistanceInTaps))
            {
                ErrorCounter++;
            }
        }
        else if (VoltageInMvRms > 350)
        {
            // if the voltage is > 400mV, then make sure the wiper on R1 is maxed out and start increasing the wiper on R0 (R0W), which is the amplifiers feedback resistor
            // The equation is resistance = -5.6818 * voltage + 12000 with a maximum value of 10K and a minimum value of
            R1ResistanceInTaps = RPOT_MAX_DATA_VALUE;

            ResistanceInOhmsQ23_8 = (-1455 * (q23_8_t) VoltageInMvRms) + 3072000;

            DEBUG(F("Amplifier > 350: ResistanceInOhmsQ23_8 calculated is: "));
            DEBUGLN(ResistanceInOhmsQ23_8);

            // TODO during testing, I saw issues where R0 (feedback resistor) created lots of noise when less than 80 ohms, but wasn't a problem when set to 0 ohms
            if (ResistanceInOhmsQ23_8 < 20480) // 20480 / Q8 shift... 20480 / 256 = 80 ohms
            {
                // Prevent negative values
                DEBUGLN(F("Amplifier > 350: ResistanceInOhmsQ23_8 is 80 ohms or less (negative), set to 0"));
                ResistanceInOhmsQ23_8 = 0;
            }

            R0ResistanceInTaps = ResistanceInOhmsQ23_8 / ResistanceInTapsConversionConstantQ23_8;

            DEBUG(F("Amplifier > 350: R0ResistanceInTaps calculated is: 0x"));
            DEBUGLN(R0ResistanceInTaps, HEX);

            DEBUG(F("Amplifier > 350: R1ResistanceInTaps calculated is: 0x"));
            DEBUGLN(R1ResistanceInTaps, HEX);

            if (R0ResistanceInTaps > RPOT_MAX_DATA_VALUE)
            {
                DEBUGLN(F("Amplifer > 350: Upper limit reached"));
                R0ResistanceInTaps = RPOT_MAX_DATA_VALUE;
            }
            else if (R0ResistanceInTaps < 0x0) // MJL: Was 0x65
            {
                // Below this value, the waveform looks pretty ugly
                DEBUGLN(F("Amplifer > 350: Lower limit reached"));
                R0ResistanceInTaps = 0x0;
            }
        }

    }
    else if (waveform == WAVEFORM_SQUARE || waveform == WAVEFORM_SQUARE_DIV_2)
    {
        // Then it is a square wave

        // The equation is ResistanceInOhms = 2.3046875x + 23.25; Converted to ResistanceInOhmsQ23_8 = 590x + 5952

        // TODO Sending small values (a0 through a6) did not turn the waveform off, but at a7 and above, it appeared to turn the waveform off
        // TODO Also noticed that resetting the device with a # worked; but the CLI did not update

        // This is the resistance for R1
        ResistanceInOhmsQ23_8 = (590 * (q23_8_t) VoltageInMvRms) + 5952;

        R1ResistanceInTaps = ResistanceInOhmsQ23_8 / ResistanceInTapsConversionConstantQ23_8;

        // Do not amplify the square wave as it causes distortion on the output, see to the lowest value
        R0ResistanceInTaps = RPOT_MAX_DATA_VALUE;

        DEBUG(F("R0ResistanceInTaps calculated is: 0x"));
        DEBUGLN(R0ResistanceInTaps, HEX);

        DEBUG(F("R1ResistanceInTaps calculated is: 0x"));
        DEBUGLN(R1ResistanceInTaps, HEX);

        if (R1ResistanceInTaps > RPOT_MAX_DATA_VALUE)
        {
            // Protect against a value larger than the maximum allowed
            R1ResistanceInTaps =RPOT_MAX_DATA_VALUE;
        }
    }
    else
    {
        // unknown value, increment the error counter
        ErrorCounter++;
    }

    // Write the value of the POTs
    if (write(RPOT_MEMORY_MAP_VOLATILE_WIPER_0, RPOT_CMD_WRITE_DATA, R0ResistanceInTaps))
    {
        ErrorCounter++;
    }

    if (write(RPOT_MEMORY_MAP_VOLATILE_WIPER_1, RPOT_CMD_WRITE_DATA, R1ResistanceInTaps))
    {
        ErrorCounter++;
    }

    return ErrorCounter;
}
void AmplifierClass::printStatus()
{
    uint16_t Data;
    read(RPOT_MEMORY_MAP_STATUS_REGISTER, &Data);

    Serial.println(Data, HEX);
}

void AmplifierClass::printTcon()
{
    uint16_t Data;
    read(RPOT_MEMORY_MAP_VOLATILE_TCON, &Data);

    Serial.println(Data, HEX);
}

void AmplifierClass::printPotValue(uint8_t PotNumber)
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
            Serial.println(F("Amplifier invalid value"));
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
uint8_t AmplifierClass::write(RPOT_MEMORY_MAP_T MemoryAddress, RPOT_CMD_T Command, uint16_t Data)
{
    uint8_t Byte2 = 0;  // Contains the 4 bits of the address, a two bit command, and two data bits
    uint8_t Byte3 = 0;  // Contains all data (optional based on command used)
    uint16_t DataMSB = 0; // Contains the upper 2 bits of the data and is sent in Byte2

    // Check inputs
    if (MemoryAddress > RPOT_MEMORY_MAP_INVALID_VALUE)
    {
        // Invalid address
        DEBUGLN(F("Amplifier: Invalid Address"));
        return 1;
    }
    else if (Command >= RPOT_CMD_INVALID)
    {
        // Invalid command
        DEBUGLN(F("Amplifier: Invalid Command"));
        return 1;
    }
    else if (Data > RPOT_MAX_DATA_VALUE)
    {
        // Invalid data
        DEBUGLN(F("Amplifier: Invalid data"));
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
        Byte3 = (uint8_t) Data & 0xFF;
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
uint8_t AmplifierClass::read(RPOT_MEMORY_MAP_T MemoryAddress, uint16_t* pData)
{
    uint8_t NumberOfBytesRead;
    uint8_t Byte2;
    uint8_t Byte3;

    // Check inputs
    if (MemoryAddress > RPOT_MEMORY_MAP_INVALID_VALUE)
    {
        // Invalid address
        DEBUGLN(F("Amplifier: Invalid address"));
        return 1;
    }
    else if (pData == NULL)
    {
        DEBUGLN(F("Amplifier: pointer is null"));
        return 1;
    }

    // First, setup the read by writing a read command to the memory register
    if (write(MemoryAddress, RPOT_CMD_READ_DATA, 0))
    {
        // unable to command the device to read
        DEBUGLN(F("Amplifier: Unable to read"));
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

