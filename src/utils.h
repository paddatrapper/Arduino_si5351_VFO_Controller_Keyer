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
#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>
#include <PCF8574.h>

// struct for 'VFO parameter set' records -- the parameters that will change with each VFO
typedef struct {
    boolean  active;
    uint32_t vfo;
    uint32_t radix;
} VFO_type;

int read_analogue_pin(byte p);
void set_filters(unsigned long frequency, PCF8574& filterPCF);
#endif
