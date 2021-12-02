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

#include "VEML6046.h"

#define VEML6046_ADDRESS            0x29
#define VEML6046_WHO_AM_I           0x01

#define VEML6046_REG_CMD00          0x00
#define VEML6046_REG_CMD01          0x01
#define VEML6046_REG_ALS_WH_L       0x04
#define VEML6046_REG_ALS_WH_H       0x05
#define VEML6046_REG_ALS_WL_L       0x06
#define VEML6046_REG_ALS_WL_H       0x07
#define VEML6046_REG_R_DATA_L       0x10
#define VEML6046_REG_R_DATA_H       0x11
#define VEML6046_REG_G_DATA_L       0x12
#define VEML6046_REG_G_DATA_H       0x13
#define VEML6046_REG_B_DATA_L       0x14
#define VEML6046_REG_B_DATA_H       0x15
#define VEML6046_REG_IR_DATA_L      0x16
#define VEML6046_REG_IR_DATA_H      0x17
#define VEML6046_REG_ID_L           0x18
#define VEML6046_REG_ID_H           0x19
#define VEML6046_REG_ALS_INT        0x1B

// CMD00
#define VEML6046_SD                 (1 << 0)
#define VEML6046_ALS_INT_EN         (1 << 1)
#define VEML6046_TRIG               (1 << 2)
#define VEML6046_AF                 (1 << 3)
#define VEML6046_IT_SHIFT           4
#define VEML6046_IT_MASK            (0x7 << VEML6046_IT_SHIFT)
#define VEML6046_IT_3_125MS         (0x0 << VEML6046_IT_SHIFT)
#define VEML6046_IT_6_25MS          (0x1 << VEML6046_IT_SHIFT)
#define VEML6046_IT_12_5MS          (0x2 << VEML6046_IT_SHIFT)
#define VEML6046_IT_25MS            (0x3 << VEML6046_IT_SHIFT)
#define VEML6046_IT_50MS            (0x4 << VEML6046_IT_SHIFT)
#define VEML6046_IT_100MS           (0x5 << VEML6046_IT_SHIFT)
#define VEML6046_IT_200MS           (0x6 << VEML6046_IT_SHIFT)
#define VEML6046_IT_400MS           (0x7 << VEML6046_IT_SHIFT)
#define VEML6046_TEST_EN            (1 << 7)

// CMD01
#define VEML6046_OTP_READ_TRIG      (1 << 0)
#define VEML6046_PERS_SHIFT         1
#define VEML6046_PERS_MASK          (0x3 << VEML6046_PERS_SHIFT)
#define VEML6046_PERS_1             (0x0 << VEML6046_PERS_SHIFT)
#define VEML6046_PERS_2             (0x1 << VEML6046_PERS_SHIFT)
#define VEML6046_PERS_4             (0x2 << VEML6046_PERS_SHIFT)
#define VEML6046_PERS_8             (0x3 << VEML6046_PERS_SHIFT)
#define VEML6046_GAIN_SHIFT         1
#define VEML6046_GAIN_MASK          (0x3 << VEML6046_GAIN_SHIFT)
#define VEML6046_GAIN_1             (0x0 << VEML6046_GAIN_SHIFT)
#define VEML6046_GAIN_2             (0x1 << VEML6046_GAIN_SHIFT)
#define VEML6046_GAIN_0_66          (0x2 << VEML6046_GAIN_SHIFT)
#define VEML6046_GAIN_0_5           (0x3 << VEML6046_GAIN_SHIFT)
#define VEML6046_PD_DIV2            (1 << 6)
#define VEML6046_SD0                (1 << 7)

// Default values
#define VEML6046_DEFAULT_CMD00      (VEML6046_IT_100MS)
#define VEML6046_DEFAULT_CMD01      (VEML6046_OTP_READ_TRIG)
#define VEML6046_DEFAULT_ALS_WH     0xFFFF
#define VEML6046_DEFAULT_ALS_WL     0x0000

VEML6046Class::VEML6046Class(TwoWire& wire) : _wire(&wire)
{
}

VEML6046Class::~VEML6046Class(void)
{
}

int VEML6046Class::begin(void)
{
  _wire->begin();
  slaveAddress = VEML6046_ADDRESS;

  // Prevent I2C bus lockup
  writeByte(VEML6046_REG_ALS_WL_L, 0x00);
  writeByte(VEML6046_REG_ALS_WL_L, 0x00);

  uint8_t id;
  if (!readByte(VEML6046_REG_ID_L, &id) || id != VEML6046_WHO_AM_I)
    return 0;

  // Initialization
  if (writeWord(VEML6046_REG_CMD00, VEML6046_DEFAULT_CMD00 | (VEML6046_DEFAULT_CMD01 << 8)) &&
      writeWord(VEML6046_REG_ALS_WH_L, VEML6046_DEFAULT_ALS_WH) &&
      writeWord(VEML6046_REG_ALS_WL_L, VEML6046_DEFAULT_ALS_WL))
  {
    lens_factor = 1.0;
    return 1;
  }
  return 0;
}

void VEML6046Class::end(void)
{
  ALS_INT_EN(false);
}

