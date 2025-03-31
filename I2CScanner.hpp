#ifndef __I2CScanner__
#define __I2CScanner__

// #include <Arduino.h>
#include <Wire.h>

class I2CScanner {
public:
  static void scan(TwoWire& wire) {
    Serial.printf("Scanning...\n");
    uint8_t nDevices = 0;

    for (uint8_t address = 1; address < 127; address++) {
      wire.beginTransmission(address);
      uint8_t error = wire.endTransmission();

      if (error == 0) {
        Serial.printf("I2C device found at address 0x%02X\n", address);
        nDevices++;
      }
      else if (error == 4) {
        Serial.printf("Unknown error at address 0x%02X\n", address);
      }
    }

    if (nDevices == 0) {
      Serial.printf("No I2C devices found\n");
    }
    else {
      Serial.printf("Done\n");
    }

    delay(100);
  }

  static void scantoint(int sensorAddress[]) {
    byte address, error;
    int nDevice = 0;
    delay(500);
    Serial.println("Scaning I2C ...");

    for (address = 0x01; address < 0x7f; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0) {
        Serial.printf("Device found at address 0x%02X\n", address);
        sensorAddress[nDevice] = address;
        nDevice++;
      }
      else if (error != 2) {
        Serial.printf("Error %d at address 0x%02X\n", error);
      }
    }
    if (nDevice == 0) {
      Serial.printf("No I2C Device Found");
    }

  }

}; // class I2CScanner
#endif