#include <SPI.h>
#include "DDS.h"

DDSClass DDS;

const int slaveSelectPin = 10;

/// Control Register used on several common Analog Devices DDS chips such as AD9834 and AD9837
/// \todo did not compile when resv bits declared const
typedef struct
{
  uint8_t d15         :1;
  uint8_t d14         :1;
  uint8_t b28         :1;
  uint8_t hlb         :1;
  uint8_t fsel        :1;
  uint8_t psel        :1;
  uint8_t resv9       :1;
  uint8_t reset       :1;
  uint8_t sleep1      :1;
  uint8_t sleep2      :1;
  uint8_t opbiten     :1;
  uint8_t resv4       :1;
  uint8_t div2        :1;
  uint8_t resv2       :1;
  uint8_t mode        :1;
  uint8_t resv0       :1;
} ddsControlRegisterBits_t;

/// Provide the option to access the DDS control register directly or through individual bits
union
{
  uint16_t controlRegister;
  ddsControlRegisterBits_t bits;
} dds;

DDSClass::DDSClass()
{
  // Set SS as an output and setup SPI modes
  pinMode (slaveSelectPin, OUTPUT);
  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);   // MSbit first
  SPI.setDataMode(SPI_MODE3);  // SCLK idle high, sample on clock rising edge is SPI_MODE3
  
  dds.controlRegister = 0;
  dds.bits.reset = 1;
  
  sendControlRegister();
}

DDSClass::~DDSClass()
{
  SPI.end();
}

/// \brief Resets the DDS chip by setting the RESET register to 1
void DDSClass::reset()
{  
  dds.controlRegister = 0;
  dds.bits.reset = 1;
  
  sendControlRegister();
}

void DDSClass::sendFrequency(unsigned long newFrequency)
{
  uint32_t frequencyRegister = 0;
  uint16_t frequencyLSB = 0;
  uint16_t frequencyMSB = 0;
  
  // Turn output off while changing phase register
  setOutput(DDS_OFF);
  
  // Calculation is 2^28/F_MCLK * FREQ = FREQ_REG
  // 2^28/F_MCLK = 16.777216, but instead of dealing with floating point, we'll calculate using 168 and divide by 1000
  // this method equates to about a 0.135% error
  frequencyRegister = (168 * newFrequency) / 10;
  
  // First write contains the 14 LSbits of the frequency word, next write contains 14 MSbits
  frequencyLSB = frequencyRegister;
  frequencyLSB &= ~(1<<15);  // Replace bit 15 with a 0
  frequencyLSB |= (1<<14);   // Replace bit 14 with a 1
  
  frequencyMSB = (frequencyRegister>>16);
  frequencyMSB &= ~(1<<15);  // Replace bit 15 with a 0
  frequencyMSB |= (1<<14);   // Replace bit 14 with a 1
  
  // Write 14-bits of LSB
  SPI.transfer(frequencyLSB>>8); // Write MSB first
  SPI.transfer(frequencyLSB);    // Write LSB next
  
  // Write 14-bits of MSB 
  SPI.transfer(frequencyMSB>>8);
  SPI.transfer(frequencyMSB);
  
  setOutput(DDS_ON);
}

void DDSClass::sendPhase(unsigned int newPhase)
{
  uint32_t phaseCalculation = 0;
  uint16_t phaseRegister = 0;
  
  // Turn output off while changing phase register
  setOutput(DDS_OFF);
  
  // Calculation is 4096/360 * PHASE = PHASE_REG
  // 4096/360 = 11.37777, but we'll use 11378 and divide by 1000 for an error of 0.002%
  phaseCalculation = (11378 * newPhase) / 1000;
  
  // Phase register has 0b110X for bits <15:12>
  phaseRegister = phaseCalculation; 
  phaseRegister |= ((1<<15) | (1<<14));
  phaseRegister &= ~(1<<13);
  
  SPI.transfer(phaseRegister>>8); // MSB first
  SPI.transfer(phaseRegister);    // LSB next
  
  setOutput(DDS_ON);
}

void DDSClass::setOutputMode(ddsMode_t newOutputWave)
{
  switch (newOutputWave)
  {
    case DDS_MODE_SINE:
      dds.bits.opbiten = 0;
      dds.bits.mode = 0;
    break;
    case DDS_MODE_TRIANGLE:
      dds.bits.opbiten = 0;
      dds.bits.mode = 1;
    break;
    case DDS_MODE_SQUARE:
      dds.bits.opbiten = 1;
      dds.bits.div2 = 0;
    break;
    case DDS_MODE_SQUARE_DIV2:
      dds.bits.opbiten = 1;
      dds.bits.div2 = 1;
    break;
    default:
      // This could be debug
      Serial.println("Invalid output mode");
    break;
  }
  
  sendControlRegister();
}

void DDSClass::setOutput(ddsOutput_t output)
{
  switch (output)
  {
    case DDS_OFF:
      dds.bits.reset = 1;
    break;
    case DDS_ON:
      dds.bits.reset = 0;
    break;
    
    sendControlRegister();
  }
}

// Private Functions_________________________________________________________________

/// \brief Sends the control register to the DDS chip 
void DDSClass::sendControlRegister()
{
  /// \todo Do we need to manually put SS low here?
  // Datasheet shows LSB with MSb in examples
  SPI.transfer((dds.controlRegister>>8));  //MSB
  SPI.transfer(dds.controlRegister);       //LSB
  /// \todo Do we need to manually put SS high here?
}
