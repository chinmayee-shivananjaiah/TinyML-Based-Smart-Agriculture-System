
#include "main.h"
#include "soil_model.h"

#include <stdio.h>
#include <string.h>


ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart2;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);


uint32_t moisture_adc = 0;
uint32_t ph_adc = 0;

float moisture_percent = 0.0f;


float ph_test = 7.0f;


float rainfall_test = 50.0f;
float humidity_test = 60.0f;

int prediction = -1;


int moisture_display = 0;

char recommendation[50];
char msg[150];

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  
  HAL_Init();

  
  SystemClock_Config();

  
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();

  
  while (1)
  {
    
    HAL_ADC_Start(&hadc1);

   
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    moisture_adc = HAL_ADC_GetValue(&hadc1);

    
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    ph_adc = HAL_ADC_GetValue(&hadc1);

    
    HAL_ADC_Stop(&hadc1);

    
    moisture_percent =
        ((4093.0f - moisture_adc) /
         (4093.0f - 1700.0f)) * 100.0f;

    
    if (moisture_percent < 0.0f)
    {
      moisture_percent = 0.0f;
    }

    if (moisture_percent > 100.0f)
    {
      moisture_percent = 100.0f;
    }

    
    moisture_display = (int)(moisture_percent * 10.0f);

    
    ph_test = 7.0f;

    
    rainfall_test = 50.0f;
    humidity_test = 60.0f;

    
    prediction = soil_predict(
        moisture_percent,
        ph_test,
        rainfall_test,
        humidity_test
    );

    
    sprintf(
        msg,
        "Moisture ADC: %lu | Moisture: %d.%d%% | "
        "pH ADC: %lu | pH: 7.0\r\n",

        (unsigned long)moisture_adc,

        moisture_display / 10,
        moisture_display % 10,

        (unsigned long)ph_adc
    );

    HAL_UART_Transmit(
        &huart2,
        (uint8_t *)msg,
        strlen(msg),
        HAL_MAX_DELAY
    );

    /*----------------------------------------------------------------------
     * 10. Soil classification
     *----------------------------------------------------------------------*/
    if (prediction == 0)
    {
      sprintf(msg, "TinyML Result: POOR\r\n");
    }
    else if (prediction == 1)
    {
      sprintf(msg, "TinyML Result: MODERATE\r\n");
    }
    else if (prediction == 2)
    {
      sprintf(msg, "TinyML Result: SUITABLE\r\n");
    }
    else
    {
      sprintf(msg, "TinyML Error\r\n");
    }

    HAL_UART_Transmit(
        &huart2,
        (uint8_t *)msg,
        strlen(msg),
        HAL_MAX_DELAY
    );

    /*----------------------------------------------------------------------
     * 11. Recommendation layer
     *----------------------------------------------------------------------*/
    if (prediction == 0)
    {
      strcpy(
          recommendation,
          "Soil improvement required"
      );
    }
    else if (prediction == 1)
    {
      strcpy(
          recommendation,
          "Maize / Millets"
      );
    }
    else if (prediction == 2)
    {
      strcpy(
          recommendation,
          "Rice / Wheat / Maize"
      );
    }
    else
    {
      strcpy(
          recommendation,
          "No recommendation"
      );
    }

    /*----------------------------------------------------------------------
     * 12. Display recommendation
     *----------------------------------------------------------------------*/
    sprintf(
        msg,
        "Recommendation: %s\r\n",
        recommendation
    );

    HAL_UART_Transmit(
        &huart2,
        (uint8_t *)msg,
        strlen(msg),
        HAL_MAX_DELAY
    );

    
    HAL_Delay(2000);
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  
  __HAL_RCC_PWR_CLK_ENABLE();

  
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK |
      RCC_CLOCKTYPE_SYSCLK |
      RCC_CLOCKTYPE_PCLK1 |
      RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(
          &RCC_ClkInitStruct,
          FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /* ADC peripheral clock */
  __HAL_RCC_ADC1_CLK_ENABLE();

  /* ADC configuration */
  hadc1.Instance = ADC1;

  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /*----------------------------------------------------------------------
   * Rank 1: PA0 / ADC1_IN0 → Soil Moisture
   *----------------------------------------------------------------------*/
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;

  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /*----------------------------------------------------------------------
   * Rank 2: PA1 / ADC1_IN1 → pH
   *----------------------------------------------------------------------*/
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;

  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  /* UART clock */
  __HAL_RCC_USART2_CLK_ENABLE();

  huart2.Instance = USART2;

  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable GPIO clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*----------------------------------------------------------------------
   * PA0 → ADC1_IN0 → Moisture
   * PA1 → ADC1_IN1 → pH
   *----------------------------------------------------------------------*/
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*----------------------------------------------------------------------
   * PA2 → USART2_TX
   * PA3 → USART2_RX
   *----------------------------------------------------------------------*/
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();

  while (1)
  {
    /* Stay here */
  }
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
  /* User can add implementation here */
}

#endif /* USE_FULL_ASSERT */