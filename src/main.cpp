#include <Arduino.h>

#define readingtimeout 1000

#include <displayconversion.h>
#include <i2csensor.h>
#include <hd44780.h>

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  
  i2csensor_setup();
  disp_setup();
}

void loop() {
  delay(readingtimeout);

  unsigned char buffer[6];
  i2csensor_i2c_write_cmd(0x44, 0xFD);
  i2csensor_read_nbytes(0x44, 6, buffer);

  unsigned long int tempconversion = 0x0000FFFF & ((buffer[0] << 8) + buffer[1]);
  tempconversion *= 175;
  tempconversion -= 2949075;
  //Now we have 16.16 fixed point integer. Chop off last 8 bits to convert to 8.8 bit integer
  unsigned char wholeTemperature = (tempconversion>>16);
  unsigned char fracTemperature = (tempconversion>>8);
  char wholeTemperatureText[8];
  char fracTemperatureText[8];
  dpc_int_to_string(wholeTemperature, wholeTemperatureText);
  dpc_fixed_frac_int_to_string(fracTemperature, fracTemperatureText);

  disp_cursorMove(6);
  disp_writeNumericalString(wholeTemperatureText, 2);
  disp_cursorMove(9);
  disp_writeNumericalString(fracTemperatureText, 5);

  // Serial.println(wholeTemperatureText);
  // Serial.println(fracTemperatureText);


  unsigned long int humdconvserion = 0x0000FFFF & ((buffer[3]<<8) + buffer[4]);
  humdconvserion *= 125;
  humdconvserion -= 393210;
  unsigned char wholeHumidity = (humdconvserion>>16);
  unsigned char fracHumidity = (humdconvserion>>8);
  char wholeHumidityText[8];
  char fracHumidityText[8];
  dpc_int_to_string(wholeHumidity, wholeHumidityText);
  dpc_fixed_frac_int_to_string(fracHumidity, fracHumidityText);

  disp_cursorMove(46);
  disp_writeNumericalString(wholeHumidityText, 2);
  disp_cursorMove(49);
  disp_writeNumericalString(fracHumidityText, 4);

  // Serial.println(wholeHumidityText);
  // Serial.println(fracHumidityText);
}