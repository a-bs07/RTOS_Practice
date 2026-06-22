/*
 * task_handle.c
 *
 *  Created on: May 10, 2025
 *      Author: Anubrata
 */
#include "cmsis_os.h"
#include "main.h"

extern TaskHandle_t ledTaskHandle;
extern TaskHandle_t rtcTaskHandle;
extern TaskHandle_t menuTaskHandle;
extern TaskHandle_t printTaskHandle;
extern TaskHandle_t commandTaskHandle;
extern osMessageQueueId_t commandQueueHandle;
extern osMessageQueueId_t print_queueHandle;

extern osTimerId_t myTimer01Handle;
extern osTimerId_t myTimer02Handle;
extern osTimerId_t myTimer03Handle;
extern osTimerId_t myTimer04Handle;

extern UART_HandleTypeDef huart2;

typedef enum {
  TASK_HANDLE_STATE_MAIN_MENU,
  TASK_HANDLE_STATE_LED_EFFECT,
  TASK_HANDLE_STATE_RTC_MENU,
  TASK_HANDLE_STATE_TIME_CONFIG,
  TASK_HANDLE_STATE_DATE_CONFIG,
  TASK_HANDLE_STATE_RTC_REPORT,
  TASK_HANDLE_STATE_MAX,
} state_t;

typedef enum {
  TASK_HANDLE_RTC_STATE_CONFIG_HOUR,
  TASK_HANDLE_RTC_STATE_CONFIG_MIN,
  TASK_HANDLE_RTC_STATE_CONFIG_SEC,
  TASK_HANDLE_RTC_STATE_CHECK_TIME_VALIDITY,
  TASK_HANDLE_RTC_STATE_CONFIG_MAX,
} rtc_time_state_t;

typedef enum {
  TASK_HANDLE_RTC_STATE_CONFIG_DAY,
  TASK_HANDLE_RTC_STATE_CONFIG_MONTH,
  TASK_HANDLE_RTC_STATE_CONFIG_YEAR,
  TASK_HANDLE_RTC_STATE_CHECK_DATE_VALIDITY,
} rtc_date_state_t;

rtc_time_state_t gRtcTimeState = TASK_HANDLE_RTC_STATE_CONFIG_HOUR;
rtc_date_state_t gRtcDateState = TASK_HANDLE_RTC_STATE_CONFIG_DAY;

state_t state = TASK_HANDLE_STATE_MAIN_MENU;

typedef struct {
  uint8_t cmdBUff[10];
  uint8_t len;
} command_t;

static uint8_t extractCommand(command_t* cmd);
static void commandStateMachine(command_t* cmd);
static uint8_t task_handle_getNumber(uint8_t* p, uint8_t len);
static uint8_t task_handle_checkTimeValidity(RTC_TimeTypeDef sTime1);
static uint8_t task_handle_ValidateRtcDate(RTC_DateTypeDef sDate1);
static void led_effect_stop();
static void led_effect(uint8_t option);

static uint8_t extractCommand(command_t* cmd) {
  if (osMessageQueueGetCount(commandQueueHandle) == 0) {
    // queue is empty
    return 0;
  }
  uint8_t index = 0;
  uint8_t item;
  osStatus_t status;
  memset(cmd, 0, sizeof(command_t));
  // extract the queue and store it in command_t structure
  do {
    status = osMessageQueueGet(commandQueueHandle, &(item), NULL, 0);  // store the command in command.cmdBUff
    if (status == osOK) cmd->cmdBUff[index++] = item;
  } while (item != '\n');
  cmd->len = index - 1;
  cmd->cmdBUff[index - 1] = '\0';
  return 1;
}
static void commandStateMachine(command_t* cmd) {
  extractCommand(cmd);  // getting the command stored in queue and storing it in cmd

  switch (state) {
    case TASK_HANDLE_STATE_MAIN_MENU:
      if (cmd->len == 1) {                                                   // since the options are 1,2 and 3 (single character , len =1)
        xTaskNotify(menuTaskHandle, (uint32_t)cmd, eSetValueWithOverwrite);  // sending the cmd address
      }
      break;
    case TASK_HANDLE_STATE_LED_EFFECT:
      xTaskNotify(ledTaskHandle, (uint32_t)cmd, eSetValueWithOverwrite);
      break;
    case TASK_HANDLE_STATE_RTC_MENU:
      xTaskNotify(rtcTaskHandle, (uint32_t)cmd, eSetValueWithOverwrite);
      break;
    case TASK_HANDLE_STATE_TIME_CONFIG:
      xTaskNotify(rtcTaskHandle, (uint32_t)cmd, eSetValueWithOverwrite);
      break;
    case TASK_HANDLE_STATE_DATE_CONFIG:
      xTaskNotify(rtcTaskHandle, (uint32_t)cmd, eSetValueWithOverwrite);
      break;
    case TASK_HANDLE_STATE_RTC_REPORT:
      xTaskNotify(rtcTaskHandle, (uint32_t)cmd, eSetValueWithOverwrite);
      break;

    default:
      state = TASK_HANDLE_STATE_MAIN_MENU;
      break;
  }
  HAL_Delay(1000);
}

