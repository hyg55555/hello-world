/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "dma.h"
#include "iwdg.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CD4051BM_driver.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define ID 1
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void send_data_to_uart(char *data, uint8_t len)
{
  HAL_GPIO_WritePin(R4_EN_GPIO_Port, R4_EN_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart4, (uint8_t *)data, len, 1000);
  HAL_GPIO_WritePin(R4_EN_GPIO_Port, R4_EN_Pin, GPIO_PIN_RESET);
}

void mbus_pack(uint8_t *buf, uint8_t addr, uint8_t cmd, uint8_t len, uint8_t *data) {
    uint8_t i, sum = 0;
    buf[0] = addr;
    buf[1] = cmd;
    buf[2] = len;
    for (i = 0; i < len; i++) {
        buf[3 + i] = data[i];
    }
    for (i = 0; i < len + 3; i++) {
        sum += buf[i];
    }
    buf[len + 3] = sum; // 校验�?
}

// void package_data(uint8_t *buff, uint8_t *data, uint8_t len)
// {
  
//   CD4051BM_scan();

//   uint8_t data[16] = {0};
//   for (int i = 0; i < 6; i++)
//   {
//     get_CD4051BM_state(&data[i+3], i);
//   }
//   uint8_t sum = 0;
//   for (int i = 0; i < 9; i++)
//   {
//     sum += data[i];
//   }
//   data[9] = sum;
//   send_data_to_uart(data, 10);
// }

static void deal_rx_data(uint8_t *buff,uint16_t len)
{
  // 从机地址 | 命令�???   | 数据长度 | 数据 | 校验�???
  if(buff[0] != ID)
  {
    return;
  }

  // 校验
  uint8_t sum = 0;
  for(int i=0;i<len-1;i++)
  {
    sum += buff[i];
  }
  if(sum != buff[len-1])
  {
    return;
  }

  // 命令
  if(buff[1] == 0x01)
  {
    CD4051BM_scan();

    uint8_t data[6] = {0};
    for (int i = 0; i < 6; i++)
    {
      get_CD4051BM_state(&data[i], i);
    }
    uint8_t buf[20] = {0};
    mbus_pack(buf, ID, 0x01, 6, data);
    send_data_to_uart(buf, 10);
  }
}
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
  MX_IWDG_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(R4_EN_GPIO_Port, R4_EN_Pin, GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  static int led_tick = 0;
  while (1)
  {
    //CD4051BM_scan();
    if(rx_flag == 1)
    {
      deal_rx_data(rx_buff, rx_len);
      memset(rx_buff,0,100);
      rx_flag = 0;
      // 重新启动接收
      HAL_UART_Receive_DMA(&huart4, rx_buff, 100);
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_IWDG_Refresh(&hiwdg);
    // led_tick++;
    // if (led_tick % 5 == 0)
    // {
    //   HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    // }
    //HAL_Delay(10);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
