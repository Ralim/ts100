/*
 * Setup.c
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Setup.h"
#include "BSP.h"
#include "Debug.h"
#include "Pins.h"
#include "gd32vf103.h"
#include <string.h>
#define ADC_NORM_CHANNELS 2
#define ADC_NORM_SAMPLES  32
uint16_t ADCReadings[ADC_NORM_SAMPLES * ADC_NORM_CHANNELS]; // room for 32 lots of the pair of readings

// Functions
void setup_gpio();
void setup_dma();
void setup_i2c();
void setup_adc();
void setup_timers();
void setup_iwdg();
void setup_uart();

void hardware_init() {
  // GPIO
  setup_gpio();
  // DMA
  setup_dma();
  // I2C
  setup_i2c();
  // ADC's
  setup_adc();
  // Timers
  setup_timers();
  // Watchdog
  setup_iwdg();
  // ELIC
  eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL0_PRIO4);
  // uart for debugging
  setup_uart();
  /* enable TIMER1 - PWM control timing*/
  timer_enable(TIMER1);
  timer_enable(TIMER2);
}
// channel 0 -> temperature sensor, 1-> VIN
uint16_t getADC(uint8_t channel) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < ADC_NORM_SAMPLES; i++)
    sum += ADCReadings[channel + (i * ADC_NORM_CHANNELS)];
  return sum >> 2;
}

void setup_uart() {
  // Setup the uart pins as a uart with dma

  /* enable USART clock */
  rcu_periph_clock_enable(RCU_USART1);

  /* connect port to USARTx_Tx */
  gpio_init(UART_TX_GPIO_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, UART_TX_Pin);

  /* connect port to USARTx_Rx */
  gpio_init(UART_RX_GPIO_Port, GPIO_MODE_IPU, GPIO_OSPEED_10MHZ, UART_RX_Pin);

  /* USART configure */
  usart_deinit(UART_PERIF);
  usart_baudrate_set(UART_PERIF, 2 * 1000 * 1000U);
  usart_word_length_set(UART_PERIF, USART_WL_8BIT);
  usart_stop_bit_set(UART_PERIF, USART_STB_1BIT);
  usart_parity_config(UART_PERIF, USART_PM_NONE);
  usart_hardware_flow_rts_config(UART_PERIF, USART_RTS_DISABLE);
  usart_hardware_flow_cts_config(UART_PERIF, USART_CTS_DISABLE);
  usart_receive_config(UART_PERIF, USART_RECEIVE_DISABLE); // Dont use rx for now
  usart_transmit_config(UART_PERIF, USART_TRANSMIT_ENABLE);
  eclic_irq_enable(USART1_IRQn, 15, 15);
  usart_enable(UART_PERIF);
}

