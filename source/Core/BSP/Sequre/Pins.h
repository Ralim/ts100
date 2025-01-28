/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_SEQURE_PINS_H_
#define BSP_SEQURE_PINS_H_
#include "configuration.h"

#ifdef MODEL_S60

#define KEY_B_Pin             GPIO_PIN_1
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_5
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_5
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_5
#define TIP_TEMP_Pin          GPIO_PIN_0
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_0
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_0
#define VIN_Pin               GPIO_PIN_4
#define VIN_GPIO_Port         GPIOA
#define VIN_ADC1_CHANNEL      ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL      ADC_CHANNEL_4
#define KEY_A_Pin             GPIO_PIN_0
#define KEY_A_GPIO_Port       GPIOB
#define PWM_Out_Pin           GPIO_PIN_8
#define PWM_Out_GPIO_Port     GPIOB
#define PWM_Out_CHANNEL       TIM_CHANNEL_3 // Timer 4; channel 3
#define SCL2_Pin              GPIO_PIN_6
#define SCL2_GPIO_Port        GPIOB
#define SDA2_Pin              GPIO_PIN_7
#define SDA2_GPIO_Port        GPIOB
// Pin gets pulled high on movement
#define MOVEMENT_Pin       GPIO_PIN_3
#define MOVEMENT_GPIO_Port GPIOA

#endif

#ifdef MODEL_S60P

#define KEY_B_Pin             GPIO_PIN_1
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_5
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_5
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_5
#define TIP_TEMP_Pin          GPIO_PIN_0
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_0
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_0
#define VIN_Pin               GPIO_PIN_4
#define VIN_GPIO_Port         GPIOA
#define VIN_ADC1_CHANNEL      ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL      ADC_CHANNEL_4
#define KEY_A_Pin             GPIO_PIN_0
#define KEY_A_GPIO_Port       GPIOB
#define PWM_Out_Pin           GPIO_PIN_8
#define PWM_Out_GPIO_Port     GPIOB
#define PWM_Out_CHANNEL       TIM_CHANNEL_3 // Timer 4; channel 3
#define SCL2_Pin              GPIO_PIN_6
#define SCL2_GPIO_Port        GPIOB
#define SDA2_Pin              GPIO_PIN_7
#define SDA2_GPIO_Port        GPIOB
#define SCL_Pin               GPIO_PIN_11
#define SCL_GPIO_Port         GPIOB
#define SDA_Pin               GPIO_PIN_10
#define SDA_GPIO_Port         GPIOB
// Pin gets pulled high on movement
#define MOVEMENT_Pin       GPIO_PIN_3
#define MOVEMENT_GPIO_Port GPIOA

#endif

#ifdef MODEL_S99

#define KEY_B_Pin             GPIO_PIN_1
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_5
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_5
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_5
#define TIP_TEMP_Pin          GPIO_PIN_0
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_0
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_0
#define VIN_Pin               GPIO_PIN_4
#define VIN_GPIO_Port         GPIOA
#define VIN_ADC1_CHANNEL      ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL      ADC_CHANNEL_4
#define KEY_A_Pin             GPIO_PIN_0
#define KEY_A_GPIO_Port       GPIOB
#define PWM_Out_Pin           GPIO_PIN_8
#define PWM_Out_GPIO_Port     GPIOB
#define PWM_Out_CHANNEL       TIM_CHANNEL_3 // Timer 4; channel 3
#define SCL2_Pin              GPIO_PIN_6
#define SCL2_GPIO_Port        GPIOB
#define SDA2_Pin              GPIO_PIN_7
#define SDA2_GPIO_Port        GPIOB
#define SCL_Pin               GPIO_PIN_11
#define SCL_GPIO_Port         GPIOB
#define SDA_Pin               GPIO_PIN_10
#define SDA_GPIO_Port         GPIOB
// Pin gets pulled high on movement
#define MOVEMENT_Pin       GPIO_PIN_3
#define MOVEMENT_GPIO_Port GPIOA

#endif // MODEL_S60P

#ifdef MODEL_T55

#define KEY_A_Pin       GPIO_PIN_1
#define KEY_A_GPIO_Port GPIOB
// No cold junction compensation as its a PT1000
#define TIP_TEMP_Pin          GPIO_PIN_5
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_5
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_5

#define VIN_Pin          GPIO_PIN_4
#define VIN_GPIO_Port    GPIOA
#define VIN_ADC1_CHANNEL ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL ADC_CHANNEL_4
#define KEY_B_Pin        GPIO_PIN_0
#define KEY_B_GPIO_Port  GPIOB

#define PWM_Out_Pin       GPIO_PIN_8
#define PWM_Out_GPIO_Port GPIOB
#define PWM_Out_CHANNEL   TIM_CHANNEL_3 // Timer 4; channel 3
#define SCL2_Pin          GPIO_PIN_6
#define SCL2_GPIO_Port    GPIOB
#define SDA2_Pin          GPIO_PIN_7
#define SDA2_GPIO_Port    GPIOB

#endif // MODEL_T55

#endif /* BSP_SEQURE_PINS_H_ */
