/*
 * adxl_app.c
 *
 *  Created on: Feb 13, 2025
 *      Author: Anubrata
 */

#include"adxl_app.h"
#include<stdio.h>
 extern UART_HandleTypeDef huart2;
bool adxl_app_SetMemMode(uint8_t mode)
{
	uint8_t data = mode<<6;
	uint8_t RegVal=0;
	adxl_app_ReadReg(&RegVal,(uint16_t)ADXL345_REG_FIFO_CTL);// for testing
	RegVal=0;
	bool status =(bool)adxl_port_SendCMDTOMemAddr(data,(uint16_t)ADXL345_REG_FIFO_CTL);
	adxl_app_ReadReg(&RegVal,(uint16_t)ADXL345_REG_FIFO_CTL);//for testing
	return status;
}
bool adxl_app_SetSampleNum(uint8_t sample)
{
	bool status1 = 0, status2 = 0;
	uint8_t RegData = 0,RegVal=0;
	sample &= 31; // to use only the lower 5 bits
	status1 = (bool) adxl_port_ReceiveDataFromMem(&RegData,
			ADXL345_REG_FIFO_CTL,1); // first we read the current reg data

	RegData |= sample; //This is updated register data
	status2 =(bool)adxl_port_SendCMDTOMemAddr(RegData, (uint16_t)ADXL345_REG_FIFO_CTL);
	adxl_app_ReadReg(&RegVal,(uint16_t)ADXL345_REG_FIFO_CTL);//for testing
	return status1&status2;

}
bool adxl_app_SetINT(uint8_t IntFlag,uint8_t pin)
{
	uint8_t RegData=0;
	uint8_t RegVal=0;
	bool status1 = 0;
	status1=(bool)adxl_port_ReceiveDataFromMem(&RegData,(uint16_t)ADXL345_REG_INT_MAP,1);// first we read the current reg dat
	adxl_app_ReadReg(&RegVal,(uint16_t)ADXL345_REG_INT_MAP);//for testing
	RegVal=0;
	if(!status1)
	{
	if(pin==adxl_app_INT_PIN1)// we have to write 0
	{
		RegData&=~(1<<IntFlag);
	    status1=(bool)adxl_port_SendCMDTOMemAddr(RegData,(uint16_t)ADXL345_REG_INT_MAP);
	    adxl_app_ReadReg(&RegVal,(uint16_t)ADXL345_REG_INT_MAP);//for testing
	    return status1;
	}
	else if(pin==adxl_app_INT_PIN2)// we have to write 1
	{
		RegData|=(1<<IntFlag);
		//RegData|=(1<<IntFlag);
		 status1=(bool)adxl_port_SendCMDTOMemAddr(RegData,(uint16_t)ADXL345_REG_INT_MAP);
		adxl_app_ReadReg(&RegVal,(uint16_t)ADXL345_REG_INT_MAP);//for testing
			    return status1;
	}
	else return 1;// indicates false
	}
	return 1;
}
bool adxl_app_GetAccVal(int16_t* x, int16_t* y,int16_t* z)
{
	bool status1 = 0;
	uint8_t fifo_data[6];

	status1 = (bool) adxl_port_ReceiveDataFromMem(fifo_data,
			   ADXL345_REG_DATAX0,6); // first we read the current reg data
	if(!status1)
	{
		*x = (int16_t)((fifo_data[1] << 8) | fifo_data[0]);
		*y = (int16_t)((fifo_data[3] << 8) | fifo_data[2]);
		*z = (int16_t)((fifo_data[5] << 8) | fifo_data[4]);
	return 0;
	}

  return 1;
}

bool adxl_app_DiINT(uint8_t flag)
{
	bool status=1;
	uint8_t RegVal=0;
	if(flag==adxl_app_INT_FLAG_ALL)
		{
		    uint8_t val=0;
			status= (bool)adxl_port_SendCMDTOMemAddr(val,(uint16_t)ADXL345_REG_INT_ENABLE);
			adxl_port_ReceiveDataFromMem(&RegVal,(uint16_t)ADXL345_REG_INT_ENABLE,1);// testing
			return status;
		}
	    bool status1 = 1;
		uint8_t RegData=0;
		status1=(bool)adxl_port_ReceiveDataFromMem(&RegData,(uint16_t)ADXL345_REG_INT_ENABLE,1);// first we read the current reg dat
		RegData&=~(1<<flag);
		return (bool)adxl_port_SendCMDTOMemAddr(RegData,(uint16_t)ADXL345_REG_INT_ENABLE);
}
bool adxl_app_EnINT(uint8_t flag)
{

	bool status1 = 1;
	uint8_t RegData=0,RegVal=0;
	status1=(bool)adxl_port_ReceiveDataFromMem(&RegData,(uint16_t)ADXL345_REG_INT_ENABLE,1);// first we read the current reg dat
	RegData|=1<<flag;
	status1=(bool)adxl_port_SendCMDTOMemAddr(RegData,(uint16_t)ADXL345_REG_INT_ENABLE);
	//adxl_port_ReceiveDataFromMem(&RegVal,(uint16_t)ADXL345_REG_INT_ENABLE,1);// testing
	return status1;
}
uint8_t adxl_app_GetFifoStatus()
{
	bool status=1;
	uint8_t fifo_Status=0;
	status=(bool) adxl_port_ReceiveDataFromMem(&fifo_Status,
			ADXL345_REG_FIFO_STATUS,1);// this is returning true
	if(status)return ;
	fifo_Status=fifo_Status&(0x3f);
	return fifo_Status;
}

