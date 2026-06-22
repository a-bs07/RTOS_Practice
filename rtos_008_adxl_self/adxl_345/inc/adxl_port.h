/*
 * adxl_port.h
 *
 *  Created on: Feb 13, 2025
 *      Author: Anubrata
 */

#ifndef ADXL_PORT_H_
#define ADXL_PORT_H_
#include"stdint.h"
#include "stm32l4xx_hal.h"

#define adxl_app_I2C_ADDR   0x53<<1   // Make sure the CS pin is pulled to VDD and the ALT ADDRESS to GND

uint8_t adxl_port_SendCMDTOMemAddr(uint8_t command,uint16_t MemAddr);
uint8_t adxl_port_ReceiveData(uint8_t *rxBuff,uint16_t len);
uint8_t adxl_port_ReceiveDataFromMem(uint8_t* data,uint16_t MemAddr,uint16_t size);
#endif /* ADXL_PORT_H_ */
