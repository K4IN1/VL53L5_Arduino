#ifndef __VL53L5_MOD__
#define __VL53L5_MOD__

// #include <sys/_stdint.h>
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>

class VL53L5CX_MOD : public SparkFun_VL53L5CX {
private:
  uint8_t m_lpnPin;
  uint8_t m_resolution;
  uint8_t m_frequency;
  uint8_t m_address;
  // uint8_t m_integralTime;

public:
  void disable(void) {
    pinMode(m_lpnPin, OUTPUT);
    digitalWrite(m_lpnPin, LOW);
    delay(100);
  }
  void enable(void) {
    pinMode(m_lpnPin, OUTPUT);
    digitalWrite(m_lpnPin, HIGH);
    delay(100);
  }
  // bool tofbegin(byte address, TwoWire& wirePort) {
  //   if (address) {
  //     m_address = address;
  //   }
  //   begin(m_address, Wire);
  // }

  bool setaddress(uint8_t address) {
    enable();
    m_address = address;
    // 必须要Begin后才能setaddress，否则报错
    if (begin() == false && begin(m_address) == false) {
      // Serial.println(F("Sensor not found. Check wiring. Freezing..."));
      while (1)
        ;
    } else {
      if (setAddress(m_address) == false) {
        // Serial.println(F("Sensor failed to set new address. Please try again. Freezing..."));
        while (1)
          ;
      } else {
        uint8_t n_add = getAddress();
        delay(100);
        // Serial.printf("New address of sensor_lpn%d is: 0x", m_lpnPin);
        // Serial.println(n_add, HEX);
        return true;
      }
    }
  }

  VL53L5CX_MOD(
    const uint8_t lpnPin
    // const uint8_t integralTime,
    // const uint8_t res = 8 * 8,
    // const uint8_t freq = 15,
    // const uint8_t address = 0x52
  ) {
    m_lpnPin = lpnPin;
    m_resolution = 8 * 8;
    m_frequency = 15;
  }
};
#endif