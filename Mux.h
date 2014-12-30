#ifndef Mux_h
#define Mux_h

typedef enum
{
  MUX_FILTERED,
  MUX_UNFILTERED,
} MUX_T;

/// @brief Class for storing information about an output channel
class MuxClass
{
  public:
    MuxClass();
    ~MuxClass();
    void select(MUX_T select);
  private:

};

extern MuxClass Mux;

#endif
