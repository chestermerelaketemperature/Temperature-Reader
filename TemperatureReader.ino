/*****************************
  Bluetooth Temperature Reader

  Created by the ChestermereLakeTemperature.com team
  https://github.com/chestermerelaketemperature

  If you modify this file please keep a reference to our GitHub.
*****************************/

//OneWire documentation: https://www.pjrc.com/teensy/td_libs_OneWire.html
//http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
#include <OneWire.h>
int pin = 2; //wire18S20 Signal pin on digital pin 2.
OneWire wire(pin); //Temperature chip i/o.
int counter = 0;
int incomingByte;

void setup() {
    Serial.begin(115200);
    Serial.println("Started on Serial 115200");
}

void loop() {
    float temperature = getTemperature();
    if (temperature > -100) // There was an error reading the temperature.
        Serial.println(temperature);
    delay(500);
}

float getTemperature() {
    //returns the temperature from one wire18S20 in DEG Celsius
    byte data[12];
    byte address[8];
    if (!wire.search(address)) {
        //no more sensors on chain, reset search.
        wire.reset_search();
        return -1000;
    }
    if (OneWire::crc8(address, 7) != address[7]) {
        Serial.println("CRC is not valid!");
        return -1000;
    }
    if (address[0] != 0x10 && address[0] != 0x28) {
        Serial.print("Device is not recognized");
        return -1000;
    }
    wire.reset();
    wire.select(address);
    wire.write(0x44,1); //start conversion, with parasite power on at the end.
    wire.reset();
    wire.select(address);
    wire.write(0xBE); //Read Scratchpad
    for (int i = 0; i < 9; i++) { //We need to read 9 bytes.
        data[i] = wire.read();
    }
    wire.reset_search();
    byte MSB = data[1];
    byte LSB = data[0];
    float temperatureRead = ((MSB << 8) | LSB); //Using two's compliment.
    float temperatureSum = temperatureRead / 16;
    return temperatureSum;
}
