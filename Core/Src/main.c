/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nmea.h"
#include "lvgl/lvgl.h"
#include "lvgl/examples/porting/lv_port_disp.h"
#include "als.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 2048
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
lv_disp_drv_t * disp_drv;
extern DMA_HandleTypeDef hdma_usart2_rx, hdma_usart3_rx;
uint16_t ADC[2], mVBat;
uint8_t RxBuffer[RX_BUFFER_SIZE];
extern NMEAData_t NMEAData;
lv_obj_t *mVBat_label, *locateStatus_label, *speed_label, *course_label, *time_label,
*quality_label, *satelliteCount_label, *altitude_label, *hdop_label, *vdop_label;
uint8_t brightness_tmp[4], als_data[3];
uint16_t brightness;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LVGL_DMA_pCallback(DMA_HandleTypeDef *_hdma);
void LVGL_UI_Init(void);
void LVGL_UI_Refresh(void);
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
  MX_ADC1_Init();
  MX_FSMC_Init();
  MX_TIM5_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_USART3_UART_Init();
  MX_I2C2_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1);
  lv_init();
  lv_port_disp_init();
  LVGL_UI_Init();
  disp_drv = lv_disp_get_default()->driver;
  HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_channel1, HAL_DMA_XFER_CPLT_CB_ID, LVGL_DMA_pCallback);
  ALS_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC[0], 2);
  HAL_UARTEx_ReceiveToIdle_DMA(&UART_GNSS, &RxBuffer[0], RX_BUFFER_SIZE);
  __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
  HAL_UARTEx_ReceiveToIdle_DMA(&UART_PC, &brightness_tmp[0], 4);
  __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
  HAL_TIM_PWM_Start(&TIM_BL, TIM_CHANNEL_2);
  HAL_TIM_Base_Start_IT(&TIM_VBat);
  HAL_TIM_Base_Start_IT(&TIM_ALS);
  while (ADC[1] == 0);
  mVBat = 1550 * ADC[1] / ADC[0];
  lv_label_set_text_fmt(mVBat_label, "%dmV", mVBat);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    lv_task_handler();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void LVGL_DMA_pCallback(DMA_HandleTypeDef *_hdma)
{
    /* Inform LVGL that flushing is ready */
    lv_disp_flush_ready(disp_drv);
    return;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &TIM_VBat)
  {
    mVBat = 1550 * ADC[1] / ADC[0];
    lv_label_set_text_fmt(mVBat_label, "%dmV", mVBat);
  }
  else if (htim == &TIM_ALS)
  {
    ALS_Read(ALS_DATA_0, &als_data[0], 3);
  }
  return;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if (hi2c == &I2C_ALS)
  {
    uint16_t duty;
    uint32_t als_value = (als_data[2] << 16) | (als_data[1] << 8) | als_data[0];
    if (als_value >= 3200)
      duty = 5000 + (als_value - 3200) * (7200 - 5000) / (100000 - 3200);
    else
      duty = 80 + als_value * (5000 - 80) / 3200;
    duty = (duty + __HAL_TIM_GET_COMPARE(&TIM_BL, TIM_CHANNEL_2) * 63) / 64;
    duty = duty > 7200 ? 7200 : duty;
    __HAL_TIM_SET_COMPARE(&TIM_BL, TIM_CHANNEL_2, duty);
  }
  return;
}

void LVGL_UI_Init(void)
{
  static lv_style_t consolas_20_style;
  lv_style_init(&consolas_20_style);
  lv_style_set_text_font(&consolas_20_style, &consolas_20);
  static lv_style_t consolas_60_style;
  lv_style_init(&consolas_60_style);
  lv_style_set_text_font(&consolas_60_style, &consolas_60);
  mVBat_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(mVBat_label, &consolas_20_style, 0);
  lv_obj_align(mVBat_label, LV_ALIGN_TOP_RIGHT, -5, 3);
  satelliteCount_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(satelliteCount_label, &consolas_20_style, 0);
  lv_obj_align(satelliteCount_label, LV_ALIGN_TOP_LEFT, 5, 3);
  quality_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(quality_label, &consolas_20_style, 0);
  lv_obj_align_to(quality_label, satelliteCount_label, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
  locateStatus_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(locateStatus_label, &consolas_20_style, 0);
  lv_obj_align(locateStatus_label, LV_ALIGN_CENTER, 0, -80);
  speed_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(speed_label, &consolas_60_style, 0);
  lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 00);
  course_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(course_label, &consolas_20_style, 0);
  lv_obj_align(course_label, LV_ALIGN_CENTER, 0, -40);
  time_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(time_label, &consolas_20_style, 0);
  lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, 3);
  altitude_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(altitude_label, &consolas_20_style, 0);
  lv_obj_align(altitude_label, LV_ALIGN_CENTER, 0, 40);
  hdop_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(hdop_label, &consolas_20_style, 0);
  lv_obj_align(hdop_label, LV_ALIGN_CENTER, 0, 60);
  vdop_label = lv_label_create(lv_scr_act());
  lv_obj_add_style(vdop_label, &consolas_20_style, 0);
  lv_obj_align(vdop_label, LV_ALIGN_CENTER, 0, 80);
  return;
}

void LVGL_UI_Refresh(void)
{
  lv_label_set_text_fmt(locateStatus_label, "%c", NMEAData.status);
  lv_label_set_text_fmt(speed_label, "%d.%02d", NMEAData.speed_kmh.integer, NMEAData.speed_kmh.decimal);
  lv_label_set_text_fmt(course_label, "%d.%02d", NMEAData.course.integer, NMEAData.course.decimal);
  lv_label_set_text_fmt(time_label, "%02d:%02d:%02d", NMEAData.hour, NMEAData.minute, NMEAData.second);
  lv_label_set_text_fmt(quality_label, "%c", NMEAData.quality);
  lv_label_set_text_fmt(satelliteCount_label, "%d", NMEAData.satelliteCount);
  lv_label_set_text_fmt(altitude_label, "%d.%01d", NMEAData.altitude.integer, NMEAData.altitude.decimal);
  lv_label_set_text_fmt(hdop_label, "%d.%02d", NMEAData.hdop.integer, NMEAData.hdop.decimal);
  lv_label_set_text_fmt(vdop_label, "%d.%02d", NMEAData.vdop.integer, NMEAData.vdop.decimal);
  return;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
  if (huart == &UART_GNSS)
  {
    HAL_UART_Transmit_DMA(&huart2, &RxBuffer[0], size);
    NMEAHandler(&RxBuffer[0], size);
    LVGL_UI_Refresh();
    HAL_UARTEx_ReceiveToIdle_DMA(&UART_GNSS, &RxBuffer[0], RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
  }
  return;
}
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
