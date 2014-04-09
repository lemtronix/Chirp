#include <SPI.h>
#include "DDS.h"
#include "Debug.h"
#define DEBUG_OUTPUT 1

#define CLOCK_FREQUENCY 16000000
#define DDS_28BITS      0x10000000

DDSClass DDS;

const int slaveSelectPin = 10;

// Defines that allows for easy bit shifting when writing to dds chip using single write
/// @todo Are using the defines for DDS every used?
#define D15        15
#define D14        14
#define B28        13
#define HLB        12
#define FSEL       11
#define PSEL       10
#define RESET      8
#define SLEEP1     7
#define SLEEP2     6
#define OPBITEN    5
#define DIV2       3
#define MODE       1

/// Control Register used on several common Analog Devices DDS chips such as AD9834 and AD9837
/// @todo did not compile when resv bits declared const
typedef struct __attribute__ ((packed)) __attribute__ ((aligned))
{
  uint8_t resv0       :1;
  uint8_t mode        :1;    //!< selects between sine (MODE=0) and triangle wave (MODE=1)
  uint8_t resv2       :1;
  uint8_t div2        :1;    //!< when OPBITEN is set, this corresponds to the square wave frequency.  DIV2=0 is MSB/2
  uint8_t resv4       :1;
  uint8_t opbiten     :1;    //!< selects square wave, OPBITEN=0 allows for sine or triangle
  uint8_t sleep12     :1;    //!< powers down the on-chip DAC, useful when the DDS is used to output the MSB of the DAC data
  uint8_t sleep1      :1;    //!< disables the internal MCLK, but DAC output remains at present value
  uint8_t reset       :1;    //!< corresponds to an analog output of midscale
  uint8_t resv9       :1;
  uint8_t psel        :1;    //!< phase select bit. PSEL=0 selects PHASE0
  uint8_t fsel        :1;    //!< frequency select bit.  FSEL=0 selects FREQ0
  uint8_t hlb         :1;    //!< determines if a write to frequency register is LSB or MSB when B28=0. HLB=1 is MSB
  uint8_t b28         :1;    //!< loads frequency register is two consecutive writes, LSB first. B28=0 is individual
  uint8_t d14         :1;
  uint8_t d15         :1;    //!< D15 and D14 functions are handled in the sendFrequency and sendPhase functions, do not manually change them!
} ddsControlRegisterBits_t;

/// Provides the option to access the DDS control register directly or through individual bits
union
{
  uint16_t controlRegister;
  ddsControlRegisterBits_t bits;
} dds;

DDSClass::DDSClass()
{
}

DDSClass::~DDSClass()
{
  SPI.end();
}

/// @brief Resets the DDS chip by setting the RESET register to 1
void DDSClass::init()
{
    // Set SS as an output and setup SPI modes
  pinMode (slaveSelectPin, OUTPUT);
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setBitOrder(MSBFIRST);   // MSbit first
  SPI.setDataMode(SPI_MODE2);  // SCLK idle high, sample on clock falling edge is SPI_MODE2
  reset();
}
void DDSClass::reset()
{
  // Frequency write function in this file performs its set operation in two consecutive (b28).  Also start the DDS in reset mode.
  // Clear all parameters, set the output to a sine wave and turn off the output (reset = 1)
  dds.controlRegister = 0;
  dds.bits.b28=1;
  
  sendFrequency(0);
  sendPhase(0);
  setOutputMode(DDS_MODE_SINE);
  setOutput(DDS_OFF);
  
  DEBUGLN("DDS reset complete");
}

