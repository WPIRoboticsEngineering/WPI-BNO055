#pragma once

// You can change the address with the ADR pin on the breakout board
#define BNO055_I2C_ADDR_LOW     0x28
#define BNO055_I2C_ADDR_HIGH    0x29

// Registers and their options

// Chip ID (r): Should always read 0xA0
#define REG_CHIP_ID             0x00

#define REG_VERSION_MIN         0x04
#define REG_VERSION_MAJ         0x05

// Page ID (w/r): Write 0x00 or 0x01 to this register to change pages
// Defaults to Page 0 at startup
#define REG_PAGE_ID             0x07
#define PAGE_ID_0               0x00
#define PAGE_ID_1               0x01

// Operating mode
#define REG_OPR_MODE            0x3D
#define OPR_MODE_CONFIGMODE     0x00
#define OPR_MODE_ACCONLY        0x01 //accelerometer only
#define OPR_MODE_AMG            0x07 //all three

// Data registers
#define REG_ACC_DATA_X_LSB      0x08
// ...
#define REG_MAG_DATA_X_LSB      0x0E
// ...
#define REG_GYR_DATA_X_LSB      0x14
// ...

// interrupt status
#define REG_INT_STA             0x37

// clock status
#define REG_SYS_CLK_STATUS      0x38
#define ST_MAIN_CLK             (1 << 0)

// system resets
#define REG_SYS_TRIGGER         0x3F
#define CLK_SEL                 (1 << 7)
#define RST_INT                 (1 << 6)
#define RST_SYS                 (1 << 5)

// PAGE 1 registers -- be sure to change pages before addressing!!!

// Accelerometer interrupt registers
#define REG_ACC_CONFIG              0x08
#define ACC_FS_2G                   0x00
#define ACC_FS_4G                   0x01
#define ACC_FS_8G                   0x02
#define ACC_FS_16G                  0x03

#define REG_INT_MSK                 0x0F //for controlling INT pin
#define ACC_HIGH_G                  (1 << 5)
#define ACC_DATA_RDY                (1 << 0)

#define REG_INT_EN                  0x10 //for enabling interrupts
#define REG_ACC_AM_THRES            0x11
#define REG_ACC_INT_Settings        0x12

// settings for interrupt registers
#define ACC_INT_X                   0x20
#define ACC_INT_Y                   0x40
#define ACC_INT_Z                   0x80

#define REG_ACC_HG_DURATION         0x13
#define REG_ACC_HG_THRES            0x14
