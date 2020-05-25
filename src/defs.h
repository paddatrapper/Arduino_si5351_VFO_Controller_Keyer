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
#ifndef DEFS_H
#define DEFS_H
#define SP_VI

#define BUTTON_HELD_MS 700  // button down period in mS required to activate 2nd pushbutton function
#define TUNE_MS       4000  // tune (key down) period (mS)
#define LPF_DROPOUT_DELAY 8000  // period of inactivity before the LPF drops out to save battery (mS)
#define FAN_DROPOUT_DELAY  5000  // period of inactivity before the fan drops out to save battery (mS)
#define VFO_DRIVE_THRESHOLD 18000  // threshold freq above which a higher si5351 clock output level on the VFO clock is used
#define CO_DRIVE_THRESHOLD  18000  // threshold freq above which a higher si5351 clock output level on the carrier oscillator (CW) clock is used


#define ENCODER_MECHANICAL
#define BFO_ENABLED
#define BFO_TUNE_LO    11990000ull  // lowest BFO frequency
#define BFO_TUNE_HI    12005000ull  // highest BFO frequency
#define USB 11998500ull  // the reference BFO freq for USB, may be tuned, put in EEPROM once
#define LSB 11996000ull  // the reference BFO freq for LSB, may be tuned, put in EEPROM once
#define SIDEBAND_THRESHOLD  10000000ull  // threshold VFO freq for auto sideband selection: above use USB, below use LSB
#define DISPLAY_OLED
#define OLED_128X64
#define I2C_OLED_ADDRESS 0x3D
#define RST_PIN -1     // Define proper RST_PIN if required
#define NBR_VFOS 10    // number of selectable VFOs
#define CW_KEYER       // include the CW keyer code
// #define DIAGNOSTIC_DISPLAY  // allow use of the OLED for tracing simple values (eg button values)
#define TX_SSB_MUTE_LINE 9    // D9 is used to mute the mic amp to silence a T/R squeal
                              // normally high, this line goes low TX_SSB_MUTE_DELAY mS after PTT when transmitting SSB
#define TX_SSB_MUTE_DELAY 350 // delay (mS) before the mic amp is unmuted after SSB PTT
#define CO_SUPPLY  8          // this pin controls a high side DC switch to enable the carrier oscillator when keyed

// Arduino Nano digital pin assignments (aligns with Raduino)
#define SERIAL_TX 0  // Serial
#define SERIAL_RX 1  // Serial
#define ENCODER_B 2  // Encoder pin B
#define ENCODER_A 3  // Encoder pin A
#define PTT_SENSE 4  // sense the PTT button being pressed (low == transmit)
#define MUTE_LINE 5  // receiver mute (high == mute)
#define TRANSMIT_LINE 7 // controls the T/R relay (high == transmit)
#define ENCODER_PUSHBTTN 10 // Encoder push button
#define PADDLE_R_PUSHBTTN 11 // Right paddle
#define PADDLE_L_PUSHBTTN 12 // Left paddle

// Arduino Nano analogue pins
#define PIN_BAND_PRESET  A0 // front panel push buttons
//                       A1 // paddle on analog pin 1
#define PIN_KEYER_MEMORY A2 // keyer memory pushbuttons on analog pin 2
#define PIN_KEYER_SPEED  A3 // speed potentiometer wiper
#define PIN_S_METER      A3 // s-meter TBA
//                       A4    SDA
//                       A5    SCL
#define PIN_PWR_METER    A6 // analogue pin for relative RF sensing circuit

// frequency ranges for automatic band pass and low pass filter switching
#define FILTER_80_LB  2501000ull  // Filter set lower bound ( 2.501 MHz)
#define FILTER_80_UB  4000000ull  // Filter set upper bound ( 4.000 MHz)

#define FILTER_40_LB  6501000ull  // Filter set lower bound ( 6.501 MHz)
#define FILTER_40_UB  8000000ull  // Filter set upper bound ( 8.000 MHz)

#define FILTER_30_LB  8001000ull  // Filter set lower bound ( 8.001 MHz)
#define FILTER_30_UB 12000000ull  // Filter set upper bound (12.000 MHz)

#define FILTER_20_LB 12001000ull  // Filter set lower bound (12.001 MHz)
#define FILTER_20_UB 16000000ull  // Filter set upper bound (16.000 MHz)

// i2c devices and addresses:
// si5351  x60
// filter selector PCF8574
#define I2C_DEMUX   0x74         // I2C address of the filter PCF8574

// CW
#define PIN_TONE_OUT       6  // digital pin with keyed audio tone on it
#define CW_TONE_HZ       700  // CW tone frequency (Hz)

#define PADDLE_L           1  // value representing paddle left (dot)
#define PADDLE_R           2  // value representing paddle right (dash)
#define PADDLE_BOTH        3  // value representing both paddles (alternating)

#define CW_DASH_LEN        3  // length of dash (in dots)
#define BREAK_IN_DELAY   800  // break-in hang time (ms)
#define SERIAL_LINE_WIDTH 80  // number of morse chars on Serial after which we newline

#define PUSHBTTN_DEBOUNCE_DELAY 25 // length of debounce delay (ms)
#endif
