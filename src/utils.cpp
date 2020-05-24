/*******************************************************************************
 * Copyright (c) 2020, Kyle Robbertze
 *
 * This project is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * It is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this project. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
#include <Arduino.h>
#include <PCF8574.h>
#include "utils.h"
#include "defs.h"

int read_analogue_pin(byte p) {
// Take an averaged reading of analogue pin 'p'
  int i, val=0, nbr_reads=2;
  for (i=0; i<nbr_reads; i++) {
    val += analogRead(p);
    delay(1);
  }
  return val/nbr_reads;
}

void set_filters(unsigned long frequency, PCF8574& filterPCF) {
    Serial.println("Entering set_filters");
    byte filter_line=-1;

    if ((frequency >= FILTER_20_LB) && (frequency <= FILTER_20_UB)) {
        filter_line = 0;
    } else if ((frequency >= FILTER_30_LB) && (frequency <= FILTER_30_UB)) {
        filter_line = 1;
    } else if ((frequency >= FILTER_40_LB) && (frequency <= FILTER_40_UB)) {
        filter_line = 2;
    } else if ((frequency >= FILTER_80_LB) && (frequency <= FILTER_80_UB)) {
        filter_line = 3;
    }

    if (filter_line > -1) {
        Serial.print("Enabling filter line: ");
        Serial.println(filter_line);
        for (int i = 0; i < 8; ++i) filterPCF.write(i, 0); // Disable all filters
        delay(50); // Let I2C communication complete;
        filterPCF.write(filter_line, 1); // Enable active filter
    }
}
