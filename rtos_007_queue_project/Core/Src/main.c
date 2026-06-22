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
#include "cmsis_os.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for print_queue */
osMessageQueueId_t print_queueHandle;
const osMessageQueueAttr_t print_queue_attributes = {.name = "print_queue"};
/* Definitions for commandQueue */
osMessageQueueId_t commandQueueHandle;
const osMessageQueueAttr_t commandQueue_attributes = {.name = "commandQueue"};
/* Definitions for myTimer01 */
osTimerId_t myTimer01Handle;
const osTimerAttr_t myTimer01_attributes = {.name = "myTimer01"};
/* Definitions for myTimer02 */
osTimerId_t myTimer02Handle;
const osTimerAttr_t myTimer02_attributes = {.name = "myTimer02"};
/* Definitions for myTimer03 */
osTimerId_t myTimer03Handle;
const osTimerAttr_t myTimer03_attributes = {.name = "myTimer03"};
/* Definitions for myTimer04 */
osTimerId_t myTimer04Handle;
const osTimerAttr_t myTimer04_attributes = {.name = "myTimer04"};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
void StartDefaultTask(void *argument);
void Callback01(void *argument);
void Callback02(void *argument);
void Callback03(void *argument);
void Callback04(void *argument);

/* USER CODE BEGIN PFP */
TaskHandle_t ledTaskHandle;
TaskHandle_t rtcTaskHandle;
TaskHandle_t menuTaskHandle;
TaskHandle_t printTaskHandle;
TaskHandle_t commandTaskHandle;

// software timer handle

// osMessageQueueId_t gCurrentQueueHandle=commandQueue_attributes;
// TaskHandle_t* pcurrentHandle=&menuTaskHandle;
TaskHandle_t *pcurrentHandle = &commandTaskHandle;
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

volatile uint8_t gRxData = 0;

uint8_t enqueueDataFromIsr() {
  osStatus_t status = 0;
  status = osMessageQueuePut(commandQueueHandle, (void *)&gRxData, 1, 0);
  if (status == osOK) {
    return 1;
  }
  return 0;
}

void notifyCommandTask(void) {
  BaseType_t pxHigherPriorityTaskWoken = pdFAIL;
  xTaskNotifyFromISR(commandTaskHandle, 0, eNoAction, &pxHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);  // requesting context switching manually
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  UNUSED(huart);
  if (osMessageQueueGetSpace(commandQueueHandle) != 0)  // first we check whether queue is empty or not
  {                                                     // queue is not full
    enqueueDataFromIsr();
  } else {
    // if queue is full : check if the gRxData is '\n' put it at the end of queue , else ignore
    if (gRxData == '\n') {
      uint8_t dummydata = 0;
      osMessageQueueGet(commandQueueHandle, (void *)&dummydata, NULL, 0);   // deleting last data
      osMessageQueuePut(commandQueueHandle, (uint8_t *)&gRxData, 1, 1000);  // storing'\n' in last byte
                                                                            //		  notifyCommandTask();
    }
  }
  if (gRxData == '\n') {
    notifyCommandTask();
  }
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&gRxData, 1);
}

void printHeapSize() {
  size_t freeHeap = xPortGetFreeHeapSize();
  uint8_t buff[50] = {0};
  int len = snprintf((char *)buff, sizeof(buff), "Free Heap Size: %u\r\n", freeHeap);

  HAL_UART_Transmit(&huart2, buff, len, 1000);  // Send the exact number of characters written

  //	    printf("Free Heap Size: %u\n", freeHeap);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Init scheduler */
  //  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of myTimer01 */
  myTimer01Handle = osTimerNew(Callback01, osTimerPeriodic, NULL, &myTimer01_attributes);

  /* creation of myTimer02 */
  myTimer02Handle = osTimerNew(Callback02, osTimerPeriodic, NULL, &myTimer02_attributes);

  /* creation of myTimer03 */
  myTimer03Handle = osTimerNew(Callback03, osTimerPeriodic, NULL, &myTimer03_attributes);

  /* creation of myTimer04 */
  myTimer04Handle = osTimerNew(Callback04, osTimerPeriodic, NULL, &myTimer04_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of print_queue */
  print_queueHandle = osMessageQueueNew(100, sizeof(char *), &print_queue_attributes);

  /* creation of commandQueue */
  commandQueueHandle = osMessageQueueNew(100, sizeof(uint16_t), &commandQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  status = xTaskCreate(ledTask, "led_task", 200, NULL, 2, &ledTaskHandle);
  if (status != pdPASS) {
    printf("led task creation failed\r\n");
  }
  status = xTaskCreate(rtcTask, "rtc_task", 200, NULL, 2, &rtcTaskHandle);
  if (status != pdPASS) {
    printf("rtc_task creation failed\r\n");
  }
  status = xTaskCreate(menuTask, "menu_task", 200, NULL, 2, &menuTaskHandle);
  if (status != pdPASS) {
    printf("main_task creation failed\r\n");
  }
  status = xTaskCreate(printTask, "print_task", 200, NULL, 2, &printTaskHandle);
  printHeapSize();
  if (status != pdPASS) {
    printf("print_task creation failed\r\n");
  }

  printHeapSize();
  status = xTaskCreate(commandTask, "command_task", 200, NULL, 2, &commandTaskHandle);
  if (status != pdPASS) {
    printf("command_task creation failed\r\n");
  }
  // suspending all task except menu task.
  printHeapSize();
  //
  //        TimerHandle_t xTimerCreate
  //                        ( const char * const pcTimerName,
  //                          const TickType_t xTimerPeriod,
  //                          const UBaseType_t uxAutoReload,
  //                          void * const pvTimerID,
  //                          TimerCallbackFunction_t pxCallbackFunction );

  HAL_UART_Receive_IT(&huart2, (uint8_t *)&gRxData, 1);

  vTaskStartScheduler();
  HAL_UART_Transmit(&huart2, (uint8_t *)"Scheduler initialization failed\r\n", sizeof("Scheduler initialization failed\r\n") - 1, 1000);
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  //  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void) {
  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
   */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_12;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {
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
  if (HAL_UART_Init(&huart2) != HAL_OK) {
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
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin | LED_WHITE_Pin | LED_RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_GREEN_Pin LED_WHITE_Pin LED_RED_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin | LED_WHITE_Pin | LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for (;;) {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* Callback01 function */
void Callback01(void *argument) {
  /* USER CODE BEGIN Callback01 */
  LED_Effect1();
  /* USER CODE END Callback01 */
}

/* Callback02 function */
void Callback02(void *argument) {
  /* USER CODE BEGIN Callback02 */
  LED_Effect2();
  /* USER CODE END Callback02 */
}

/* Callback03 function */
void Callback03(void *argument) {
  /* USER CODE BEGIN Callback03 */
  LED_Effect3();
  /* USER CODE END Callback03 */
}

/* Callback04 function */
void Callback04(void *argument) {
  /* USER CODE BEGIN Callback04 */
  LED_Effect4();
  /* USER CODE END Callback04 */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
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
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
