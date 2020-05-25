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
#include "cw.h"
#include "receive.h"
#include "state.h"
#include "utils.h"

MorseChar_t morse_code[] = {
    {'A', '.', '-',  0,   0,   0,   0},
    {'B', '-', '.', '.', '.',  0,   0},
    {'C', '-', '.', '-', '.',  0,   0},
    {'D', '-', '.', '.',  0,   0,   0},
    {'E', '.',  0,   0,   0,   0,   0},
    {'F', '.', '.', '-', '.',  0,   0},
    {'G', '-', '-', '.',  0,   0,   0},
    {'H', '.', '.', '.', '.',  0,   0},
    {'I', '.', '.',  0,   0,   0,   0},
    {'J', '.', '-', '-', '-',  0,   0},
    {'K', '-', '.', '-',  0,   0,   0},
    {'L', '.', '-', '.', '.',  0,   0},
    {'M', '-', '-',  0,   0,   0,   0},
    {'N', '-', '.',  0,   0,   0,   0},
    {'O', '-', '-', '-',  0,   0,   0},
    {'P', '.', '-', '-', '.',  0,   0},
    {'Q', '-', '-', '.', '-',  0,   0},
    {'R', '.', '-', '.',  0,   0,   0},
    {'S', '.', '.', '.',  0,   0,   0},
    {'T', '-',  0,   0,   0,   0,   0},
    {'U', '.', '.', '-',  0,   0,   0},
    {'V', '.', '.', '.', '-',  0,   0},
    {'W', '.', '-', '-',  0,   0,   0},
    {'X', '-', '.', '.', '-',  0,   0},
    {'Y', '-', '.', '-', '-',  0,   0},
    {'Z', '-', '-', '.', '.',  0,   0},
    {'0', '-', '-', '-', '-', '-',  0},
    {'1', '.', '-', '-', '-', '-',  0},
    {'2', '.', '.', '-', '-', '-',  0},
    {'3', '.', '.', '.', '-', '-',  0},
    {'4', '.', '.', '.', '.', '-',  0},
    {'5', '.', '.', '.', '.', '.',  0},
    {'6', '-', '.', '.', '.', '.',  0},
    {'7', '-', '-', '.', '.', '.',  0},
    {'8', '-', '-', '-', '.', '.',  0},
    {'9', '-', '-', '-', '-', '.',  0},
    {'/', '-', '.', '.', '-', '.',  0},
    {'?', '.', '.', '-', '-', '.', '.'},
    {'.', '.', '-', '.', '-', '.', '-'},
    {',', '-', '-', '.', '.', '-', '-'}
};

// set canned CW messages for specific projects
String morse_msg[] = {"CQ ZS1KJ K", "DE ZS1KJ" };
//String morse_msg[] = {"1", "2" };  // for testing

unsigned int dot_dash_counter = 0; // total nbr CW chars sent since power-up
byte dot_length_ms = 100; // the CW keyer speed, lower is faster, 60 is 10 w.p.m.

void init_cw(volatile VFO_type& vfo) {
    Serial.println("Entering CW transmit state");
    digitalWrite(MUTE_LINE, 1); // Mute the receiver
    digitalWrite(TRANSMIT_LINE, 1); // enable transmit
    digitalWrite(CO_SUPPLY, 0);  // power down the carrier oscillator buffer
    refresh_display(0, vfo, STATE_Cw);
}

int step_cw(byte curr_msg_num, Si5351& si5351, volatile VFO_type& vfo) {
    if (curr_msg_num > 0) {
        play_message(morse_msg[curr_msg_num - 1], si5351, vfo);
        delay(5);
        return 0;
    }

    // see if the paddle is being pressed
    byte j = check_paddle();
    switch (j) {
        case PADDLE_BOTH:
            send_dot(si5351);
            send_dash(si5351);
            break;
        case PADDLE_L:
            send_dot(si5351);
            break;
        case PADDLE_R:
            send_dash(si5351);
            break;
        default:
            return -1;
    }
    return BREAK_IN_DELAY;
}

