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
#include "can.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include "sms_c_api.h"
#include "stdio.h"
#include "string.h"
// #include "STS_control.h"
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
uint8_t servo_enable=0;
uint8_t servo_id[1] = {1, };
uint16_t servo_position[sizeof(servo_id)];
uint16_t servo_speed[sizeof(servo_id)];
uint8_t servo_rxpacket[4];
uint16_t servo_set_position[sizeof(servo_id)];
uint16_t servo_set_speed[sizeof(servo_id)];
uint16_t servo_set_acc[sizeof(servo_id)];
uint16_t default_speed = 3250;
uint16_t default_acc = 0;
uint8_t servo_can_rx[8] = {0};
uint8_t servo_can_tx[8] = {0};
uint8_t servo_read_flag_12 = 0;
uint8_t servo_read_flag_3 = 0;
uint8_t servo_write_flag = 0;


/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // char *msg = "Hello from STM32F103 via USB CDC\r\n";
  // CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
  // STS_control_init(); // Removed STS dependency
  int cur_i = 0;
  int cur_target = 0;

  // set filter
  CAN_FilterTypeDef filterConfig;
  filterConfig.FilterActivation = ENABLE;
  filterConfig.FilterBank = 0;
  filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
  filterConfig.FilterIdHigh = 0x0000;
  filterConfig.FilterIdLow = 0x0000;
  filterConfig.FilterMaskIdHigh = 0x0000;
  filterConfig.FilterMaskIdLow = 0x0000;
  filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  HAL_CAN_ConfigFilter(&hcan, &filterConfig);
  
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);

  CAN_TxHeaderTypeDef TxHeader;
  uint8_t TxData[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  uint32_t TxMailbox;

  TxHeader.StdId = 0x000;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 8;

  uint8_t uart1_rec_buffer[100] = {0};
  uint8_t uart1_trans_buffer[100] = {0};
  
  // while ((CAN1->TSR & CAN_TSR_TME) == 0) {
  //   int x = 0;
  // }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  // Servo control functions for ID=1
  void servo_enable_torque(void) {
    uint8_t cmd[] = {0xFF, 0xFF, 0x01, 0x04, 0x03, 0x28, 0x01, 0xCE};
    HAL_UART_Transmit(&huart2, cmd, 8, 100);
  }
  
  void servo_disable_torque(void) {
    uint8_t cmd[] = {0xFF, 0xFF, 0x01, 0x04, 0x03, 0x28, 0x00, 0xCF};
    HAL_UART_Transmit(&huart2, cmd, 8, 100);
  }
  
  void servo_read_position(void) {
    uint8_t cmd[] = {0xFF, 0xFF, 0x01, 0x04, 0x02, 0x38, 0x02, 0xBE};
    uint8_t response[8];
    HAL_UART_Transmit(&huart2, cmd, 8, 100);
    if(HAL_UART_Receive(&huart2, response, 8, 100) == HAL_OK) {
      // Validate response header
      if(response[0] == 0xFF && response[1] == 0xFF) {
        // Extract position from bytes 5 and 6 (0-indexed)
        uint16_t position = response[5] | (response[6] << 8);  // Low byte first, then high byte
        servo_position[0] = position;  // Store the angle
      }
    }
  }
  

  void servo_position_control(uint16_t position, uint16_t velocity) {
    // Based on the image: FF FF 01 09 03 2A E8 03 00 E8 03 F2 42
    uint8_t cmd[13] = {0xFF, 0xFF, 0x01, 0x09, 0x03, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    // Set position (low byte first, then high byte)
    cmd[6] = (uint8_t)(position & 0xFF);
    cmd[7] = (uint8_t)(position >> 8);
    
    // Time bytes are reserved (set to 00)
    cmd[8] = 0x00;
    cmd[9] = 0x00;
    
    // Set velocity (low byte first, then high byte)  
    cmd[10] = (uint8_t)(velocity & 0xFF);
    cmd[11] = (uint8_t)(velocity >> 8);
    
    // Calculate checksum using BITXOR method: XOR(SUM & 0xFF, 0xFF)
    uint8_t sum = 0;
    for(int i = 2; i < 12; i++) {
      sum += cmd[i];
    }
    cmd[12] = (sum & 0xFF) ^ 0xFF;
    
    uint8_t response[6];
    HAL_UART_Transmit(&huart2, cmd, 13, 100);
    // Expected response: FF FF 01 02 00 FC
    HAL_UART_Receive(&huart2, response, 6, 100);
  }
  
  uint8_t test_buffer[10] = {0x00, 0x0a, 0x00, 0x01, 0x00, 0x00, 0x01 ,0x00, 0x00, 0x00};
  uint8_t send_buffer[10] = {0x00, 0x09, 0x00, 0x01, 0x00, 0x00, 0x01 ,0x00, 0x00, 0x00}; // ID 7 means read servo 1 and 2's data
  while (1)
  {
    // HAL_UART_Transmit(&huart1, send_buffer, 10, 100);
    // HAL_Delay(300);  // 0.3s delay 
    
    // if(HAL_OK==HAL_UART_Receive(&huart1, uart1_rec_buffer, 10, 100)){ // len is 10 and timeout is 100

    //   HAL_UART_Transmit(&huart1, uart1_rec_buffer, 10, 100);
    // }

    
    if(HAL_OK!=HAL_UART_Receive(&huart1, uart1_rec_buffer, 10, 100)){ // len is 10 and timeout is 100
    // if(1==1){ // len is 10 and timeout is 100
      continue;
    }else{

      HAL_UART_Transmit(&huart1, test_buffer, 10, 200);
      uint16_t can_id = (uint16_t)(uart1_rec_buffer[0] << 8) | uart1_rec_buffer[1];
      switch(can_id) {
        case 0x06: 
          switch (uart1_rec_buffer[2])
          {
          case 0x00:
            servo_enable = 0;
            servo_disable_torque();
            break;
          
          case 0x01:
            servo_enable = 1;
            servo_enable_torque();
            
            break;
          case 0x02:
            servo_write_flag =1 ; 
            // servo_position_control();
          // servo_write_flag = 1;
          // memcpy(servo_can_rx, uart1_rec_buffer + 2, 8);
          //   break;
          }
          break;

        case 0x07:
          servo_read_flag_12 = 1;
          break;
        // case 0x08:
        //   servo_read_flag_3 = 1;
        //   break;
      }
    }


    if(servo_read_flag_12 == 1) {
      servo_read_position();
      // send first two servos' data
      for(uint8_t i = 0; i < 1; i++) {  
        servo_can_tx[4 * i] = (uint8_t)(servo_position[i] >> 8);
        servo_can_tx[4 * i + 1] = (uint8_t)(servo_position[i] & 0xFF);
        servo_can_tx[4 * i + 2] = (uint8_t)(servo_speed[i] >> 8);
        servo_can_tx[4 * i + 3] = (uint8_t)(servo_speed[i] & 0xFF);
      }
      // if(HAL_CAN_AddTxMessage(&hcan, &TxHeader, servo_can_tx, &TxMailbox) != HAL_OK) {
      //   int test = 0;
      // }
      memcpy(uart1_trans_buffer + 2, servo_can_tx, 8);
      HAL_UART_Transmit(&huart1, uart1_trans_buffer, 10, 100);
      servo_read_flag_12 = 0;
    }

    // if(servo_read_flag_3 == 1) {
    //   STS_syn_read();
    //   // send third servo's data
    //   servo_can_tx[0] = (uint8_t)(servo_position[2] >> 8);
    //   servo_can_tx[1] = (uint8_t)(servo_position[2] & 0xFF);
    //   servo_can_tx[2] = (uint8_t)(servo_speed[2] >> 8);
    //   servo_can_tx[3] = (uint8_t)(servo_speed[2] & 0xFF);
    //   servo_can_tx[4] = 0;
    //   servo_can_tx[5] = 0;
    //   servo_can_tx[6] = 0;
    //   servo_can_tx[7] = 0;
    //   // if(HAL_CAN_AddTxMessage(&hcan, &TxHeader, servo_can_tx, &TxMailbox) != HAL_OK) {
    //   //   int test = 0;
    //   // }
    //   memcpy(uart1_trans_buffer + 2, servo_can_tx, 8);
    //   HAL_UART_Transmit(&huart1, uart1_trans_buffer, 10, 100);
    //   servo_read_flag_3 = 0;
    // }

    if(servo_write_flag == 1) {
      // Check if this is a position control command
      if(uart1_rec_buffer[2] == 0x02) {
        // Extract position from bytes 3-4 and velocity from bytes 5-6
        uint16_t position = ((uint16_t)uart1_rec_buffer[4] << 8) | uart1_rec_buffer[3];
        uint16_t velocity = ((uint16_t)uart1_rec_buffer[6] << 8) | uart1_rec_buffer[5];
        
        // Call servo position control with extracted data
        servo_position_control(position, velocity);
      }
      
      servo_write_flag = 0;
    }


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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
#ifdef USE_FULL_ASSERT
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
