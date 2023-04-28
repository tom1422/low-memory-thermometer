#pragma once
#include <Arduino.h>

#define delayus_fast    45      //Used for fast 8 bit command execution speed
#define delayus_slow    1620    //Used for slow 8 bit command execution speed
#define delayus_cmd     50      //Used for first 4 bit transmission speed
#define RS 3
#define E  4
#define D4 5
#define D5 6
#define D6 7
#define D7 8

unsigned int cursorPosition = 0; //Max: 80. 0 to 39 represents first line and 40 to 79 represent second line

void disp_cursorMove(unsigned int location);
void disp_resetCursor();
void disp_run8BitCommand(byte cmd);
void disp_run8BitCommand_RS(byte cmd);
void disp_run4BitCommand(byte cmd, unsigned int execTime);
void disp_clockDown();
void disp_clockUp();
void disp_writeNumericalString(char* characters, int bluff);

void disp_setup() {
    pinMode(RS, OUTPUT);
    pinMode(E , OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    digitalWrite(RS, LOW);

    delay(100);

    //Run 3 FUNCTION SET commands to put in 8 bit mode
    disp_run4BitCommand(0b00000011, delayus_slow);
    disp_run4BitCommand(0b00000011, delayus_slow);
    disp_run4BitCommand(0b00000011, delayus_slow);

    //Put back in 4 bit mode (0b0010)
    disp_run4BitCommand(0b00000010, delayus_slow);

    //Set to 4 bit mode and 2 line
    disp_run8BitCommand(0b00101000);

    //Turn on display, turn on cursor, blink cursor 00001DCB 
    //D – 0 = display-off, 1 = display on; 
    //C – 0 = cursor off, 1 = cursor on; 
    //B – 0 = cursor blink off, 1 = cursor blink on;
    disp_run8BitCommand(0b00001110);

    //Clear display
    disp_run8BitCommand(0b00000001);

    //Return home
    disp_run8BitCommand(0b00000010);

    disp_run8BitCommand_RS(0b01010100); //Capital T
    disp_run8BitCommand_RS(0b01100101); //e
    disp_run8BitCommand_RS(0b01101101); //m
    disp_run8BitCommand_RS(0b01110000); //p
    disp_run8BitCommand_RS(0b00111010); //:

    disp_cursorMove(8);

    disp_run8BitCommand_RS(0b00101110); //.

    disp_cursorMove(14);

    disp_run8BitCommand_RS(0b11011111); //Deg
    disp_run8BitCommand_RS(0b01000011); //C

    disp_cursorMove(40);

    disp_run8BitCommand_RS(0b01001000); //Capital H
    disp_run8BitCommand_RS(0b01110101); //u
    disp_run8BitCommand_RS(0b01101101); //m
    disp_run8BitCommand_RS(0b01100100); //d
    disp_run8BitCommand_RS(0b00111010); //:

    disp_cursorMove(48);

    disp_run8BitCommand_RS(0b00101110); //.

    disp_cursorMove(53);

    disp_run8BitCommand_RS(0b00100101); //%
    disp_run8BitCommand_RS(0b01010010); //R
    disp_run8BitCommand_RS(0b01001000); //H
}

void disp_cursorMove(unsigned int location) {
    for (cursorPosition; cursorPosition < location; cursorPosition++) {
        //Shift right
        disp_run8BitCommand(0b00010100);
    }
    for (cursorPosition; cursorPosition > location; cursorPosition--) {
        //Shift left
        disp_run8BitCommand(0b00010000);
    }
    // Serial.print("Cursor PositionM: ");
    // Serial.println(cursorPosition);
}

void disp_resetCursor() {
    cursorPosition = 0;
    //Home cursor !SLOW COMMAND!
    disp_run8BitCommand(0b00000010);
}

void disp_run8BitCommand(byte cmd) {
    disp_run4BitCommand(cmd>>4, delayus_cmd);
    //All commands take ~37us apart from 0b00000001 and 0b0000001X which take ~1520us
    if ((cmd == 0x01) || ((cmd & 0xFE) == 0x02)) {
        disp_run4BitCommand(cmd, delayus_slow);
    } else {
        disp_run4BitCommand(cmd, delayus_fast);
    }
}

void disp_run8BitCommand_RS(byte cmd) {
    cursorPosition++;
    digitalWrite(RS, HIGH);
    disp_run8BitCommand(cmd);
    digitalWrite(RS, LOW);
    // Serial.print("Cursor PositionR: ");
    // Serial.println(cursorPosition);
}

void disp_run4BitCommand(byte cmd, unsigned int execTime) {
    disp_clockDown();
    digitalWrite(D7, bitRead(cmd, 3));
    digitalWrite(D6, bitRead(cmd, 2));
    digitalWrite(D5, bitRead(cmd, 1));
    digitalWrite(D4, bitRead(cmd, 0));
    disp_clockUp();
    delayMicroseconds(execTime);
}

void disp_clockDown() {
    digitalWrite(E, HIGH);
}

void disp_clockUp() {
    digitalWrite(E, LOW);
}

void disp_writeNumericalString(char* characters, int bluff) {
    //Go through each character and convert to display bit
    int i = 1;
    unsigned char cur_char = characters[0];
    while (cur_char != '\0') {
        cur_char -= 48;
        disp_run8BitCommand_RS(0b00110000 | (0x0F & cur_char));
        cur_char = characters[i++];
        if (i > bluff) {
            break;
        }
    }

    i-=1;
    for (i; i < bluff; i++) {
        disp_run8BitCommand_RS(0b00100000);
    }
    
}