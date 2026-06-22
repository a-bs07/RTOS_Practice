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

## 🛠️ Technologies & Tools
• **OS/Framework:** FreeRTOS
• **Language:** C / Embedded C
• **Target Architectures:** ARM Cortex-M (STM32) 
