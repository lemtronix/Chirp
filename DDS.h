/*
    This file is part of Chirp.

    Chirp is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    Chirp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Chirp.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2016 Mike Lemberger
*/
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