static uint8_t task_handle_getNumber(uint8_t* p, uint8_t len) {
  uint8_t val;
  if (len > 1) {
    val = (((p[0] - 48) * 10) + (p[1] - 48));
  } else {
    val = (p[0] - 48);
  }
  return val;
}
static uint8_t task_handle_checkTimeValidity(RTC_TimeTypeDef sTime1) {
  if ((sTime1.Hours <= 12) && (sTime1.Minutes <= 60) && (sTime1.Seconds <= 60)) {
    return 1;
  }
  return 0;
}
static uint8_t task_handle_ValidateRtcDate(RTC_DateTypeDef sDate1) {
  if ((sDate1.Date <= 31) && (sDate1.Month <= 12) && (sDate1.Year <= 99)) {
    return 1;
  }
  return 0;
}
static void led_effect_stop() {
  // stop all timers
  osTimerStop(myTimer01Handle);
  osTimerStop(myTimer02Handle);
  osTimerStop(myTimer03Handle);
  osTimerStop(myTimer04Handle);
}
static void led_effect(uint8_t option)  // this function is generally used to start the  required timer based on the option provided
{
  led_effect_stop();  // stop any previous executing led effect

  switch (option) {
    case 1:
      osTimerStart(myTimer01Handle, 1000);
      break;
    case 2:
      osTimerStart(myTimer02Handle, 1000);
      break;
    case 3:
      osTimerStart(myTimer03Handle, 1000);
      break;
    case 4:
      osTimerStart(myTimer04Handle, 1000);
      break;
    default:
      break;
  }
}

void printTask(void* para) {
  char* msg;
  uint32_t length = 0;
  while (1) {
    osMessageQueueGet(print_queueHandle, &msg, 0, portMAX_DELAY);
    length = strlen(msg);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, length, HAL_MAX_DELAY);
  }
}

void commandTask(void* para) {
  command_t Command;

  while (1) {
    if (xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) == pdPASS) {
      commandStateMachine(&Command);
    }
  }
}

void menuTask(void* para) {
  command_t* cmd;
  uint32_t cmd_addr;

  const char* msg_menu =
      "====================================================\r\n"
      "|                      MENU                        |\r\n"
      "====================================================\r\n"
      "LED Effect    ------------> 1\r\n"
      "Date and Time ------------> 2\r\n"
      "Exit          ------------> 3\r\n"
      "Enter your choice here : \r\n";

  const char* invalid_msg = "//INVALID OPTION // \n";

  while (1) {
    osMessageQueuePut(print_queueHandle, &msg_menu, 0, portMAX_DELAY);

    xTaskNotifyWait(0, 0, &cmd_addr, portMAX_DELAY);
    cmd = (command_t*)cmd_addr;

    if (cmd->len == 1) {
      uint8_t option = cmd->cmdBUff[0] - 48;

      if (option == 1) {
        xTaskNotify(ledTaskHandle, 0, eNoAction);
        state = TASK_HANDLE_STATE_LED_EFFECT;
      } else if (option == 2) {
        xTaskNotify(rtcTaskHandle, 0, eNoAction);
        state = TASK_HANDLE_STATE_RTC_MENU;
      } else if (option == 3) {
        break;
      } else {
        osMessageQueuePut(print_queueHandle, &invalid_msg, 0, portMAX_DELAY);
      }
    } else {
      osMessageQueuePut(print_queueHandle, &invalid_msg, 0, portMAX_DELAY);
      continue;  // to again go to the starting of while , to print the main menu
    }

    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);  // wait for next menu trigger
  }
}

