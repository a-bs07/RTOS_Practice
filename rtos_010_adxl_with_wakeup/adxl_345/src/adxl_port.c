/*
 * adxl_port.c
 *
 *  Created on: Feb 13, 2025
 *      Author: Anubrata
 */

#include"adxl_port.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t adxl_port_SendCMDTOMemAddr(uint8_t command,uint16_t MemAddr)
{
	HAL_StatusTypeDef status=HAL_OK;
	status=HAL_I2C_Mem_Write(&hi2c1,(uint16_t)adxl_app_I2C_ADDR,MemAddr ,I2C_MEMADD_SIZE_8BIT,&command,1,HAL_MAX_DELAY);
	return (uint8_t)status;
}


uint8_t adxl_port_ReceiveData(uint8_t *pdata,uint16_t len)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Master_Receive(&hi2c1, (uint16_t) adxl_app_I2C_ADDR,
			pdata, len, HAL_MAX_DELAY);
	return (uint8_t) status;
}
uint8_t adxl_port_ReceiveDataFromMem(uint8_t*pdata,uint16_t MemAddr,uint16_t size )
{
	    HAL_StatusTypeDef status=HAL_OK;
	    status=HAL_I2C_Mem_Read(&hi2c1,(uint16_t)adxl_app_I2C_ADDR,MemAddr, I2C_MEMADD_SIZE_8BIT,pdata,size,HAL_MAX_DELAY);
		return (uint8_t)status;
}
