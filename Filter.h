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
*/
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
