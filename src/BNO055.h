#pragma once

#include <Arduino.h>
#include <BNO055_defines.h>
#include <vector>

#include <Wire.h>

using namespace std; // for vector containers

/*
 * Bosch BNO055 IMU
 */

class BNO055
{
  private:
    uint8_t i2cAddr = BNO055_I2C_ADDR_LOW; // pulled LOW on breakout

  public:
    volatile uint8_t imuInt = 0;

  public:
    uint8_t init(uint8_t mode, uint8_t interruptPin = -1);
    uint8_t setMode(uint8_t mode);

    vector<int16_t> readRawAcc(void);
    vector<int16_t> readRawGyro(void);

    uint8_t resetInterrupt(void) {return writeReg(REG_SYS_TRIGGER, RST_INT);}
    void enableExternalCrystal(bool useExternal);

    uint8_t setAccFS(uint8_t fs);

    uint8_t enableHighAccInterupt(uint8_t threshold, uint8_t duration);
    uint8_t enableDataReadyInterupt(void);

  private:
    uint8_t selectPage(uint8_t);
    uint8_t writeReg(uint8_t reg, uint8_t val);
    uint8_t readReg(uint8_t reg);
    uint8_t readReg(uint8_t startReg, uint8_t* buffer, uint8_t count);
};
