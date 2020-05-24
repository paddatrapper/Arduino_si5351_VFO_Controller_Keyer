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
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>
#include "display.h"
#include "defs.h"
#include "state.h"
#include "s_meter_font.h"
#include "utils.h"

SSD1306AsciiAvrI2c oled;

void init_display() {
    oled.begin(&Adafruit128x64, I2C_OLED_ADDRESS);
    oled.setFont(fixed_bold10x15);
    oled.clear();
    oled.println("   Summit     ");
    oled.println(" Prowler_VI");
    oled.println(" < ZS1KJ > ");
    oled.println("R1.4 11/2018");
    //  oled.println();
    delay(1000);
    oled.clear();
}

void refresh_display(int pwr_val, const volatile VFO_type& vfo, const State& state) {
#ifdef DIAGNOSTIC_DISPLAY
    if (diagnostic_flag) {
        oled.clear();
        oled.set1X();
        oled.setCursor(0, 0);
        oled.print(diagnostic_int, DEC);
        return;
    };
#endif  // DIAGNOSTIC_DISPLAY

    uint16_t f = vfo.vfo / 1000;   // frequency in whole Hz

    oled.set2X();
    oled.setCursor(0, 0);

    int mhz = f / 1000;                       // MHz (0..99)
    int khz = f - mhz * 1000;                 // kHz (0..999)
    int hz = vfo.vfo - (mhz * 1000000 + khz * 1000);  // Hz (0..999)
    //  Serial.print(" hz="); Serial.println(hz);
    if (mhz < 10) oled.print(" ");
    oled.print(mhz, DEC);
    oled.print(",");
    if (khz < 10) oled.print("0");
    if (khz < 100) oled.print("0");
    oled.println(khz, DEC);

    oled.set1X();
    oled.setCursor(0, 4);

    switch (state) {
        case STATE_Rx:
            if (vfo.radix==10000) oled.print("10kHz ");
            if (vfo.radix==1000)  oled.print("1kHz  ");
            if (vfo.radix==100)   oled.print("100Hz ");
            if (vfo.radix==10)    oled.print("10Hz  ");
            break;
        case STATE_Cw:
            oled.print("CW");
            break;
        case STATE_Tune:
            oled.print("Tune");
            break;
        case STATE_Ssb:
            oled.print("Tx");
            break;
    }

    oled.setCursor(84, 4);  // was 96, 4
    oled.print(".");
    if (hz<10) oled.print("0");
    if (hz<100) oled.print("0");
    oled.println(hz, DEC);
    oled.setCursor(0, 7);

    int val = 0, s = 0;
    if (state == STATE_Rx) {
        oled.setCursor(30, 6);
        oled.print("S");
        oled.setFont(s_meter10x15);

        // read and format the S-meter
        //    byte s = (rand()%2)+2;
        val = analogRead(PIN_S_METER);

        // do band-specific sensitivity scaling
        if ((vfo.vfo >= FILTER_80_LB) && (vfo.vfo <= FILTER_80_UB)) val = val*3;   // '2' is the magic number for 80m

        s = map(val, 0, 550, 8, 1);  // map s-meter analogue reading to s-scale

        // now add some random movement (make the meter flicker up by 1 S-point to animate it!)
        s = s + 1 + rand()%2;

        //    Serial.print(" s=");  Serial.print(s);  Serial.print(" s2=");  Serial.println(s2);
        byte c = 0;
        for (byte j = 1; j <= s; j++) {
            oled.print((char)j);
            c++;
        };
        oled.setFont(fixed_bold10x15);
        for (byte k = 8; k > c; k--) oled.print(" "); // right pad the s-meter space
    } else {
        // in transmit mode, so display a relative RF power meter
        if (state == STATE_Tune || state == STATE_Ssb) {
            pwr_val = read_analogue_pin(PIN_PWR_METER);
            //      Serial.print(" refresh_OLED():: pwr_val=");  Serial.println(pwr_val);
        }
        refresh_pwr_meter(pwr_val);
    };
}

void refresh_pwr_meter(const int pwr_val) {
    // write to the power meter line on the OLED
    oled.setCursor(30, 6);
    oled.print("P");
    oled.setFont(s_meter10x15);

    // do band-specific sensitivity scaling
    //    if ((VFOSet[v].vfo >= FILTER_80_LB) && (VFOSet[v].vfo <= FILTER_80_UB)) val = val*3;   // '2' is the magic number for 80m

    // NOTE: the RF power sensing analog pin is read elsewhere, and the value stored in global pwr_val
    byte s = map(pwr_val, 0, 550, 1, 8);

    byte c = 0;
    for(byte j = 1; j <= s; j++){
        oled.print((char)j);
        c++;
    };
    oled.setFont(fixed_bold10x15);
    for(byte k = 8; k > c; k--)
        oled.print(" "); // right pad the pwr-meter display space
}
