#include <BNO055.h>
#include <Wire.h>

void ISR_IMU(void* vp_imu)
{
  BNO055* pIMU = (BNO055*) vp_imu;
  if(pIMU) pIMU -> imuInt = 1;
}

uint8_t BNO055::init(uint8_t mode, uint8_t interruptPin)
{
  Serial.println("BNO055::init()");

  Wire.begin();
    
  // Go to config mode
  delay(650); //according to application note...
  setMode(OPR_MODE_CONFIGMODE);
  delay(50);

  Serial.print("v");
  Serial.print(readReg(REG_VERSION_MAJ), HEX);
  Serial.print('.');
  Serial.print(readReg(REG_VERSION_MIN), HEX);
  Serial.print('\n');

  if(interruptPin != -1) 
  {
    pinMode(interruptPin, INPUT);
    attachInterruptArg(interruptPin, ISR_IMU, this, RISING);
  }

  // Then change to what we want
  setMode(mode);

  Serial.println("/BNO055::init()");

  return mode;
}

uint8_t BNO055::setMode(uint8_t mode)
{
  selectPage(PAGE_ID_0);
  writeReg(REG_OPR_MODE, mode);

  delay(50);

  // clear any existing interrupt
  resetInterrupt();

  return mode;
}

uint8_t BNO055::selectPage(uint8_t page)
{
  writeReg(REG_PAGE_ID, page);
  return readReg(REG_PAGE_ID); //for verification
}

uint8_t BNO055::writeReg(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(i2cAddr);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission();
}

uint8_t BNO055::readReg(uint8_t reg)
{
  Wire.beginTransmission(i2cAddr);
  Wire.write(reg);
  Wire.endTransmission(false); //repeated ST

  Wire.requestFrom(i2cAddr, (uint8_t)1); //the explicit cast is to get rid of a warning

  //annoying block
  while(Wire.available() < 1) {}
  
  return Wire.read();
}

uint8_t BNO055::readReg(uint8_t startReg, uint8_t* buffer, uint8_t count)
{
  Wire.beginTransmission(i2cAddr);
  Wire.write(startReg);
  Wire.endTransmission(false); //repeated ST

  uint8_t err = Wire.requestFrom(i2cAddr, count);

  //annoying block
  while(Wire.available() < count) {}

  //fill in data
  for(uint8_t i = 0; i < count; i++)
  {
    buffer[i] = Wire.read();
  }

  return err;
}

void BNO055::enableExternalCrystal(bool useExternal)
{
  // store current mode
  uint8_t mode = readReg(REG_OPR_MODE);

  // config mode
  setMode(OPR_MODE_CONFIGMODE);

  // ensure page 0
  selectPage(PAGE_ID_0);

  if(useExternal)
  {
    //datasheet says to check that the clock is running
    while(readReg(REG_SYS_CLK_STATUS) & ST_MAIN_CLK) {delay(50);}
    writeReg(REG_SYS_TRIGGER, CLK_SEL);
  }

  else
  {
    writeReg(REG_SYS_TRIGGER, 0x00);
  }

  //set the mode back to what it was
  setMode(mode);
}


vector<int16_t> BNO055::readRawAcc(void)
{
  uint8_t buffer[6];
  readReg(REG_ACC_DATA_X_LSB, buffer, 6);

  std::vector<int16_t> accRaw(3);
  accRaw[0] = (int16_t)buffer[0] | ((int16_t)buffer[1] << 8);
  accRaw[1] = (int16_t)buffer[2] | ((int16_t)buffer[3] << 8); 
  accRaw[2] = (int16_t)buffer[4] | ((int16_t)buffer[5] << 8);

  return accRaw;
}

vector<int16_t> BNO055::readRawGyro(void)
{
  uint8_t buffer[6];
  readReg(REG_GYR_DATA_X_LSB, buffer, 6);

  std::vector<int16_t> gyroRaw(3);
  gyroRaw[0] = (int16_t)buffer[0] | ((int16_t)buffer[1] << 8);
  gyroRaw[1] = (int16_t)buffer[2] | ((int16_t)buffer[3] << 8); 
  gyroRaw[2] = (int16_t)buffer[4] | ((int16_t)buffer[5] << 8);

  return gyroRaw;
}

uint8_t BNO055::enableHighAccInterupt(uint8_t threshold, uint8_t duration)
{
  // store current mode
  uint8_t mode = readReg(REG_OPR_MODE);

  // config mode
  setMode(OPR_MODE_CONFIGMODE);

  // these settings live on page 1
  selectPage(PAGE_ID_1);

  // enable interrupts
  writeReg(REG_INT_EN, ACC_HIGH_G);

  // enable interrupt pin
  writeReg(REG_INT_MSK, ACC_HIGH_G);

  // enable xy only axes
  writeReg(REG_ACC_INT_Settings, ACC_INT_X | ACC_INT_Y);

  //set the duration
  writeReg(REG_ACC_HG_DURATION, duration);

  //set the threshold
  writeReg(REG_ACC_HG_THRES, threshold); 

  // return to page 0
  selectPage(PAGE_ID_0);

  // clear any existing interrupt
  resetInterrupt();

  // return to previous mode
  return setMode(mode);
}

/**
 * THIS DOES NOT WORK WITH THE CURRENT VERSION OF THE FIRMWARE
 */ 
uint8_t BNO055::enableDataReadyInterupt(void)
{
  // store current mode
  uint8_t mode = readReg(REG_OPR_MODE);

  // config mode
  setMode(OPR_MODE_CONFIGMODE);

  // these settings live on page 1
  selectPage(PAGE_ID_1);

  // enable interrupts
  writeReg(REG_INT_EN, ACC_DATA_RDY);

  // enable interrupt pin
  writeReg(REG_INT_MSK, ACC_DATA_RDY);

  // return to page 0
  selectPage(PAGE_ID_0);

  // clear any existing interrupt
  writeReg(REG_SYS_TRIGGER, RST_INT);

  // return to previous mode
  return setMode(mode);
}

/**
 * This doesn't change the sensitivity, only the 
 * cap on the return value. I struggle to understand why they did this
 * and why you would choose anything under 16g #whatamimisssing?
 */ 
uint8_t BNO055::setAccFS(uint8_t fs)
{
  selectPage(1);

  uint8_t accConfig = readReg(REG_ACC_CONFIG);

  //clear fs bits
  accConfig &= 0xfc;

  //set new fs bits
  accConfig |= fs;

  writeReg(REG_ACC_CONFIG, accConfig);

  selectPage(0);

  return accConfig;
}