/* TODO: REMOVE THIS COMMENT AFTER TESTING
 *
 * Tap Sensitivity	THRESH_TAP Value (Hex)	Acceleration Threshold (g)
Very Sensitive	0x10 (16)	1 g
Moderate	0x20 (32)	2 g
Hard Tap	0x40 (64)	4 g
 */
bool adxl_app_SetThresVal(uint8_t val)
{
	return (bool)adxl_port_SendCMDTOMemAddr(val,(uint16_t)ADXL345_REG_THRESH_TAP);
}
/* for testing
 * scaling factor : 625 µs
 * Tap Type	Suggested DUR Value (Hex)	Duration (ms)
    Very Quick Tap	  0x10 (16)	   10 ms
    Normal Tap	      0x20 (32)	   20 ms
    Slightly Long Tap 0x40 (64)	   40 ms
    Extended Tap	  0x80 (128)   80 ms
 */
bool adxl_app_SetDurationVal(uint8_t val)
{
	return (bool)adxl_port_SendCMDTOMemAddr(val,(uint16_t)ADXL345_REG_DUR);
}
//Scale Factor=1.25ms
/*
 *
Double Tap Speed	Suggested LATENT Value (Hex)	Latency Time (ms)
Very Fast Double Tap   0x08 (8)	    10 ms
Normal Double Tap	   0x20 (32)	40 ms(use this)
Slower Double Tap	   0x40 (64)	80 ms
Very Slow Double Tap   0x80 (128)	160 ms
 */
bool adxl_app_SetLatentVal(uint8_t val)
{
	return (bool)adxl_port_SendCMDTOMemAddr(val,(uint16_t)ADXL345_REG_LATENT);
}/*
 * Scale Factor=1.25ms
 * 5. Choosing a Good WINDOW Value for Testing
Double Tap Speed	     Suggested WINDOW Value (Hex)	Window Time (ms)
Very Fast Double Tap	 0x10 (16)	                    20 ms
Normal Double Tap	     0x20 (32)	                    40 ms
Slightly Slow Double Tap 0x40 (64)	                    80 ms
Delayed Double Tap	     0x80 (128)	                    160 ms
For most applications, a good starting value is 0x40 (80 ms).
 */
bool adxl_app_SetWindowVal(uint8_t val)
{
	return (bool)adxl_port_SendCMDTOMemAddr(val,(uint16_t)ADXL345_REG_WINDOW);
}

bool adxl_app_EnableTapAxis(uint8_t axis)
{
	uint8_t regVal=0,data=0;
	bool status=(bool) adxl_port_ReceiveDataFromMem(&regVal,ADXL345_REG_TAP_AXES,1);

	if(!status)
	{
	if(axis==adxl_app_EN_ALL_AXIS)
	{
		regVal|=(adxl_app_EN_ALL_AXIS);
		regVal|=(1<<3);// for enabling suppress bit (refer the register of TAP_AXES  register
	status=(bool)adxl_port_SendCMDTOMemAddr(regVal,(uint16_t)ADXL345_REG_TAP_AXES);
	adxl_app_ReadReg(&data,ADXL345_REG_TAP_AXES);
	return status;
	}
	else
	{
		regVal|= 1<<axis;
		regVal|=(1<<3);// for enabling suppress bit (refer the register of TAP_AXES  register
		return (bool)adxl_port_SendCMDTOMemAddr(regVal,(uint16_t)ADXL345_REG_TAP_AXES);
	}
	}
	return 1;
}

