/*
 * rtc_handle.c
 *
 *  Created on: Apr 30, 2025
 *      Author: Anubrata
 */


#include"main.h"
#include"FreeRTOS.h"
#include "cmsis_os.h"

extern RTC_HandleTypeDef hrtc;
extern osMessageQueueId_t print_queueHandle;


uint8_t rtc_handle_ConfigDate(RTC_DateTypeDef *sDate)
{
	if(HAL_RTC_SetDate(&hrtc, sDate, RTC_FORMAT_BIN)!=HAL_OK)
	{
     return 0;
	}
     return 1;
}
uint8_t rtc_handle_ConfigTime(RTC_TimeTypeDef *sTime)
{
	sTime->DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
	sTime->TimeFormat=RTC_HOURFORMAT12_AM;
	sTime->StoreOperation=RTC_STOREOPERATION_RESET;
    if(HAL_RTC_SetTime(&hrtc,sTime,RTC_FORMAT_BIN)!=HAL_OK)
    {
    	return 0;
    }
    return 1;
}

void rtc_handle_ShowTimeAndDate(void)
{
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	char dateBuff[40];
	char timeBuff[40];
	memset(dateBuff,0,sizeof(dateBuff));
	memset(timeBuff,0,sizeof(timeBuff));

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	snprintf(timeBuff, sizeof(timeBuff)-1,"Time : %d:%d:%d \r\n",sTime.Hours,sTime.Minutes,sTime.Seconds);
    osMessageQueuePut(print_queueHandle, &timeBuff, 0, portMAX_DELAY);


	HAL_RTC_GetDate(&hrtc,&sDate, RTC_FORMAT_BIN);
	snprintf(timeBuff, sizeof(timeBuff)-1,"Date : %d/%d/%d \r\n",sDate.Date,sDate.Month,sDate.Year);
    osMessageQueuePut(print_queueHandle, &dateBuff, 0, portMAX_DELAY);



}


