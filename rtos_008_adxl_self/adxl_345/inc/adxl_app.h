/*
 * adxl_app.h
 *
 *  Created on: Feb 13, 2025
 *      Author: Anubrata
 */

#ifndef ADXL_APP_H_
#define ADXL_APP_H_

#include"adxl_port.h"
#include"stdbool.h"


/*
 * @Registers
 */


#define ADXL345_DEVICE_ID   0xE5         // Expected Device ID

// @Register_Addresses
#define ADXL345_REG_DEVID        0x00
#define ADXL345_REG_THRESH_TAP   0x1D
#define ADXL345_REG_OFSX         0x1E
#define ADXL345_REG_OFSY         0x1F
#define ADXL345_REG_OFSZ         0x20
#define ADXL345_REG_DUR          0x21
#define ADXL345_REG_LATENT       0x22
#define ADXL345_REG_WINDOW       0x23
#define ADXL345_REG_THRESH_ACT   0x24
#define ADXL345_REG_THRESH_INACT 0x25
#define ADXL345_REG_TIME_INACT   0x26
#define ADXL345_REG_ACT_INACT_CTL 0x27
#define ADXL345_REG_THRESH_FF    0x28
#define ADXL345_REG_TIME_FF      0x29
#define ADXL345_REG_TAP_AXES     0x2A
#define ADXL345_ACT_TAP_STATUS   0x2B
#define ADXL345_REG_BW_RATE      0x2C
#define ADXL345_REG_POWER_CTL    0x2D
#define ADXL345_REG_INT_ENABLE   0x2E
#define ADXL345_REG_INT_MAP      0x2F
#define ADXL345_REG_INT_SOURCE   0x30
#define ADXL345_REG_DATA_FORMAT  0x31
#define ADXL345_REG_DATAX0       0x32
#define ADXL345_REG_DATAX1       0x33
#define ADXL345_REG_DATAY0       0x34
#define ADXL345_REG_DATAY1       0x35
#define ADXL345_REG_DATAZ0       0x36
#define ADXL345_REG_DATAZ1       0x37
#define ADXL345_REG_FIFO_CTL     0X38
#define ADXL345_REG_FIFO_STATUS  0X39


/*
 * @Tap_Type
 */

#define adxl_app_Tap_Single   0
#define adxl_app_Tap_Double   1
#define adxl_app_Tap_Both     2

/*
 * @ Memory_modes
 */

#define adxl_app_MODE_BYPASS   0
#define adxl_app_MODE_FIFO     1
#define adxl_app_MODE_STREAM   2
#define adxl_app_MODE_TRIGGER  3

/*
 * @pin
 */

#define adxl_app_INT_PIN1   0
#define adxl_app_INT_PIN2   1

/*
 * @INT_FLAGS
 */
#define adxl_app_INT_FLAG_OVERRUN     0
#define adxl_app_INT_FLAG_WATERMARK   1
#define adxl_app_INT_FLAG_FREE_FALL   2
#define adxl_app_INT_FLAG_INACTIVITY  3
#define adxl_app_INT_FLAG_ACTIVITY    4
#define adxl_app_INT_FLAG_DOUBLE_TAP  5
#define adxl_app_INT_FLAG_SINGLE_TAP  6
#define adxl_app_INT_FLAG_DATA_READY  7
#define adxl_app_INT_FLAG_ALL         8

/*
 * @TAP_dECECTION_AXIS
 */

#define adxl_app_EN_X_AXIS   2
#define adxl_app_EN_Y_AXIS   1
#define adxl_app_EN_Z_AXIS   0
#define adxl_app_EN_ALL_AXIS 7