bool adxl_app_ReadReg(uint8_t* pdata,uint8_t Reg)
{
	bool status=(bool) adxl_port_ReceiveDataFromMem(pdata,(uint16_t)Reg,1);
	return status;
}
bool adxl_app_ENMeasurement()
{
	bool status=1;
	uint8_t val=1<<3;
	uint8_t RegVal=0;
	status= (bool)adxl_port_SendCMDTOMemAddr(val,(uint16_t)ADXL345_REG_POWER_CTL);
				adxl_port_ReceiveDataFromMem(&RegVal,(uint16_t)ADXL345_REG_POWER_CTL,1);//  FOR testing
				return status;
}
bool adxl_app_DIMeasurement()
{
	uint8_t val=0;
	bool status=1;
	uint8_t RegVal=0;
	status= (bool)adxl_port_SendCMDTOMemAddr(val,(uint16_t)ADXL345_REG_POWER_CTL);
				adxl_port_ReceiveDataFromMem(&RegVal,(uint16_t)ADXL345_REG_INT_ENABLE,1);//  FOR testing
				return status;
}
// Returns 0 if no INT has occured , or INT is cleared,
bool adxl_app_checkINTflagstatus(uint8_t flag)
{
	uint8_t data =0;
	 adxl_app_ReadReg(&data,ADXL345_REG_INT_SOURCE);
	 data = (data>>flag)&1;
	 return (bool)data;

}
bool adxl_app_StreamModeInit()
{

	bool status1=0,status2=0,status3=0,status4=0,status5=0,status6=0;
	  status1=adxl_app_SetMemMode(adxl_app_MODE_STREAM);//working
	  /*
	   * when the number of samples stored/collected in Fifo  is same as that of
	   * the value that we set using the below funciton , Watermark interrupt occurs
	   */
	  status2=adxl_app_SetSampleNum(31);//

	  //Disable all interrupt
	   adxl_app_DiINT(adxl_app_INT_FLAG_ALL);
	   //  mapping the INT to a pin
	   status3=adxl_app_SetINT(adxl_app_INT_FLAG_WATERMARK,adxl_app_INT_PIN1);//previously was pin 1

	   status4= adxl_app_EnINT(adxl_app_INT_FLAG_WATERMARK);

	   status5=adxl_app_WriteReg(0x0B,ADXL345_REG_DATA_FORMAT);

	  // change the power mode from STANDBY to Measurement mode
	   status6= adxl_app_ENMeasurement();
	   return status1|status2|status3|status4|status5|status6;
}
bool adxl_app_WriteReg(uint8_t data,uint8_t reg)
{
	bool status= (bool)adxl_port_SendCMDTOMemAddr(data,(uint16_t)reg);
	return status;
}

bool adxl_app_SingleTapModeInit(uint8_t tapThres,uint8_t DurVAl)
{
	//set latent and window to 0(to disable double tap)
		adxl_app_SetLatentVal(0);
		adxl_app_SetWindowVal(0);
		// do data formatting
		adxl_app_WriteReg(0x08,ADXL345_REG_DATA_FORMAT);
		//set detection axis
		adxl_app_EnableTapAxis(adxl_app_EN_ALL_AXIS);
		adxl_app_SetThresVal(tapThres);
		//set Duration
		adxl_app_SetDurationVal(DurVAl);
		//Interrupt configuration
		//1.Map the interrupt
		adxl_app_SetINT(adxl_app_INT_FLAG_SINGLE_TAP, adxl_app_INT_PIN1);//previously 2
		//2.Disable alll interrupt
		adxl_app_DiINT(adxl_app_INT_FLAG_ALL);
		//3.Now enable specific interrupts
		adxl_app_EnINT(adxl_app_INT_FLAG_SINGLE_TAP);

		//adxl_app_ENMeasurement();
		return 0;
}
bool  adxl_app_DoubleTapMode(uint8_t latVal,uint8_t windowVal,uint8_t thresVal,uint8_t durVal)
{
	bool status1=0,status2=0,status3=0,status4=0,status5=0,status6=0,status7=0,status8=0;


	        adxl_app_WriteReg(0x08,ADXL345_REG_DATA_FORMAT);
			status5=(adxl_app_EnableTapAxis(adxl_app_EN_ALL_AXIS));

	        status1=(adxl_app_SetThresVal(thresVal));
			status2=(adxl_app_SetDurationVal(durVal));
			status3=adxl_app_SetLatentVal(latVal);
		    status4=(adxl_app_SetWindowVal(windowVal));


		    //Interrupt setting
		    status6=(adxl_app_SetINT(adxl_app_INT_FLAG_DOUBLE_TAP, adxl_app_INT_PIN1));//prev pin1
		    status7=(adxl_app_DiINT(adxl_app_INT_FLAG_ALL));
		    status8=adxl_app_EnINT((adxl_app_INT_FLAG_DOUBLE_TAP));
			   return status1|status2|status3|status4|status5|status6|status7|status8;
}
#define NUM_SAMPLES 10

