# RTOS_basics

Welcome to the **RTOS_basics** repository. This project serves as a practical learning path and code reference for fundamental Real-Time Operating System (RTOS) concepts, focusing primarily on FreeRTOS implementations for embedded systems. 

## 📁 Repository Structure

The repository is organized into distinct, modular folders. Each directory contains a standalone project demonstrating a specific RTOS feature, API usage, or hardware integration concept:

### Core Task Management
• **Rtos_001Task**: Fundamentals of task creation, execution, and basic scheduling.
• **rtos_003_vtaskdelay**: Implementation of relative blocking delays using the `vTaskDelay` API.
• **rtos_004_vtaskDelayUntil**: Implementation of absolute timing and precise periodic tasks using `vTaskDelayUntil`.
• **rtos_004_notifyAndDeleteTask**: Safe practices for task deletion and state management.

### Inter-Task Communication & Synchronization
• **rtos_002_notify**: Using Direct to Task Notifications for lightweight synchronization.
• **rtos_007_queue_project**: Passing data securely between executing tasks using FreeRTOS Queues.
• **rtos_011_mutual_exclusion_using_semaphore**: Protecting shared resources and avoiding race conditions using Mutexes and Semaphores.

### Interrupts & Timers
• **Timer_test**: Creating and managing FreeRTOS software timers (auto-reloading and one-shot).
• **rtos_009_button_INT_self**: Handling external hardware button interrupts and deferring processing to RTOS tasks.
• **rrtos_006_notifyFromISR**: Unblocking tasks directly from Interrupt Service Routines (ISRs) using `vTaskNotifyGiveFromISR`.

### Hardware & Sensor Integration
• **rtos_008_adxl_self**: Integrating an ADXL accelerometer sensor within a multi-tasking environment.
• **rtos_010_adxl_with_wakeup**: Advanced ADXL implementation utilizing hardware wake-up interrupts to trigger RTOS events.
• **FreeRTOS_002**: General hardware peripheral and RTOS initialization routines.

## Technologies & Tools
• **OS/Framework:** FreeRTOS
• **Language:** C / Embedded C
• **Target Architectures:** ARM Cortex-M (STM32) 

---

##  STM32CubeIDE FreeRTOS Configuration

To replicate these projects or start your own FreeRTOS project from scratch using STM32CubeIDE, follow these essential configuration steps inside the `.ioc` Device Configuration Tool:

### 1. Change the Timebase Source (Crucial)
FreeRTOS requires the `SysTick` timer to manage its own task scheduler. Therefore, you must assign a different hardware timer to the STM32 HAL timebase.
* Navigate to **System Core** -> **SYS**.
* Change the **Timebase Source** from `SysTick` to any available basic timer (e.g., `TIM1`, `TIM6`, or `TIM11`).

### 2. Enable FreeRTOS Middleware
* Navigate to **Middleware and Software Packs** (or just Middleware) -> **FREERTOS**.
* In the "Interface" dropdown, select **CMSIS_V1** or **CMSIS_V2** (V2 is recommended for newer projects).

### 3. Configure RTOS Parameters
Under the Configuration pane below the Middleware section, adjust the following tabs as needed:
* **Config parameters:** * Ensure the `TICK_RATE_HZ` is set to `1000` (1 ms tick).
  * Adjust `TOTAL_HEAP_SIZE` if your application requires more memory for tasks, queues, or dynamic allocation.
* **Tasks and Queues:** * Here you can define your initial tasks, assign their priorities (e.g., `osPriorityNormal`), and allocate their stack sizes before generating the code.

### 4. Generate Code
* Once configured, save the `.ioc` file (Ctrl + S) or click the gear icon to **Generate Code**. 
* STM32CubeIDE will automatically generate the FreeRTOS initialization code, `osKernelStart()`, and the weak task handlers in your `main.c` file.
