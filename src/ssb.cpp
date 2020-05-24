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

void init_ssb(Si5351& si5351, volatile VFO_type& vfo) {
    digitalWrite(MUTE_LINE, 1); // Mute the receiver
    delay(1);
}

void step_ssb(VFO_type& vfo) {

}

byte check_ptt_pushbuttom() {
    return !digitalRead(PTT_SENSE);
}