/*
 * @brief:Set Memory mode
 * @refer:@ Memory_modes
 * @para1:uint8_t mode
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool  adxl_app_SetMemMode(uint8_t mode);




/*
 * @brief:Set number of samples .when the number of samples stored/collected in Fifo  is same as that of
   * the value that we set as a sample number in FIFO_CTL register ,Watermark interrupt occurs
 * @para1:uint8_t (numbre of samples) Min val:0, Max val:31
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_SetSampleNum(uint8_t sample);//MAX Sample Val=31





/*
 * @brief:This function maps an interrupt to either pin1 or pin2
 * @para1:uint8_t flag (Refer: @INT FLAGS)
 * @para2:uin8_t  pin (Refer:@pin)
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_SetINT(uint8_t IntFlag,uint8_t pin);




/*
 * @brief:This function reads acceleration data from x,y,z register in burst mode ,and stores the data
 * in the provided address of x ,y,z.
 * @para1:uint16_t * pointer to s variable where x axis acc val will be stored
 * @para1:uint16_t * pointer to s variable where y axis acc val will be stored
 * @para1:uint16_t * pointer to s variable where z axis acc val will be stored
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 *
 */
bool adxl_app_GetAccVal(int16_t* pxData, int16_t* pyData,int16_t* pzData);




/*
 * @brief:This function returns the numbre of samples that are currently present/collected in FIFO
 * @return  number of samples
 *
 */
uint8_t adxl_app_GetFifoStatus();



//**************************************API for Single Tap detection****************




/*
 * @brief:This function is used to set a threshold value, in the THRESH_TAP register that
 *  determines how strong an impact (tap) must be to trigger an interrupt.
 *  @para1:uint8_t Threshold val (range 0 to 255 or 0x00 to 0xFF) Scaling factor is 0.0625 g
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 *
 */
bool adxl_app_SetThresVal(uint8_t val);





/*
 * @brief:This function set a time  duration which defines the maximum time an acceleration event must remain above the THRESH_TAP
 *        threshold to be considered a valid tap event.
 *  @para1:uint8_t Duration val (range 0 to 255 or 0x00 to 0xFF) Scaling factor is 625 µs (Tap Duration=val ×625 µs)
 *
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 *
 */
bool adxl_app_SetDurationVal(uint8_t val);





/*
 * @brief: enables the axis for which we want to detect tap
 * @para:uint8_t(refer:TAP_dECECTION_AXIS)
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */

bool adxl_app_EnableTapAxis(uint8_t axis);

//************************Specific API for Double tap**************************

/*
 * @brief: This function sets Latent time  that defines the waiting period between two tap events when detecting double taps.
 * When a first tap is detected, the accelerometer starts a latency timer.
   The LATENT value specifies how long the sensor must wait before looking for a second tap.

 * @para1:Latent value (@scale factor:1.25 ms,  Latency Time=LATENT value×1.25 ms)
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_SetLatentVal(uint8_t val);




/*
 * @brief: The WINDOW register defines the time window after the LATENT period during
 *  which a second tap can occur for it to be considered a double tap.

 * @para1:Window value (@scale factor:1.25 ms,  Window Time=WINDOW value×1.25 ms)
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_SetWindowVal(uint8_t val);




/*
 * @brief: Enables interrupt
 * @para1:flag(@refer: @INT_FLAGS)
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_EnINT(uint8_t flag);





/*
 * @brief:DIsable interrupt
 * @para1:flag(@refer: @INT_FLAGS)
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_DiINT(uint8_t flag);


/*
 * @brief:Initialize ADXL for Steram mode
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_StreamModeInit();

/*
 * @brief:Initialize ADXL for Single Tap Mode
 * when a single acceleration event that
     is greater than the value in the THRESH_TAP register(Threshold val) occurs for less time than is specified in the DUR register(durVal)
 * @para1:Threshold val
 * @para2:durVal
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_SingleTapModeInit(uint8_t thresVal,uint8_t durVal );
//bool adxl_app_SingleTapModeInit();
/*
 * @brief:Initialize ADXL for Single Tap Mode
 * when an acceleration event that is greater than the value in the THRESH_TAP register(Threshold val) occurs for less time than is specified in the DUR register(durVal)tap is detected ,in case of
 * double tap ,latVal is the waiting period from the end of the first tap to the begining of the window,the second tap must be detected with in the window time (windowVal)
the start of the time window
   @para1:latVal
   @para2: WindowVal
 * @para3:Threshold val
 * @para4:durVal
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool  adxl_app_DoubleTapMode(uint8_t latVal,uint8_t windowVal,uint8_t thresVal,uint8_t durVal);


/*
 * @brief:Initialize ADXL for Single Tap Mode
 * when an acceleration event that is greater than the value in the THRESH_TAP register(Threshold val) occurs for less time than is specified in the DUR register(durVal)tap is detected ,in case of
 * double tap ,latVal is the waiting period from the end of the first tap to the begining of the window,the second tap must be detected with in the window time (windowVal)
the start of the time window
   @para1: Tap type (@refer: @Tap_Type)
   @para1:latVal
   @para2: WindowVal
 * @para3:Threshold val
 * @para4:durVal
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_TapModeInit(uint8_t taptype,uint8_t latVal,uint8_t windowVal,uint8_t thresVal,uint8_t durVal);



/*
 * @brief:Used to read any register of adxl

   @para1: address of the variable where register content will be stored
   @para2:Register from where we want fetch stored  data (refer:@Register_Addresses)
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_ReadReg(uint8_t* pdata,uint8_t Reg);

/*
 * @brief:Used to disable measurement mode
 * note:This must be called to make the adxl calculate acceleration for enabled axis
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_DIMeasurement();
/*
 * @brief:Used to disable measurement mode
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_ENMeasurement();

/**
 * @brief  Checks whether a specific interrupt has been triggered.
 * @param  flag: The flag from the INT_SOURCE register to verify if the interrupt has occurred.
 * @return bool: Returns `true` if the interrupt is triggered, otherwise `false`.
 */
