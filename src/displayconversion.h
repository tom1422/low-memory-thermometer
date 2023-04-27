#pragma once

void dpc_fixed_frac_int_to_string(unsigned char frac, char* buffer) {
    unsigned short int n = 0b0000000011111111 & frac; //8.8 Fixed point binary. In denary: 0.8125
    int i = 0;

    while(true) //while(!((n & 0x8000)>>15))
    {
        //Mul by 10 to move 1st digit into 1's place
        //Mod by 10 on whole part. '0' converts to character
        unsigned short int oldn = n;
        n=n*10;

        if (n/10 != oldn) {
            break;
        }

        buffer[i++] = ((n>>8)%10+'0');
    }

    buffer[i] = '\0';
}

void dpc_int_to_string(unsigned int n, char* buffer) {
    int i = 0;

    while(n!=0) {
        buffer[i++] = n%10+'0';
        n=n/10;
    }

    buffer[i] = '\0';

    for(int j = 0; j < i/2; j++) {
        //Reverse array using XORs
        buffer[j] ^= buffer[i-j-1];
        buffer[i-j-1] ^= buffer[j];
        buffer[j] ^= buffer[i-j-1];
    } 

}