void play_message(String msg, Si5351& si5351, volatile VFO_type& vfo) {
    // sends the message in string 'm' as CW, with inter letter and word spacing
    char buff[30]; // magic number, should guard this!

    msg.toCharArray(buff, msg.length()+1);

    for (byte cw_msg_index=0; cw_msg_index < msg.length(); cw_msg_index++) {
        if (buff[cw_msg_index] == ' ') {
            send_word_space();
        } else {
            byte n = morse_lookup(buff[cw_msg_index]);
            if (n == -1) {
                // char not found, ignore it (but report it on Serial)
                Serial.print("!! not found <");
                Serial.print(buff[cw_msg_index]);
                Serial.println(">");
            } else {
                // char found, so send it as dots and dashes
                // Serial.println(n);
                refresh_display(0, vfo, STATE_Cw); // refresh the LCD so the current CW char gets displayed

                for(byte j = 1; j < 7; j++)
                    send_morse_char(morse_code[n].ch[j], si5351);
                send_letter_space();  // send an inter-letter space
                // if (s==0)read_keyer_speed();  // see if speed has changed mid message ***
            }
        }
    }
}

int read_keyer_speed() {
  int n = read_analogue_pin((byte)PIN_KEYER_SPEED);
  //Serial.print("Speed returned=");
  //Serial.println(n);
  //dot_length_ms = 60 + (n-183)/5;   // scale to wpm (10 wpm == 60mS dot length)
                                    // '511' should be mid point of returned range
                                    // change '5' to widen/narrow speed range...
                                    // smaller number -> greater range
  return n;
};

int morse_lookup(char c) {
    // returns the index of parameter 'c' in MorseCode array, or -1 if not found
    for(int i = 0; i < sizeof(morse_code); i++) {
        if (c == morse_code[i].ch[0])
            return i;
    }
    return -1;
};

byte check_keyer_pushbutton() {
    // Reads the keyer pushbuttons and returns the button number as a byte;
    // 0 if not pressed
    byte b = 0;
    int z = read_analogue_pin(PIN_KEYER_MEMORY); // read the analog pin

    // reading is from the rear keyer pushbuttons
    // open (USB power: 840) (LiFePO+7812: 1023)
    if (z > 450 && z < 990) b = 1;       // L    (USB power: 727) (LiFePO+7812: 712)
    else if (z > 10 && z <= 450) b = 2;  // R    (USB power: 318) (LiFePO+7812: 122)
    // both: (USB power:  66) (LiFePO+7812: 112)

    return b;
}

byte check_paddle() {
    // Reads the paddle, returns the paddle number as a byte; 0 if not pressed
    // 1 if left is pressed, 2 if right is pressed and 3 if both are pressed
    byte l = read_push_button(PADDLE_L_PUSHBTTN, LOW);
    byte r = read_push_button(PADDLE_R_PUSHBTTN, LOW);
    return r<<1 | l;
};

void enable_key(bool enable, Si5351& si5351) {
    // push the morse key down, or let it spring back up

    if (enable) {
        // do whatever you need to key the transmitter
        digitalWrite(13, 1);  // turn the Nano's LED on
        si5351.output_enable(SI5351_CLK1, 1); // turn the (CW clock) on
    } else {
        // do whatever you need to un-key the transmitter
        digitalWrite(13, 0);  // turn the Nano's LED off
        si5351.output_enable(SI5351_CLK1, 0); // turn of (CW freq) clock off
    };
}

void send_dot(Si5351& si5351) {
    delay(dot_length_ms);  // wait for one dot period (space)

    // send a dot and the following space
    tone(PIN_TONE_OUT, CW_TONE_HZ);
    enable_key(true, si5351);
    if (dot_dash_counter % SERIAL_LINE_WIDTH == 0) Serial.println();
    Serial.print(".");
    delay(dot_length_ms);  // key down for one dot period

    enable_key(false, si5351);
    dot_dash_counter++;
}

void send_dash(Si5351& si5351) {
    delay(dot_length_ms);  // wait for one dot period (space)
    // send a dash and the following space
    tone(PIN_TONE_OUT, CW_TONE_HZ);
    enable_key(true, si5351);
    if (dot_dash_counter % SERIAL_LINE_WIDTH == 0) Serial.println();
    Serial.print("-");
    delay(dot_length_ms * CW_DASH_LEN);  // key down for CW_DASH_LEN dot periods

    enable_key(false, si5351);
    dot_dash_counter++;
}

void send_letter_space() {
    delay(dot_length_ms * 4);  // wait for 3 dot periods
    Serial.print(" ");
}

void send_word_space() {
    delay(dot_length_ms * 7);  // wait for 6 dot periods
    Serial.print("  ");
}

void send_morse_char(char c, Si5351& si5351) {
    // 'c' is a '.' or '-' char, so send it
    if (c == '.') send_dot(si5351);
    else if (c == '-') send_dash(si5351);
    // ignore anything else, including 0s
}

