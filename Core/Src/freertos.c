/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "User/motor.h"
#include "User/oled.h"
#include "usart.h"

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
/* USER CODE BEGIN Variables */
uint16_t usartCnt = 0;
uint16_t beforeCnt = 0;
uint8_t checkTime = 0;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for myTaskMotor */
osThreadId_t myTaskMotorHandle;
const osThreadAttr_t myTaskMotor_attributes = {
  .name = "myTaskMotor",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for myTaskDisplay */
osThreadId_t myTaskDisplayHandle;
const osThreadAttr_t myTaskDisplay_attributes = {
  .name = "myTaskDisplay",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for myTaskUsart */
osThreadId_t myTaskUsartHandle;
const osThreadAttr_t myTaskUsart_attributes = {
  .name = "myTaskUsart",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
// TaskMotor======================================================================
motorInfoType motorInfo[2];
void para_init(){
  for(uint8_t i=0; i<2; i++){
    motorInfo[i].ENC = 0;
    motorInfo[i].PWM = 0;
    motorInfo[i].ADD = 0;
    motorInfo[i].TGT = 0;
  }
}
void check_timeout(){
  if(checkTime++ == 25){
    checkTime = 0;
    if(beforeCnt == usartCnt){
      motorInfo[0].TGT = 0;
      motorInfo[1].TGT = 0;
    } else{
      beforeCnt = usartCnt;
    }
  }
}
// TaskDisplay======================================================================
void show_info(int data, uint8_t x, uint8_t y){
  static uint8_t str[6];
  static uint8_t len;

  len = 6;
  for(uint8_t i=0; i<len; i++){
    str[i] = '0';
  }
  if(data >= 0){
    str[0] = '+';
  }else{
    str[0] = '-';
    data = -data;
  }
  str[--len] = '\0';
  while(data){
    str[--len] = data % 10 + '0';
    data /= 10;
  }

  OLED_ShowString(x, y, str, 8);
}
void OLED_flash_data(){
  show_info(motorInfo[0].ENC, 1+8*6, 0);
  show_info(motorInfo[0].ADD, 1+8*6, 1);
  show_info(motorInfo[0].TGT, 1+8*6, 2);
  show_info(motorInfo[0].PWM, 1+8*6, 3);
  show_info(motorInfo[1].ENC, 1+8*6, 4);
  show_info(motorInfo[1].ADD, 1+8*6, 5);
  show_info(motorInfo[1].TGT, 1+8*6, 6);
  show_info(motorInfo[1].PWM, 1+8*6, 7);
}
// TaskUsart======================================================================
void user_API(uint8_t temp[], uint8_t temp_len){
  if(temp[0]=='[' && temp[3]==']'){
    motorInfo[0].TGT = 128-temp[1];
    motorInfo[1].TGT = temp[2]-128;
    if(usartCnt++ == 60000) usartCnt=0;
  }

  DMA_Usart_Send(rx_buffer, rx_len);
}
void Usart_Handle()     //USART2对接收的�???????帧数据进行处�???????
{
  user_API(rx_buffer, rx_len);

  rx_len = 0;//清除计数
  rec_end_flag = 0;//清除接收结束标志�???????
  memset(rx_buffer, 0, rx_len);
  HAL_UART_Receive_DMA(&huart1, rx_buffer, BUFFER_SIZE);//重新打开DMA接收
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskMotor(void *argument);
void StartTaskDisplay(void *argument);
void StartTaskUsart(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTaskMotor */
  myTaskMotorHandle = osThreadNew(StartTaskMotor, NULL, &myTaskMotor_attributes);

  /* creation of myTaskDisplay */
  myTaskDisplayHandle = osThreadNew(StartTaskDisplay, NULL, &myTaskDisplay_attributes);

  /* creation of myTaskUsart */
  myTaskUsartHandle = osThreadNew(StartTaskUsart, NULL, &myTaskUsart_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskMotor */
/**
* @brief Function implementing the myTaskMotor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskMotor */
void StartTaskMotor(void *argument)
{
  /* USER CODE BEGIN StartTaskMotor */
  para_init();
  /* Infinite loop */
  for(;;)
  {
//    check_timeout();
//    motorInfo[0].TGT = -10;
    check_ENC(&motorInfo[0], &motorInfo[1]);
//    plus_ADD(&motorInfo[0], &motorInfo[1]);
    incremental_PI_A(&motorInfo[0]);
    incremental_PI_B(&motorInfo[1]);
    range_PWM(&motorInfo[0], &motorInfo[1], 7000);
    if(motorInfo[0].TGT==0) motorInfo[0].PWM = 0;
    if(motorInfo[1].TGT==0) motorInfo[1].PWM = 0;
    set_PWM(&motorInfo[0], &motorInfo[1]);
    osDelay(10);
  }
  /* USER CODE END StartTaskMotor */
}

/* USER CODE BEGIN Header_StartTaskDisplay */
/**
* @brief Function implementing the myTaskDisplay thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskDisplay */
void StartTaskDisplay(void *argument)
{
  /* USER CODE BEGIN StartTaskDisplay */
  /* Infinite loop */
  for(;;)
  {
    OLED_flash_data();
    osDelay(50);
  }
  /* USER CODE END StartTaskDisplay */
}

/* USER CODE BEGIN Header_StartTaskUsart */
/**
* @brief Function implementing the myTaskUsart thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUsart */
void StartTaskUsart(void *argument)
{
  /* USER CODE BEGIN StartTaskUsart */
  /* Infinite loop */
  for(;;)
  {
    if(rec_end_flag)  //判断是否接收�????????1帧数�????????
    {
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      Usart_Handle();
    }
    osDelay(1);
  }
  /* USER CODE END StartTaskUsart */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
