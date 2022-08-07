#ifndef _I2C_H_
#define _I2C_H_
#include "stdint.h"

#define I2C_SCL 2
#define I2C_SDA 1

#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))

#define I2C_SCL_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*I2C_SCL))
#define I2C_SDA_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*I2C_SDA))

#define I2C_SCL_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + 4*I2C_SCL + 4))
#define I2C_SDA_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + 4*I2C_SDA + 4))

#define I2C_ENABLE_IC ((volatile uint32_t*)(I2C0_BASE +I2C_IC_ENABLE_OFFSET))
#define I2C_IC_CON ((volatile uint32_t*)(I2C0_BASE +I2C_IC_CON_OFFSET))
#define I2C_IC_TAR ((volatile uint32_t*)(I2C0_BASE +I2C_IC_TAR_OFFSET))
#define I2C_IC_DATA_CMD ((volatile uint32_t*)(I2C0_BASE +I2C_IC_DATA_CMD_OFFSET))

#define I2C_IC_SS_SCL_HCNT ((volatile uint32_t*)(I2C0_BASE +I2C_IC_SS_SCL_HCNT_OFFSET))
#define I2C_IC_SS_SCL_LCNT ((volatile uint32_t*)(I2C0_BASE +I2C_IC_SS_SCL_LCNT_OFFSET))
#define I2C_IC_STATUS ((volatile uint32_t*)(I2C0_BASE +I2C_IC_STATUS_OFFSET))

#define I2C_IC_CLR_TX_ABRT ((volatile uint32_t*)(I2C0_BASE +I2C_IC_CLR_TX_ABRT_OFFSET))
#define I2C_IC_RAW_INTR_STAT ((volatile uint32_t*)(I2C0_BASE +I2C_IC_RAW_INTR_STAT_OFFSET))
#define I2C_IC_TX_ABRT_SOURCE ((volatile uint32_t*)(I2C0_BASE +I2C_IC_TX_ABRT_SOURCE_OFFSET))

#define I2C_ERROR_ARBITRATION_LOST 1
#define I2C_ERROR_SLAVE_ADDRESS_NACK 2
#define I2C_ERROR_DATA_NACK 3
void initI2c(uint8_t slaveAdress);


uint8_t masterTransmit(uint8_t data,uint8_t lastCmd);
uint8_t masterReceive(uint8_t lastCmd);


#endif