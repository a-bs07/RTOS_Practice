/*
 * led_handle.c
 *
 *  Created on: Apr 30, 2025
 *      Author: Anubrata
 */

#include"main.h"

static void led_handle_TurnOnAllLEDs(void)
{
	 HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin|LED_WHITE_Pin|LED_RED_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
}

static void led_handle_TurnOffAllLEDs(void)
{
	 HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin|LED_WHITE_Pin|LED_RED_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}

static void led_effect_EvenLedOn()
{
	HAL_GPIO_WritePin(GPIOA, LED_WHITE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_RESET);

}
static void led_effect_OddLedOn()
{
	HAL_GPIO_WritePin(GPIOA, LED_WHITE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_SET);

}


void  LED_Effect1(void )
{
 static uint8_t effect1Flag=1;
 if(effect1Flag==1)
 {
	 led_handle_TurnOnAllLEDs();
	 effect1Flag=0;
 }
 else
 {
	 led_handle_TurnOffAllLEDs();
	 effect1Flag=1;
 }

 // or we can simply use
}

//
//void  LED_Effect1(void )
//{
//	HAL_GPIO_TogglePin(GPIOA, LED_GREEN_Pin|LED_WHITE_Pin|LED_RED_Pin);
//	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
//}



void  LED_Effect2(void )
{
 static uint8_t effect2Flag=0;
 if(effect2Flag==0)
 {
	 effect2Flag=1;
	 led_effect_EvenLedOn();
 }
 else
 {
	 effect2Flag=0;
	 led_effect_OddLedOn();

 }
}
void  LED_Effect3(void )
{
static uint8_t option=1;
 switch(option)
 {
 case 1:

		HAL_GPIO_WritePin(GPIOA,LED_WHITE_Pin|LED_RED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin, GPIO_PIN_SET);
		option=2;
	 break;
 case 2:

		HAL_GPIO_WritePin(GPIOA,LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA,LED_WHITE_Pin ,GPIO_PIN_SET);
		option=3;
	 break;
 case 3 :
		HAL_GPIO_WritePin(GPIOA,LED_GREEN_Pin|LED_WHITE_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA,LED_RED_Pin ,GPIO_PIN_SET);
		option=4;
     break;
 case 4:
		HAL_GPIO_WritePin(GPIOA,LED_GREEN_Pin|LED_WHITE_Pin|LED_RED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		option=1;
	 break;
 default:
	 break;

 }
}
void  LED_Effect4(void )
{
	static uint8_t option4=4;
	 switch(option4)
	 {
	 case 1:

			HAL_GPIO_WritePin(GPIOA,LED_WHITE_Pin|LED_RED_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin, GPIO_PIN_SET);
			option4=4;
		 break;
	 case 2:

			HAL_GPIO_WritePin(GPIOA,LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,LED_WHITE_Pin ,GPIO_PIN_SET);
			option4=1;
		 break;
	 case 3 :
			HAL_GPIO_WritePin(GPIOA,LED_GREEN_Pin|LED_WHITE_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,LED_RED_Pin ,GPIO_PIN_SET);
			option4=2;
	     break;
	 case 4:
			HAL_GPIO_WritePin(GPIOA,LED_GREEN_Pin|LED_WHITE_Pin|LED_RED_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			option4=3;
		 break;
	 default:
		 break;

	 }
}

