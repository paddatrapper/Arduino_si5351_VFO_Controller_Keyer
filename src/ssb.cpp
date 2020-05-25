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
#include "ssb.h"
#include <si5351.h>
#include "defs.h"
#include "display.h"
#include "state.h"
#include "utils.h"

void init_ssb(Si5351& si5351, volatile VFO_type& vfo) {
    Serial.println("Entering SSB transmit state");
    digitalWrite(MUTE_LINE, 1); // Mute the receiver
    digitalWrite(TRANSMIT_LINE, 1); // enable transmit
    delay(TX_SSB_MUTE_DELAY);
    digitalWrite(TX_SSB_MUTE_LINE, 1); // un-mute the mic amp
    delay(1);
    refresh_display(0, vfo, STATE_Ssb);
}

void step_ssb(volatile VFO_type& vfo) {
    refresh_display(0, vfo, STATE_Ssb);
}

byte check_ptt_pushbuttom() {
    return read_push_button(PTT_SENSE, LOW);
}
