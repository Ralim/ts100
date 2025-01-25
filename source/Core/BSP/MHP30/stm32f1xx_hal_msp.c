#include "Pins.h"
#include "Setup.h"
#include "stm32f1xx_hal.h"
/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) {
  __HAL_RCC_AFIO_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc) {

  GPIO_InitTypeDef GPIO_InitStruct;
  if (hadc->Instance == ADC1) {
    __HAL_RCC_ADC1_CLK_ENABLE();

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance                 = DMA1_Channel1;
    hdma_adc1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode                = DMA_CIRCULAR;
    hdma_adc1.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_adc1);

    __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc1);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  } else {
    __HAL_RCC_ADC2_CLK_ENABLE();

    /**ADC2 GPIO Configuration
     PB0     ------> ADC2_IN8
     PB1     ------> ADC2_IN9
     */
    GPIO_InitStruct.Pin  = TIP_TEMP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(TIP_TEMP_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin  = TMP36_INPUT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(TMP36_INPUT_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin  = VIN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(VIN_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin  = PLATE_SENSOR_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(PLATE_SENSOR_GPIO_Port, &GPIO_InitStruct);

    /* ADC2 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base) {

  if (htim_base->Instance == TIM3) {
    __HAL_RCC_TIM3_CLK_ENABLE();
  } else if (htim_base->Instance == TIM2) {
    __HAL_RCC_TIM2_CLK_ENABLE();
  }
}