void adxl_app_calculate_offsets() {
    int32_t x_sum = 0, y_sum = 0, z_sum = 0;
    int16_t x, y, z;

    // Read multiple samples to average
    for (int i = 0; i < NUM_SAMPLES; i++) {
    	if(!adxl_app_GetAccVal(&x,&y,&z))
    	{
            x_sum += x;
            y_sum += y;
            z_sum += z;
            x=0,y=0,z=0;
        }
    }

    // Compute the averaged 0g offsets
    int16_t X0g = x_sum / NUM_SAMPLES;
    int16_t Y0g = y_sum / NUM_SAMPLES;
    int16_t Z1g = z_sum / NUM_SAMPLES; // This includes gravity

    int16_t SZ = 256; // Expected LSB/g in full-resolution mode
    int16_t Z0g = Z1g - SZ; // Compute actual 0g offset for Z-axis

    // Compute the offsets
    int8_t X_OFFSET = -(X0g / 4);
    int8_t Y_OFFSET = -(Y0g / 4);
    int8_t Z_OFFSET = -(Z0g / 4);

    // Write offsets to the ADXL345 registers
  adxl_app_WriteReg((uint8_t)X_OFFSET,ADXL345_REG_OFSX);
  adxl_app_WriteReg((uint8_t)Y_OFFSET,ADXL345_REG_OFSY);
  adxl_app_WriteReg((uint8_t)Z_OFFSET,ADXL345_REG_OFSZ);

    uint8_t buff[50]={0};
    sprintf(buff,"Offsets Set: X=%d, Y=%d, Z=%d\n",X_OFFSET,Y_OFFSET,Z_OFFSET);
    HAL_UART_Transmit(&huart2, (uint8_t*) buff,sizeof(buff),
    						HAL_MAX_DELAY);
}
bool adxl_app_TapModeInit(uint8_t taptype,uint8_t latVal,uint8_t windowVal,uint8_t thresVal,uint8_t durVal)
{
	bool status=0;
	if(taptype==adxl_app_Tap_Single)
	{
		status= adxl_app_SingleTapModeInit(thresVal,durVal);

		 return status;
	}
	else if(taptype==adxl_app_Tap_Double){
		status= adxl_app_DoubleTapMode(latVal,windowVal,thresVal,durVal);
	    return status;
	}
	else if(taptype==adxl_app_Tap_Both)
	{
		status= adxl_app_DoubleTapMode(latVal,windowVal,thresVal,durVal);
//		status|=(adxl_app_SetINT(adxl_app_INT_FLAG_SINGLE_TAP, adxl_app_INT_PIN2));
//		status|=adxl_app_EnINT((adxl_app_INT_FLAG_SINGLE_TAP));
		adxl_app_SetINT(adxl_app_INT_FLAG_SINGLE_TAP, adxl_app_INT_PIN2);
	    adxl_app_EnINT((adxl_app_INT_FLAG_SINGLE_TAP));
		return status;

	}
	else return 1;
}
void adxl_app_Printdata() {

	volatile uint8_t NoOfSamples = 0;
	int16_t x_axis = 0, y_axis = 0, z_axis = 0;

	//if (ReadData_falg == 1)  // Check if interrupt flag is set
		if(adxl_app_checkINTflagstatus(adxl_app_INT_FLAG_WATERMARK))
			{
		NoOfSamples = adxl_app_GetFifoStatus();  // Get FIFO status
		printf("FIFO Samples: %d\n", NoOfSamples); // Debug print

		bool status = 1;
		int len = 0;
		char buffer[50];

		while (NoOfSamples--)  // Read all available samples
		{
			status = adxl_app_GetAccVal(&x_axis, &y_axis, &z_axis);
			printf("Read Status: %d\n", status);  // Debug print

			if (!status)  // If read successful (status == 0)
			{
				len = sprintf(buffer,
						"\r\n X-axis: %d   Y-axis: %d   Z-axis: %d \n", x_axis,
						y_axis, z_axis);
				HAL_UART_Transmit(&huart2, (uint8_t*) buffer, len,
						HAL_MAX_DELAY);
				memset(buffer, 0, sizeof(buffer));  // Clear buffer efficiently
				//HAL_Delay(100);
			}
		}

//		ReadData_falg = 0;  // Clear flag AFTER reading all samples
		status = adxl_app_checkINTflagstatus(adxl_app_INT_FLAG_WATERMARK); // Update ADXL345 flag status
		// printf("Int flag  Status: %d\n", status);
		len = sprintf(buffer, "Int flag  Status: %d\n", status);
		HAL_UART_Transmit(&huart2, (uint8_t*) buffer, len, HAL_MAX_DELAY);
	}
}