boolean VEML6046Class::reads(uint8_t reg, uint8_t *data, uint8_t num)
{
  uint8_t   wd;

  _wire->beginTransmission(slaveAddress);
  if (_wire->write (reg) != 1)
    goto reads_error;
  _wire->endTransmission(false);
  if (_wire->requestFrom(slaveAddress, (uint8_t)2) != 2)
    goto reads_error;
  
  wd = 10;
  while (_wire->available()<num && wd)
    wd--;
  if (!wd)
    goto reads_error;

  for (int i = 0; i < num; i++, data++)
    *data = _wire->read();
    
  return true;

reads_error:
  _wire->endTransmission(true);
  return false;
}

boolean VEML6046Class::writes(uint8_t reg, uint8_t *data, uint8_t num)
{
  boolean status = true;
  
  if ((reg == 0x00 && num == 1) || (reg == 0x01))
    return false;
    
  _wire->beginTransmission(slaveAddress);
  if (!_wire->write(reg))
    status = false;

  for (int i = 0; status && i < num; i ++, data++)
    if (!_wire->write(*data))
      status = false;
  _wire->endTransmission(true);

  return status;
}

boolean VEML6046Class::readByte(uint8_t reg, uint8_t *byteData)
{
  return reads(reg, byteData, 1);  
}

boolean VEML6046Class::writeByte(uint8_t reg, uint8_t byteData)
{
  return writes(reg, &byteData, 1);
}

boolean VEML6046Class::readWord(uint8_t reg, uint16_t *wordData)
{
  uint8_t buffer[2];
  boolean status = reads(reg, buffer, 2);
  *wordData = buffer[0] | ((uint16_t)buffer[1]) << 8;
  return status;
}

boolean VEML6046Class::writeWord(uint8_t reg, uint16_t wordData)
{
  uint8_t buffer[] = {(uint8_t)wordData, (uint8_t)(wordData >> 8)};
  return writes(reg, buffer, 2);
}

boolean VEML6046Class::bitsUpdate(uint8_t reg, uint8_t mask, uint8_t update)
{
  uint8_t value;
  
  if (!readByte(reg, &value))
    return false;
  value &= mask;
  value |= update;
  
  if (reg == 0x00 || reg == 0x01)
  {
    uint16_t valWord;
    if (!readWord(reg, &valWord))
      return false;
    if (reg == 0x00)
    {
      valWord &= 0xFF00;
      valWord |= value;
    } else
    {
      valWord &= 0x00FF;
      valWord |= value << 8;
    }
    return writeWord(0x00, valWord);
  }
  return writeByte(reg, value);
}

boolean VEML6046Class::read_ALS(uint16_t *als)
{
  return readWord(VEML6046_REG_G_DATA_L, als);
}

boolean VEML6046Class::read_IR(uint16_t *ir)
{
  return readWord(VEML6046_REG_IR_DATA_L, ir);
}

boolean VEML6046Class::read_RGB(uint16_t *r, uint16_t *g, uint16_t *b)
{
  if (readWord(VEML6046_REG_R_DATA_L, r) &&
      readWord(VEML6046_REG_G_DATA_L, g) &&
      readWord(VEML6046_REG_B_DATA_L, b))
    return true;
  return false;
}

float VEML6046Class::get_lux(void)
{
  uint8_t cmd[2];
  uint16_t als;
  
  if (!reads(VEML6046_REG_CMD00, cmd, 2) ||
      !read_ALS(&als))
    return -1.0f;

  uint8_t it = (cmd[0] & VEML6046_IT_MASK) >> VEML6046_IT_SHIFT;
  uint8_t gain = (cmd[1] & VEML6046_GAIN_MASK) >> VEML6046_GAIN_SHIFT;
  
  // GAIN = 0, PD_DIV = 0
  float resolution[] = {
    0.2176f, 0.1088f, 0.0544f, 0.0272f,
    0.0136f, 0.0068f, 0.0034f, 0.0017f};

  float lux = (float)als;
  lux *= resolution[it];
  if (cmd[1] & VEML6046_PD_DIV2)
    lux *= 2.0f;
  float gain_factor[] = {1.0f, 2.0f, 3.0f, 4.0f};
  lux *= gain_factor[gain];
  lux *= lens_factor;
  return lux;
}

boolean VEML6046Class::ALS_INT_EN(boolean enable)
{
  return bitsUpdate(
    VEML6046_REG_CMD00,
    ~VEML6046_ALS_INT_EN,
    enable ? VEML6046_ALS_INT_EN : 0);
}

boolean VEML6046Class::ALS_INT_EN_with_threshold(float percent)
{
  uint16_t als;
  
  if (!read_ALS(&als) || percent <= 0)
    return false;
 
  float thdh = (float)als * (100.0 + percent) / 100.0;
  float thdl = (float)als * (100.0 - percent) / 100.0;
  if (thdh - thdl < 1.0)
    thdh += 1.0;
  uint16_t wh = (thdh > 65535.0f) ? 65535 : (uint16_t)thdh;   
  uint16_t wl = (thdl < 0.0f) ? 0 : (uint16_t)thdl;

  if (ALS_INT_EN(false) &&
      writeWord(VEML6046_REG_ALS_WH_L, wh) &&
      writeWord(VEML6046_REG_ALS_WL_L, wl) &&
      ALS_INT_EN(true))
    return true;
    
  return false;
}

boolean VEML6046Class::read_ALS_INT(uint8_t *int_flag)
{
  return readByte(VEML6046_REG_ALS_INT, int_flag);
}

VEML6046Class veml6046(Wire);
