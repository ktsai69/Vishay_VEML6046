/*
  This file is part of the Vishay_VEML6046 library.
  Copyright (c) 2021 Vishay Capella.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include <Wire.h>

class VEML6046Class {
  public:
    VEML6046Class(TwoWire& wire);
    virtual ~VEML6046Class();

    int begin();
    void end();
    boolean reads( uint8_t reg, uint8_t *data, uint8_t num);
    boolean writes( uint8_t reg, uint8_t *data, uint8_t num);
    boolean readByte( uint8_t reg, uint8_t *byteData);
    boolean writeByte( uint8_t reg, uint8_t byteData);
    boolean readWord( uint8_t reg, uint16_t *wordData);
    boolean writeWord( uint8_t reg, uint16_t wordData);
    virtual boolean read_ALS(uint16_t *als);
    virtual boolean read_IR(uint16_t *ir);
    virtual boolean read_RGB(uint16_t *r, uint16_t *g, uint16_t *b);
    virtual float get_lux(void);
    boolean ALS_INT_EN(boolean enable);
    boolean ALS_INT_EN_with_threshold(float percent);
    boolean read_ALS_INT(uint8_t *int_flag);
    
    float lens_factor;

    const uint8_t
      POR_FLAG = (1 << 0),
      ALS_IF_H = (1 << 1),
      ALS_IF_L = (1 << 2),
      ALS_IF_F = (1 << 3);
      
  private:
    TwoWire* _wire;
    uint8_t slaveAddress;
    boolean bitsUpdate(uint8_t reg, uint8_t mask, uint8_t update);  
};

extern VEML6046Class veml6046;
