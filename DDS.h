#ifndef DDS_h
#define DDS_h

// TODO ddsMode_t is the same was WAVEFORM_T, consolidate these
typedef enum
{
  DDS_MODE_SINE = 0,
  DDS_MODE_TRIANGLE,
  DDS_MODE_SQUARE,
  DDS_MODE_SQUARE_DIV2
} ddsMode_t;

typedef enum
{
  DDS_OFF  = 0,
  DDS_ON   = 1  
} ddsOutput_t;

class DDSClass
{
  public:
    DDSClass();
    ~DDSClass();
    void init();
    void reset();
    void sendFrequency(uint32_t);
    void sendPhase(uint16_t);
    void setOutputMode(ddsMode_t);
    void setOutput(ddsOutput_t);
  private:
    void writeDDS(uint16_t data);
};

extern DDSClass DDS;

#endif DDS_h

