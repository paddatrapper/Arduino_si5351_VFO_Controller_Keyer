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
#include <EEPROM.h>
#include <PCF8574.h>
#include <si5351.h>
#include <Rotary.h>
#include <Wire.h>
#include "defs.h"
#include "display.h"
#include "cw.h"
#include "state.h"
#include "ssb.h"
#include "receive.h"
#include "utils.h"

State state;
Si5351 si5351;
PCF8574 filterPCF { I2C_DEMUX };
Rotary rotary_encoder { ENCODER_A, ENCODER_B };
volatile VFO_type vfos[NBR_VFOS]; // array of band preset sets
byte active_preset;      // index for the current active VFO preset
uint32_t current_frequency;

void set_frequency(unsigned char direction) {
    if (direction == DIR_CW) {
        vfos[active_preset].vfo += vfos[active_preset].radix;
    } else if (direction == DIR_CCW) {
        vfos[active_preset].vfo -= vfos[active_preset].radix;
    }
}

/**************************************/
/* Interrupt service routine for encoder frequency change */
/**************************************/
void encoder_change() {
    unsigned char result = rotary_encoder.process();
    if (state != STATE_Rx) {
        return;
    }
    set_frequency(result);
}

void setup() {
    Serial.begin(9600);
    Wire.begin();

    Serial.println("SP_VI ...");
    init_display();

    // set digital and analogue pins
    pinMode(TX_SSB_MUTE_LINE, OUTPUT); // mute the mic amp line, goes low after SSB PTT
    pinMode(PIN_BAND_PRESET, INPUT_PULLUP); // switch bank is Pull-up
    pinMode(PTT_SENSE, INPUT_PULLUP);  // senses the PTT switch in the microphone, normally open, grounded when PTT pressed
    pinMode(CO_SUPPLY, OUTPUT);  // digital pin to control DC power to the carrier osc buffer
    pinMode(ENCODER_PUSHBTTN, INPUT_PULLUP);
    pinMode(PADDLE_L_PUSHBTTN, INPUT_PULLUP);
    pinMode(PADDLE_R_PUSHBTTN, INPUT_PULLUP);
    pinMode(PIN_KEYER_MEMORY, INPUT);

    filterPCF.begin(0x00);

    pinMode(MUTE_LINE, OUTPUT);
    digitalWrite(MUTE_LINE, 0);     // put the mute line low (un-mute)
    pinMode(TRANSMIT_LINE, OUTPUT);
    digitalWrite(TRANSMIT_LINE, 0); // put the transmit line low (relay not energised)

    // TODO: use digitalPinToInterrupt when Arduino library updated in Debian
    attachInterrupt(0, encoder_change, CHANGE);
    attachInterrupt(1, encoder_change, CHANGE);

    // load up VFO array from EEPROM
    active_preset = EEPROM.read(0);
    Serial.print("setup() eeprom: active_preset=");
    Serial.println(active_preset);
    if (active_preset >= NBR_VFOS) active_preset = 1; // in case NBR_VFOS has been reduced since the last run (EEPROM update)

    int element_len = sizeof(VFO_type);
    for(int i=0; i < NBR_VFOS; i++) {
        VFO_type vfo; // Ensures volatile variable write is correct
        EEPROM.get(1 + (i * element_len), vfo);
        vfos[i].active = vfo.active;
        vfos[i].radix = vfo.radix;
        vfos[i].vfo = vfo.vfo;
    };

    // dump out the VFOset array for diagnostics
    for(int n = 0; n < NBR_VFOS ; ++n) {
        Serial.print((int)vfos[n].active);
        Serial.print(" ");
        Serial.print(vfos[n].vfo);
        Serial.print(" ");
        Serial.print((long)vfos[n].radix);
        Serial.println();
    }

    // initialise and start the si5351 clocks
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0); // 0 is the default xtal freq of 25Mhz
    si5351.set_correction(19100, SI5351_PLL_INPUT_XO); // to determine the correction using the serial monitor
    si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);

    state = STATE_Rx;
    init_receive(si5351, vfos[active_preset]);
}

void loop() {
    if (current_frequency != vfos[active_preset].vfo) {
        current_frequency = vfos[active_preset].vfo;
        // Strip decimals below sensitivity of radix
        uint16_t f = vfos[active_preset].vfo % vfos[active_preset].radix;
        vfos[active_preset].vfo -= f;
        set_filters(vfos[active_preset].vfo, filterPCF);
        set_oscillator(si5351, vfos[active_preset]);
        EEPROM.write(0, active_preset);   // write the band index to the first byte

        int element_len = sizeof(VFO_type);
        for(int i=0; i<NBR_VFOS ; i++) { // write each element of the VFOSet array
            EEPROM.put(1 + (i * element_len), vfos[i]);
        }
    }
    byte cw_msg_num = check_keyer_pushbutton();
    byte paddles = check_paddle();
    if (cw_msg_num > 0 || paddles > 0) {
        state = STATE_Cw;
        init_cw(vfos[active_preset]);
    }

    byte ptt = check_ptt_pushbuttom();
    if (ptt && state != STATE_Ssb) {
        // PTT pressed
        state = STATE_Ssb;
        init_ssb(si5351, vfos[active_preset]);
    } else if (!ptt && state == STATE_Ssb) {
        // PTT released
        state = STATE_Rx;
        init_receive(si5351, vfos[active_preset]);
    }

    byte preset_button = check_band_pushbutton();
    if (preset_button > 0) {
        switch (preset_button) {
            case 1:
                ++active_preset;
                break;
            case 2:
                --active_preset;
                break;
        }
        if (active_preset < 0) active_preset = NBR_VFOS - 1;
        if (active_preset >= NBR_VFOS) active_preset = 0;
        set_filters(vfos[active_preset].vfo, filterPCF);
        set_oscillator(si5351, vfos[active_preset]);
    }

    switch (state) {
        case STATE_Rx:
            step_receive(si5351, vfos[active_preset]);
            break;
        case STATE_Cw:
            {
                int delay = step_cw(cw_msg_num, si5351, vfos[active_preset]);
                while (delay > 0) {
                    unsigned long start = millis();
                    cw_msg_num = check_keyer_pushbutton();
                    int new_delay = step_cw(cw_msg_num, si5351, vfos[active_preset]);
                    if (new_delay == -1) delay -= millis() - start;
                    else delay = new_delay;
                }
                state = STATE_Rx;
                init_receive(si5351, vfos[active_preset]);
                break;
            }
        case STATE_Ssb:
            step_ssb(vfos[active_preset]);
            break;
        case STATE_Tune:
            break;
    }
}
