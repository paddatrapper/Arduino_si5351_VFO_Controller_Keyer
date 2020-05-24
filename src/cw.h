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
#ifndef CW_H
#define CW_H
#include <Arduino.h>
#include <si5351.h>
#include "state.h"
#include "utils.h"

void init_cw(volatile VFO_type& vfo);
int step_cw(byte curr_msg_num, Si5351& si5351, volatile VFO_type& vfo);
int morse_lookup(char c);
byte check_keyer_pushbutton();
byte check_paddle();
void enable_key(bool activate, Si5351& si5351);
void send_dot(Si5351& si5351);
void send_dash(Si5351& si5351);
void send_key(bool enable, Si5351& si5351);
void send_letter_space();
void send_word_space();
void send_morse_char(char c, Si5351& si5351);
void play_message(String msg, Si5351& si5351, volatile VFO_type& vfo);

// morse reference table
struct MorseChar_t {
    char ch[7];
};
#endif
