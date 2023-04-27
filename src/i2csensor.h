//Class to handle all i2c communication details for SHT40 sensor and TWI registers
#pragma once
#include <Arduino.h>

byte i2csensor_i2c_write_cmd(byte address, byte cmd);
byte i2csensor_sendStopCondition();
byte i2csensor_master_receiver_startRead(byte address);
void i2csensor_read_nbytes(byte address, uint8_t nbytes, unsigned char* buffer);
void i2csensor_read_byte_to_buffer(bool ack, unsigned char* buffer, uint8_t index);
void i2csensor_setupPrescaler();

void i2csensor_setup() {
    //Setup i2c sensor stuff
    i2csensor_setupPrescaler();
}

byte i2csensor_i2c_write_cmd(byte address, byte cmd) {
    //START TRANSMISSION
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    if ((TWSR & 0xF8) != 0x08)
        return (TWSR & 0xF8);

    //SLA-W
    TWDR = (address<<1);
    TWCR = (1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    if ((TWSR & 0xF8) != 0x18) //Could be 0x18 (ACK) 0x20 (NACK) 0x38 (Arbitration Lost)
        return (TWSR & 0xF8);

    //SLA-W DATA
    TWDR = cmd;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT))); //Wait for bit to be 1 again

    if ((TWSR & 0xF8) != 0x28) //Could be 0x28 (ACK) 0x30 (NACK) 0x38 (Arbitration Lost)
        return (TWSR & 0xF8);

    i2csensor_sendStopCondition();

    return 0;
}

byte i2csensor_sendStopCondition() {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    return 0;
}

byte i2csensor_master_receiver_startRead(byte address) {
    //START TRANSMISSION
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    if (!((TWSR & 0xF8) == 0x08 || (TWSR & 0xF8) == 0x10)) //0x08 and 0x10 are START and REPEATED START
        return (TWSR & 0xF8);

    //SLA-R
    TWDR = (address<<1) | (0x01); // Read command at "address"
    TWCR = (1<<TWINT)|(1<<TWEN); //Send data in TWDR
    while(!(TWCR & (1<<TWINT)));

    if ((TWSR & 0xF8) != 0x40) //Could be 0x40 (ACK) 0x48 (NACK) 0x38 (Arbitration Lost)
        return (TWSR & 0xF8);

    return (TWSR & 0xF8);
}


void i2csensor_read_nbytes(byte address, uint8_t nbytes, unsigned char* buffer) {
    byte result;
    for (int i = 0; i < 20; i++) {
        delay(5);
        result = i2csensor_master_receiver_startRead(address);
        if (result != 0x48) {
            //Fall through straight away. If a NACK, it has to keep retrying
            break;
        }
    }

    if (result != 0x40) {
        i2csensor_sendStopCondition();
        return; //Not an ACK, could be NACK or other error, return.
    }

    for (uint8_t index = 0; index < nbytes; index++) {
        i2csensor_read_byte_to_buffer((index < (nbytes-1)) ? 1 : 0, buffer, index);
    }

    i2csensor_sendStopCondition();

    return;
}


void i2csensor_read_byte_to_buffer(bool ack, unsigned char* buffer, uint8_t index) {
    if (ack) {
        TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    } else {
        TWCR = (1<<TWINT)|(1<<TWEN);
    }

    while(!(TWCR & (1<<TWINT)));

    buffer[index] = TWDR;
}

void i2csensor_setupPrescaler() {
    byte twsr = TWSR;
    byte twsr_wbitmask = twsr & 0x03;
    byte prescaler_val = 1<<(twsr_wbitmask << 1);
    int twbr_shift_amount = 5 - (twsr_wbitmask << 1);
    if (twbr_shift_amount < 0) {
        // Set TWSR to 0bXXXXXX00
        TWSR = TWSR & 0b11111100;
        i2csensor_setupPrescaler();
        return;
    }
    TWBR = (1 << twbr_shift_amount);
}