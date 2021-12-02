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

#include <Vishay_VEML6046.h>

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  Serial.println();

  if (!veml6046.begin())
  {
    Serial.println("Failed to initialize veml6046");
    while(1);
  }
  Serial.println("R\tG\tB\tIR\tLux");
}

void loop()
{
  uint16_t r, g, b, ir;
  if (veml6046.read_RGB(&r, &g, &b) && veml6046.read_IR(&ir))
  {
    Serial.print(r);
    Serial.print('\t');    
    Serial.print(g);
    Serial.print('\t');    
    Serial.print(b);
    Serial.print('\t');    
    Serial.print(ir);
    Serial.print('\t');    
    Serial.println(veml6046.get_lux());
  }

  delay(500);
}
