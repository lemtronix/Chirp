#ifndef DDS_h
#define DDS_h

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
    void sendFrequency(unsigned long);
    void sendPhase(unsigned int);
    void setOutputMode(ddsMode_t);
    void setOutput(ddsOutput_t);
  private:
    void writeDDS(uint16_t data);
};

extern DDSClass DDS;

#endif DDS_h