void ledTask(void* param) {
  uint32_t cmd_addr;
  command_t* cmd;
  const char* msg_led =
      "========================\n"
      "|      LED Effect     |\n"
      "========================\n"
      "(none,e1,e2,e3,e4)\n"
      "Enter your choice here : ";
  const char* invalid_msg = "//INVALID OPTION // \n";

  while (1) {
    /*TODO: Wait for notification (Notify wait) */
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    /*TODO: Print LED menu */
    osMessageQueuePut(print_queueHandle, &msg_led, 0, portMAX_DELAY);

    /*TODO: wait for LED command (Notify wait) */
    xTaskNotifyWait(0, 0, (uint32_t*)&cmd_addr, portMAX_DELAY);
    cmd = (command_t*)cmd_addr;  // getting the command address that was received from notify API , to access its members

    if (cmd->len <= 4) {
      if (!strcmp((char*)cmd->cmdBUff, "none"))
        led_effect_stop();
      else if (!strcmp((char*)cmd->cmdBUff, "e1"))
        led_effect(1);
      else if (!strcmp((char*)cmd->cmdBUff, "e2"))
        led_effect(2);
      else if (!strcmp((char*)cmd->cmdBUff, "e3"))
        led_effect(3);
      else if (!strcmp((char*)cmd->cmdBUff, "e4"))
        led_effect(4);
      else {
        osMessageQueuePut(print_queueHandle, &invalid_msg, 0, portMAX_DELAY);
      }
    } else {
      osMessageQueuePut(print_queueHandle, &invalid_msg, 0, portMAX_DELAY);
    }
    /*TODO : update state variable */
    state = TASK_HANDLE_STATE_MAIN_MENU;

    /*TODO : Notify menu task */
    xTaskNotify(menuTaskHandle, 0, eNoAction);
  }
}