void setup_gpio() {
  /* enable GPIOB clock */
  rcu_periph_clock_enable(RCU_GPIOA);
  /* enable GPIOB clock */
  rcu_periph_clock_enable(RCU_GPIOB);
  // Alternate function clock enable
  rcu_periph_clock_enable(RCU_AF);
  // Buttons as input
  gpio_init(KEY_A_GPIO_Port, GPIO_MODE_IPD, GPIO_OSPEED_2MHZ, KEY_A_Pin);
  gpio_init(KEY_B_GPIO_Port, GPIO_MODE_IPD, GPIO_OSPEED_2MHZ, KEY_B_Pin);
  // OLED reset as output
  gpio_init(OLED_RESET_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, OLED_RESET_Pin);
  gpio_bit_set(SDA_GPIO_Port, SDA_Pin);
  gpio_bit_set(SDA_GPIO_Port, SCL_Pin);
  // I2C as AF Open Drain
  gpio_init(SDA_GPIO_Port, GPIO_MODE_AF_OD, GPIO_OSPEED_2MHZ, SDA_Pin | SCL_Pin);
  // PWM output as AF Push Pull
  gpio_init(PWM_Out_GPIO_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, PWM_Out_Pin);
  // Analog Inputs ... as analog inputs
  gpio_init(TMP36_INPUT_GPIO_Port, GPIO_MODE_AIN, GPIO_OSPEED_2MHZ, TMP36_INPUT_Pin);
  gpio_init(TIP_TEMP_GPIO_Port, GPIO_MODE_AIN, GPIO_OSPEED_2MHZ, TIP_TEMP_Pin);
  gpio_init(VIN_GPIO_Port, GPIO_MODE_AIN, GPIO_OSPEED_2MHZ, VIN_Pin);

  // Remap PB4 away from JTAG NJRST
  gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);
  // FUSB interrupt
  gpio_init(FUSB302_IRQ_GPIO_Port, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, FUSB302_IRQ_Pin);
}
void setup_dma() {
  // Setup DMA for ADC0
  {
    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA data mode */
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(ADCReadings);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = ADC_NORM_SAMPLES * ADC_NORM_CHANNELS;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
  }
}
void setup_i2c() {
  /* enable I2C0 clock */
  rcu_periph_clock_enable(RCU_I2C0);
  // Setup I20 at 400kHz
  i2c_clock_config(I2C0, 400 * 1000, I2C_DTCY_2);
  i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x00);
  i2c_enable(I2C0);
  /* enable acknowledge */
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
  eclic_irq_enable(I2C0_EV_IRQn, 1, 0);
  eclic_irq_enable(I2C0_ER_IRQn, 2, 0);
}
void setup_adc() {

  // Setup ADC in normal + injected mode
  // Want it to sample handle temp and input voltage normally via dma
  // Then injected trigger to sample tip temp
  memset(ADCReadings, 0, sizeof(ADCReadings));
  rcu_periph_clock_enable(RCU_ADC0);
  rcu_periph_clock_enable(RCU_ADC1);
  adc_deinit(ADC0);
  adc_deinit(ADC1);
  /* config ADC clock */
  rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV16);
  // Run in normal parallel + inserted parallel
  adc_mode_config(ADC_DAUL_REGULAL_PARALLEL_INSERTED_PARALLEL);
  adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
  adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
  adc_special_function_config(ADC1, ADC_CONTINUOUS_MODE, ENABLE);
  adc_special_function_config(ADC1, ADC_SCAN_MODE, ENABLE);
  // Align right
  adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
  adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);
  // Setup reading 2 channels on regular mode (Handle Temp + dc in)
  adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, ADC_NORM_CHANNELS);
  adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, ADC_NORM_CHANNELS);
  // Setup the two channels
  adc_regular_channel_config(ADC0, 0, TMP36_ADC0_CHANNEL,
                             ADC_SAMPLETIME_71POINT5); // temp sensor
  adc_regular_channel_config(ADC1, 0, TMP36_ADC1_CHANNEL,
                             ADC_SAMPLETIME_71POINT5); // temp sensor
  adc_regular_channel_config(ADC0, 1, VIN_ADC0_CHANNEL,
                             ADC_SAMPLETIME_71POINT5); // DC Input voltage
  adc_regular_channel_config(ADC1, 1, VIN_ADC1_CHANNEL,
                             ADC_SAMPLETIME_71POINT5); // DC Input voltage
  // Setup that we want all 4 inserted readings to be the tip temp
  adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 4);
  adc_channel_length_config(ADC1, ADC_INSERTED_CHANNEL, 4);
  for (int rank = 0; rank < 4; rank++) {
    adc_inserted_channel_config(ADC0, rank, TIP_TEMP_ADC0_CHANNEL, ADC_SAMPLETIME_1POINT5);
    adc_inserted_channel_config(ADC1, rank, TIP_TEMP_ADC1_CHANNEL, ADC_SAMPLETIME_1POINT5);
  }
  // Setup timer 1 channel 0 to trigger injected measurements
  adc_external_trigger_source_config(ADC0, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_T1_CH0);
  adc_external_trigger_source_config(ADC1, ADC_INSERTED_CHANNEL, ADC0_1_EXTTRIG_INSERTED_T1_CH0);

  adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
  adc_external_trigger_source_config(ADC1, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
  // Enable triggers for the ADC
  adc_external_trigger_config(ADC0, ADC_INSERTED_CHANNEL, ENABLE);
  adc_external_trigger_config(ADC1, ADC_INSERTED_CHANNEL, ENABLE);
  adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
  adc_external_trigger_config(ADC1, ADC_REGULAR_CHANNEL, ENABLE);

  adc_watchdog_disable(ADC0);
  adc_watchdog_disable(ADC1);
  adc_resolution_config(ADC0, ADC_RESOLUTION_12B);
  adc_resolution_config(ADC1, ADC_RESOLUTION_12B);
  /* clear the ADC flag */
  adc_oversample_mode_disable(ADC0);
  adc_oversample_mode_disable(ADC1);
  adc_enable(ADC0);
  adc_calibration_enable(ADC0);
  adc_enable(ADC1);
  adc_calibration_enable(ADC1);
  adc_dma_mode_enable(ADC0);
  // Enable interrupt on end of injected readings
  adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOC);
  adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
  adc_interrupt_enable(ADC0, ADC_INT_EOIC);
  eclic_irq_enable(ADC0_1_IRQn, 2, 0);
  adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
  adc_software_trigger_enable(ADC1, ADC_REGULAR_CHANNEL);
  adc_tempsensor_vrefint_disable();
}
void setup_timers() {
  // Setup timer 1 to run the actual PWM level
  /* enable timer1 clock */
  rcu_periph_clock_enable(RCU_TIMER1);
  rcu_periph_clock_enable(RCU_TIMER2);
  timer_oc_parameter_struct timer_ocintpara;
  timer_parameter_struct    timer_initpara;
  {
    // deinit to reset the timer
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 5000;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = powerPWM + tempMeasureTicks;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV4;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* CH0 configured to implement the PWM irq's for the output control*/
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1, TIMER_CH_0, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, powerPWM + holdoffTicks);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_0, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    /* CH1 used for irq */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 0);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
    // IRQ
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_CH1);
  }

  eclic_irq_enable(TIMER1_IRQn, 2, 5);
  // Setup timer 2 to control the output signal
  {
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 200;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 100;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV4;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    /* CH0 configuration in PWM mode0 */
    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER2, TIMER_CH_0, &timer_ocintpara);
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 0);
    timer_channel_output_mode_config(TIMER2, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    timer_auto_reload_shadow_enable(TIMER2);
    timer_enable(TIMER2);
  }
}
void setup_iwdg() {

  fwdgt_config(0x0FFF, FWDGT_PSC_DIV256);
  fwdgt_enable();
}

void setupFUSBIRQ() {
  eclic_global_interrupt_enable();
  eclic_irq_enable(EXTI5_9_IRQn, 15, 0);
  gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_5);

  /* configure key EXTI line */
  exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
}