bool adxl_app_checkINTflagstatus(uint8_t flag);

/*
 * @brief:Used to write data to any register of adxl

   @para1: data we want to write
   @para2:Register  to which  we want write  data (refer:@Register_Addresses)
 * @return bool( 0 means SUCCESS , 1 means NOT SUCCESS)
 */
bool adxl_app_WriteReg(uint8_t data,uint8_t reg);

/**
 * @brief  Calculates and sets the offset values for the ADXL345 sensor
 *
 * @details This function measures the raw acceleration values when the sensor
 *          is stationary and computes the offset values for X, Y, and Z axes.
 *          The calculated offsets are written to the OFSX, OFSY, and OFSZ registers
 *          (0x1E, 0x1F, 0x20) to correct sensor bias and improve accuracy.
 *
 * @note   This function should be run with the sensor in a stable, level position.
 */
void adxl_app_calculate_offsets();

/*
 * @brief: Used to initialize different tap modes
 * @NOTE: You can use individual tap modes or this function.
 *        However, to enable both single and double tap detection, this function must be called.
 *
 * @param taptype   Type of tap detection (Single Tap or Double Tap) (refer: @Tap_Type)
 *                   Single Tap
 *                   Double Tap
 *                   Both Single & Double Tap
 *
 * @param latVal    Latency time (0x22) – The delay between the first and second tap
 *                  - Recommended: > 0x10 (20ms)
 *
 * @param windowVal Time window (0x23) – Time duration within which the second tap must occur
 *                  - Recommended: > 0x40 (80ms)
 *
 * @param thresVal  Tap threshold (0x1D) – Minimum acceleration required to register a tap
 *                  - Recommended: > 0x30 (3g)
 *
 * @param durVal    Tap duration (0x21) – Maximum time allowed for a tap event
 *                  - Recommended: > 0x10 (10ms)
 *
 * @return bool     0 = SUCCESS, 1 = FAILURE
 */
bool adxl_app_TapModeInit(uint8_t taptype, uint8_t latVal, uint8_t windowVal, uint8_t thresVal, uint8_t durVal);


/*
 * @brief:Used to  print  all axis data in stream mode
 * @return  void
 */
void adxl_app_Printdata();

#endif /* ADXL_APP_H_ */