void rtcTask(void* param) {
  const char* msg_rtc1 =
      "========================\n"
      "|         RTC          |\n"
      "========================\n";

  const char* msg_rtc2 =
      "Configure Time            ----> 0\n"
      "Configure Date            ----> 1\n"
      "Enable reporting          ----> 2\n"
      "Exit                      ----> 3\n"
      "Enter your choice here : ";

  const char* msg_rtc_hh = "Enter hour(1-12):";
  const char* msg_rtc_mm = "Enter minutes(0-59):";
  const char* msg_rtc_ss = "Enter seconds(0-59):";

  const char* msg_rtc_dd = "Enter date(1-31):";
  const char* msg_rtc_mo = "Enter month(1-12):";
  const char* msg_rtc_yr = "Enter year(0-99):";

  const char* msg_conf = "Configuration successful\n";
  //	const char *msg_rtc_report = "Enable time&date reporting(y/n)?: ";

  RTC_TimeTypeDef sTime1;
  RTC_DateTypeDef sDate1;

  uint32_t cmd_addr;
  command_t* cmd;
  const char* invalid_msg = "-----------INVALID STATE-----------  \r\n";

  while (1) {
    /*TODO: Notify wait (wait till someone notifies)*/
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    osMessageQueuePut(print_queueHandle, &msg_rtc1, 0, portMAX_DELAY);
    /*TODO : Print the menu and show current date and time information */
    rtc_handle_ShowTimeAndDate();
    osMessageQueuePut(print_queueHandle, &msg_rtc2, 0, portMAX_DELAY);

    while (state != TASK_HANDLE_STATE_MAIN_MENU) {
      /*TODO: Wait for command notification (Notify wait) */
      xTaskNotifyWait(0, 0, (uint32_t*)&cmd_addr, portMAX_DELAY);
      cmd = (command_t*)cmd_addr;  // getting the command address that was received from notify API , to access its members
      switch (state) {
        case TASK_HANDLE_STATE_RTC_MENU:

          if (cmd->len == 1) {
            uint8_t option = cmd->cmdBUff[0] - 48;
            if (option == 0) {
              state = TASK_HANDLE_STATE_TIME_CONFIG;
              osMessageQueuePut(print_queueHandle, &msg_rtc_hh, 0, portMAX_DELAY);

            } else if (option == 1) {
              state = TASK_HANDLE_STATE_DATE_CONFIG;
              osMessageQueuePut(print_queueHandle, &msg_rtc_dd, 0, portMAX_DELAY);
            } else if (option == 2) {
              state = TASK_HANDLE_STATE_RTC_REPORT;

            } else if (option == 3) {
              // exit
              xTaskNotify(menuTaskHandle, 0, eNoAction);
              state = TASK_HANDLE_STATE_MAIN_MENU;
            } else {
              osMessageQueuePut(print_queueHandle, &invalid_msg, 0, portMAX_DELAY);
            }
          } else {
            // print invalid
            osMessageQueuePut(print_queueHandle, &invalid_msg, 0, portMAX_DELAY);
            state = TASK_HANDLE_STATE_MAIN_MENU;
          }

          break;

        case TASK_HANDLE_STATE_TIME_CONFIG:

          // hours
          switch (gRtcTimeState) {
            case TASK_HANDLE_RTC_STATE_CONFIG_HOUR: {
              uint8_t hour = 0;
              hour = task_handle_getNumber(cmd->cmdBUff, cmd->len);
              sTime1.Hours = hour;
              state = TASK_HANDLE_RTC_STATE_CONFIG_MIN;
              osMessageQueuePut(print_queueHandle, &msg_rtc_mm, 0, portMAX_DELAY);

              break;
            }
            case TASK_HANDLE_RTC_STATE_CONFIG_MIN: {
              uint8_t min = 0;
              min = task_handle_getNumber(cmd->cmdBUff, cmd->len);
              sTime1.Minutes = min;
              state = TASK_HANDLE_RTC_STATE_CONFIG_SEC;
              osMessageQueuePut(print_queueHandle, &msg_rtc_ss, 0, portMAX_DELAY);
              break;
            }
            case TASK_HANDLE_RTC_STATE_CONFIG_SEC: {
              uint8_t sec = 0;
              sec = task_handle_getNumber(cmd->cmdBUff, cmd->len);
              sTime1.Seconds = sec;
              state = TASK_HANDLE_RTC_STATE_CHECK_TIME_VALIDITY;
              break;
            }
            case TASK_HANDLE_RTC_STATE_CHECK_TIME_VALIDITY: {
              if (task_handle_checkTimeValidity(sTime1) == 1) {
                if (rtc_handle_ConfigTime(&sTime1) == 1) {
                  osMessageQueuePut(print_queueHandle, &msg_conf, 0, portMAX_DELAY);
                  rtc_handle_ShowTimeAndDate();
                }

              } else {
                osMessageQueuePut(print_queueHandle, &invalid_msg, 0, portMAX_DELAY);
              }
              state = TASK_HANDLE_STATE_MAIN_MENU;
              gRtcTimeState = TASK_HANDLE_RTC_STATE_CONFIG_HOUR;

              break;
            }
            default:
              break;
          }  // rtc_state switch ends

          break;

        case TASK_HANDLE_STATE_DATE_CONFIG:

          //					osMessageQueuePut(print_queueHandle, &msg_rtc_dd, 0, portMAX_DELAY);
          xTaskNotifyWait(0, 0, (uint32_t*)&cmd_addr, portMAX_DELAY);
          cmd = (command_t*)cmd_addr;  // getting the command address that was received from notify API , to access its members

          switch (gRtcDateState) {
            case TASK_HANDLE_RTC_STATE_CONFIG_DAY: {
              uint8_t day = 0;
              day = task_handle_getNumber(cmd->cmdBUff, cmd->len);
              sDate1.Date = day;
              gRtcDateState = TASK_HANDLE_RTC_STATE_CONFIG_MONTH;
              osMessageQueuePut(print_queueHandle, &msg_rtc_mo, 0, portMAX_DELAY);

              break;
            }
            case TASK_HANDLE_RTC_STATE_CONFIG_MONTH: {
              uint8_t month = 0;
              month = task_handle_getNumber(cmd->cmdBUff, cmd->len);
              sDate1.Month = month;
              gRtcDateState = TASK_HANDLE_RTC_STATE_CONFIG_YEAR;
              osMessageQueuePut(print_queueHandle, &msg_rtc_yr, 0, portMAX_DELAY);

              break;
            }
            case TASK_HANDLE_RTC_STATE_CONFIG_YEAR: {
              uint8_t year = 0;
              year = task_handle_getNumber(cmd->cmdBUff, cmd->len);
              sDate1.Year = year;
              gRtcDateState = TASK_HANDLE_RTC_STATE_CHECK_DATE_VALIDITY;
              break;
            }
            case TASK_HANDLE_RTC_STATE_CHECK_DATE_VALIDITY: {
              if (task_handle_ValidateRtcDate(sDate1) == 1) {
                if (rtc_handle_ConfigDate(&sDate1) == 1) {
                  osMessageQueuePut(print_queueHandle, &msg_conf, 0, portMAX_DELAY);
                  rtc_handle_ShowTimeAndDate();
                }

              } else {
                osMessageQueuePut(print_queueHandle, &invalid_msg, 0, portMAX_DELAY);
              }
              state = TASK_HANDLE_STATE_MAIN_MENU;
              gRtcDateState = TASK_HANDLE_RTC_STATE_CONFIG_DAY;

              break;
            }
            default:
              break;
          }

          break;

        case TASK_HANDLE_STATE_RTC_REPORT:
          rtc_handle_ShowTimeAndDate();
          state = TASK_HANDLE_STATE_MAIN_MENU;

          /*TODO: enable or disable RTC current time reporting over ITM printf */
          break;
        default:
          break;

      }  // switch end

    }  // while end

    /*TODO : Notify menu task */
    xTaskNotify(menuTaskHandle, 0, eNoAction);

  }  // while super loop end
}