void DDSClass::sendFrequency(unsigned long newFrequency)
{
  uint32_t frequencyTuningWord = 0; /// 
  uint16_t LSB = 0;                 /// Lower 16-bits of the 28-bit register
  uint16_t MSB = 0;                 /// Upper 16-bits of the 28-bit register
  float calculatedFrequency = 0.0;  /// 28-bit register for storing the frequency
    
  // Calculation is 2^28/F_MCLK * FREQ = FREQ_REG
  calculatedFrequency = (((float)(newFrequency)))/(CLOCK_FREQUENCY);
  frequencyTuningWord = (uint32_t)(calculatedFrequency*DDS_28BITS);
  
  // Modify the frequencyTuningWord into two 14-bit registers to be sent
  MSB = (uint16_t)((frequencyTuningWord & 0xFFFC000)>>14);  
  LSB = (uint16_t)(frequencyTuningWord & 0x3FFF);
  
  // We're always writing to the FREQ0 register to keep this nice and simple (0b01XXXXXX)
  MSB |= 0x4000;
  LSB |= 0x4000;
  
  // Write it to the DDS chip
  writeDDS(LSB);
  writeDDS(MSB);
  
  DEBUG("LSB: 0x");
  DEBUGLN(LSB,HEX);
  DEBUG("MSB: 0x");
  DEBUGLN(MSB,HEX);
  
  DEBUGLN("DDS freq set");
}

void DDSClass::sendPhase(unsigned int newPhase)
{
  uint32_t phaseCalculation = 0;
  uint16_t phaseRegister = 0;
  
  // Calculation is 4096/360 * PHASE = PHASE_REG
  // 4096/360 = 11.37777, but we'll use 11378 and divide by 1000 for an error of 0.002%
  phaseCalculation = (11378 * newPhase) / 1000;
  
  // Phase0 register has 0b110X for bits <15:12> in control register
  // Phase1 register has 0b111X
  // We're always writing to the PHASE0 register to keep this nice and simple 
  /// @todo Enhance this function to allow for writes to either PHASE0 or PHASE1
  phaseRegister = phaseCalculation; 
  phaseRegister |= ((1<<15) | (1<<14));
  phaseRegister &= ~(1<<13);
    
  writeDDS(phaseRegister);
    
  DEBUGLN("DDS phase set");
}

void DDSClass::setOutputMode(ddsMode_t newOutputWave)
{
  switch (newOutputWave)
  {
    case DDS_MODE_SINE:
      dds.bits.opbiten = 0;
      dds.bits.mode = 0;
      DEBUGLN("DDS sine");
    break;
    case DDS_MODE_TRIANGLE:
      dds.bits.opbiten = 0;
      dds.bits.mode = 1;
      DEBUGLN("DDS triangle");
    break;
    case DDS_MODE_SQUARE:
      dds.bits.opbiten = 1;
      dds.bits.div2 = 0;
      DEBUGLN("DDS square");
    break;
    case DDS_MODE_SQUARE_DIV2:
      dds.bits.opbiten = 1;
      dds.bits.div2 = 1;
      DEBUGLN("DDS square div2");
    break;
    default:
      // This could be debug
      DEBUG("Invalid output mode");
    break;
  }
  
  writeDDS(dds.controlRegister);
}

void DDSClass::setOutput(ddsOutput_t output)
{
  switch (output)
  {
    case DDS_OFF:
      dds.bits.reset = 1;
      DEBUGLN("DDS off");
    break;
    case DDS_ON:
      dds.bits.reset = 0;
      DEBUGLN("DDS on");
    break;
    default:
      DEBUGLN("DDS output mode invalid");
    break;
  }
  
  writeDDS(dds.controlRegister);
}

// Private Functions_________________________________________________________________

/// @brief Sends the control register to the DDS chip 
void DDSClass::writeDDS(uint16_t data)
{
  digitalWrite(slaveSelectPin, LOW);
  // Datasheet shows LSB with MSb in examples
  SPI.transfer((data>>8));  //MSB
  SPI.transfer(data);       //LSB
    
  digitalWrite(slaveSelectPin, HIGH);
  
  DEBUG("DDS write: ");
  DEBUGLN(data, BIN); 
  
  /// @todo Add a delay in writeDDS?
}

