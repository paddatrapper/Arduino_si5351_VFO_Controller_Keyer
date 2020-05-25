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
#include <si5351.h>
#include "defs.h"
#include "display.h"
#include "receive.h"
#include "state.h"
#include "utils.h"

void init_receive(Si5351& si5351, volatile VFO_type& vfo) {
    Serial.println("Entering receieve state");
    digitalWrite(TRANSMIT_LINE, 0);    // drop out the T/R relay
    delay(10);
    digitalWrite(MUTE_LINE, 0);        // un-mute the receiver
    digitalWrite(TX_SSB_MUTE_LINE, 0); // mute the mic amp to silence T/R squeal
    si5351.output_enable(SI5351_CLK0, 1);  // turn the VFO on
    si5351.output_enable(SI5351_CLK2, 1);  // turn the BFO on
    digitalWrite(CO_SUPPLY, 0);        // turn the carrier oscillator buffer off

    set_oscillator(si5351, vfo);
}

void step_receive(Si5351& si5351, volatile VFO_type& vfo) {
    refresh_display(0, vfo, STATE_Rx);
    if (read_push_button(ENCODER_PUSHBTTN, LOW)) {
        vfo.radix *= 10;
        if (vfo.radix > 10000) vfo.radix = 10;
    }
}

void set_oscillator(Si5351& si5351, volatile VFO_type& vfo) {
    unsigned long long bfo;
    if (vfo.vfo >= SIDEBAND_THRESHOLD)
        bfo = USB;
    else
        bfo = LSB;

    si5351.set_freq((vfo.vfo + bfo) * SI5351_FREQ_MULT, SI5351_CLK0); // set CLK0 to VFO freq for current band

    if (vfo.vfo < VFO_DRIVE_THRESHOLD)
        si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
    else
        si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_4MA);

    si5351.set_freq(bfo * SI5351_FREQ_MULT, SI5351_CLK2); // set CLK2 to the BFO frequency for the current band
    si5351.drive_strength(SI5351_CLK2,SI5351_DRIVE_2MA);  // can change this for a stronger BFO
}

byte check_band_pushbutton() {
    // Reads the keyer pushbuttons and returns the button number as a byte;
    // 0 if not pressed
    byte b = 0;
    int z = read_analogue_pin(PIN_BAND_PRESET); // read the analog pin

    // reading is from the band preset pushbuttons
    // open (USB power: 850) (LiFePO+7812: 1023)
    if (z > 450 && z < 990) b = 1;       // L    (USB power: 592) (LiFePO+7812: 712)
    else if (z > 10 && z <= 450) b = 2;  // R    (USB power:  93) (LiFePO+7812: 122)
    // both: (USB power:  66) (LiFePO+7812: 112)

    return b;
}
