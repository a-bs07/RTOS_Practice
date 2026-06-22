/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"adxl345_driver.h"
#include"stdbool.h"
#include"string.h"
#include"stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
 static volatile bool gStopflag=1;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for command_queue */
osMessageQueueId_t command_queueHandle;
const osMessageQueueAttr_t command_queue_attributes = {
  .name = "command_queue"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */
void command_task(void* para);
void sensor_task(void* para);

uint8_t isrBuff[40]={0};
volatile uint8_t gRxData = 0;
static volatile uint8_t gRxBuffer[15];

TaskHandle_t command_task_handle;
TaskHandle_t sensor_task_handle;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* _GNUC_ */

PUTCHAR_PROTOTYPE {
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

static void notifyCommandTask(void)
{
	BaseType_t pxHigherPriorityTaskWoken = pdFAIL;
	xTaskNotifyFromISR(command_task_handle, 0, eNoAction, &pxHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);  // requesting context switching manually
}

uint8_t enqueueDataFromIsr() {
  osStatus_t status = 0;
  status = osMessageQueuePut(command_queueHandle, (void *)&gRxData, 1, 0);
  if (status == osOK) {
    return 1;
  }
  return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  UNUSED(huart);
  static int index=0;
  if(osMessageQueueGetSpace(command_queueHandle)!=0)
  {
	  if(index==0 && gRxData =='\n' )
	  {

	  }
	  else
	  {
		  enqueueDataFromIsr();
		  isrBuff[index]=gRxData;// for test
		  index++;// for test
	  }

  }
  else
  {
	  if(gRxData=='\n')
	  {
		    isrBuff[index]=gRxData;// for test

		  index=0;
		  uint8_t dummydata = 0;
		  osMessageQueueGet(command_queueHandle, (void *)&dummydata, NULL, 0);   // deleting last data
		  enqueueDataFromIsr();
	  }
  }
  if(gRxData=='\n')
  {
	  index=0;//test
	  isrBuff[index]=gRxData;// for test
	  enqueueDataFromIsr();// this is not required
	  memset(&isrBuff,0,sizeof(isrBuff));// test
	  notifyCommandTask();
  }
  HAL_UART_Receive_IT(&huart2, &gRxData,1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	BaseType_t status;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  bool status1 =adxl_app_StreamModeInit();
	adxl_app_calculate_offsets();
	if (status1)
		while (1)
			;

	  printf("stream mode init successful \r\n");

  /* USER CODE END 2 */

  /* Init scheduler */
//  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of command_queue */
  command_queueHandle = osMessageQueueNew (16, sizeof(uint8_t), &command_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
//  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  status=xTaskCreate(command_task, "command", 200, NULL,1,&command_task_handle);
  configASSERT(status==pdPASS);
  status=xTaskCreate(sensor_task, "sensor", 200, NULL,1,&sensor_task_handle);
  configASSERT(status==pdPASS);

  printf("Task creation complete successful \r\n");
  HAL_UART_Receive_IT(&huart2, &gRxData, 1);
  vTaskStartScheduler();

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
//  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  /* USER CODE END 2 */

  /* Init scheduler */
//  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of command_queue */
  command_queueHandle = osMessageQueueNew (16, sizeof(uint8_t), &command_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
//  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  status=xTaskCreate(command_task, "command", 200, NULL,1,&command_task_handle);
  configASSERT(status==pdPASS);
  status=xTaskCreate(sensor_task, "sensor", 200, NULL,1,&sensor_task_handle);
  configASSERT(status==pdPASS);

  printf("Task creation complete successful \r\n");
  vTaskStartScheduler();

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
//  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00707CBB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


static uint8_t extractCommandFromQueue(char * buff)
{
    char dummyBuff[20] = {0};
    uint8_t index = 0;
    uint8_t item;
    while (index < sizeof(dummyBuff) - 1) // Prevent buffer overflow
    {
        if (osMessageQueueGet(command_queueHandle, &item, NULL, portMAX_DELAY) == osOK)
        {
            dummyBuff[index] = item;
            if (item == '\n')
            {
                break;
            }
            index++;
        }
        else
        {
            // Queue is empty or error, handle accordingly
            break;
        }
    }
    dummyBuff[index] = '\0'; // Null-terminate the received command
	memcpy(buff,dummyBuff,strlen(dummyBuff)+1);
	osMessageQueueReset(command_queueHandle);//clearing the queue after extracting the cmd

    return 1; // Or some status indication
}
/*
// good
//pair 1: command task
void command_task(void* para)
{
	 char responseBuff[30]={0};
	 const char * menu ="====================================================\r\n"
			            "|                      Accelerometer Data            |\r\n"
			            "====================================================\r\n"
			            "Enter [start] to view data  \r\n"
			            "Enter [stop]  to stop viewing data \r\n"
			            "Enter your choice here : \r\n";

	 // send the data using print queue
    printf("%s \r\n",menu);
	while(1)
	{
		xTaskNotifyWait(0, 0,NULL,portMAX_DELAY);// wait for the user to enter option
		memset(responseBuff,0,sizeof(responseBuff));
		extractCommandFromQueue(responseBuff);
//        printf("Received command: [%s]\r\n", responseBuff); // For debugging

//		if(strcmp(responseBuff,"start")==0)
        if(strstr(responseBuff,"start")!=NULL)
		{
			gStopflag=0;
			// start showing data and wait until user input any data
			xTaskNotify(sensor_task_handle,0,eNoAction);// after notifying the sensor_task() it again wait for the user notificatio(i/p)n
		}
		else if(strstr(responseBuff,"stop")!=NULL)
		{
			if(gStopflag==1)
			{
				printf("Accelerometer is already in stop state \r\n");
			}
			else
			{
				gStopflag=1;
			}
			printf("%s", menu); // Print the menu again after stopping
		}
		else
		{
			if(responseBuff[0]!='\0') // this is because we were doing memset to 0 whose ascii is '\0', so it was considered as an invalid response
			{
				printf("Invalid command. Please enter 'start' or 'stop'  .\r\n");
				printf("%s", menu); // Print the menu again after stopping
			}
		}

	}


}

*/

// pair 1 : sensor task
/*void sensor_task(void* para)
{
	while(1)
	{
	    xTaskNotifyWait(0, 0,NULL,portMAX_DELAY);// wait for the command task to notify
		while(gStopflag != 1)
		        {
		            adxl_app_Printdata();
		            osDelay(pdMS_TO_TICKS(1000)); // Add a small delay for readability
		        }

	}


}
// in the above task , if you look carefully , the task gets notified only once , and n the basis of the gStopflag, it either prints
//data or doesn't , but it doesn't goes to blocked state ( hence CPU cycle wasted) Below is the better solution .
*/
// better :



// pair 2 command task

void command_task(void* para)
{
	 char responseBuff[30]={0};
	 const char * menu ="====================================================\r\n"
			            "|                      Accelerometer Data            |\r\n"
			            "====================================================\r\n"
			            "Enter [start] to view data  \r\n"
			            "Enter [stop]  to stop viewing data \r\n"
			            "Enter your choice here : \r\n";

	 // send the data using print queue
    printf("%s \r\n",menu);
	while(1)
	{
		xTaskNotifyWait(0, 0,NULL,portMAX_DELAY);// wait for the user to enter option
		memset(responseBuff,0,sizeof(responseBuff));
		extractCommandFromQueue(responseBuff);
//        printf("Received command: [%s]\r\n", responseBuff); // For debugging

//		if(strcmp(responseBuff,"start")==0)
        if(strstr(responseBuff,"start")!=NULL)
		{
			gStopflag=0;
			// start showing data and wait until user input any data
			xTaskNotify(sensor_task_handle,0,eNoAction);// after notifying the sensor_task() it again wait for the user notificatio(i/p)n
		}
		else if(strstr(responseBuff,"stop")!=NULL)
		{
			if(gStopflag==1)
			{
				printf("Accelerometer is already in stop state \r\n");
			}
			else
			{
				xTaskNotify(sensor_task_handle,0,eNoAction);// after notifying the sensor_task() it again wait for the user notificatio(i/p)n
				gStopflag=1;
			}
			printf("%s", menu); // Print the menu again after stopping
		}
		else
		{
			if(responseBuff[0]!='\0') // this is because we were doing memset to 0 whose ascii is '\0', so it was considered as an invalid response
			{
				printf("Invalid command. Please enter 'start' or 'stop'  .\r\n");
				printf("%s", menu); // Print the menu again after stopping
			}
		}

	}


}
void sensor_task(void* para)
{
    for (;;)
    {
        // Block until "start" command arrives
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

        // Now stream data until "stop" arrives
        while (1)
        {
            adxl_app_Printdata();
            vTaskDelay(pdMS_TO_TICKS(1000));

            // Non-blocking check for stop notification
            if (xTaskNotifyWait(0, 0, NULL, 0) == pdPASS)
            {
                break;  // Stop received → go back to blocked state
            }
        }
    }
}


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
