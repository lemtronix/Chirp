#ifndef Filter_h
#define Filter_h

/// @brief Class for storing information about an output channel
class FilterClass
{
  public:
    FilterClass();
    ~FilterClass();
    void on();
    void off();
  private:
};

extern FilterClass Filter;

#endif
