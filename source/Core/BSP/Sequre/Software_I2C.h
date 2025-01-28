/*
 * Software_I2C.h
 *
 *  Created on: 25 Jul 2020
 *      Author: Ralim
 */

#ifndef BSP_SEQURE_SOFTWARE_I2C_H_
#define BSP_SEQURE_SOFTWARE_I2C_H_
#include "BSP.h"
#include "configuration.h"
#include "stm32f1xx_hal.h"

#ifdef I2C_SOFT_BUS_1
#define SOFT_SCL1_HIGH() HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET)
#define SOFT_SCL1_LOW()  HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET)
#define SOFT_SDA1_HIGH() HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET)
#define SOFT_SDA1_LOW()  HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET)
#define SOFT_SDA1_READ() (HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin) == GPIO_PIN_SET ? 1 : 0)
#define SOFT_SCL1_READ() (HAL_GPIO_ReadPin(SCL_GPIO_Port, SCL_Pin) == GPIO_PIN_SET ? 1 : 0)

#endif /* I2C_SOFT_BUS_1 */

#ifdef I2C_SOFT_BUS_2
#define SOFT_SCL2_HIGH() HAL_GPIO_WritePin(SCL2_GPIO_Port, SCL2_Pin, GPIO_PIN_SET)
#define SOFT_SCL2_LOW()  HAL_GPIO_WritePin(SCL2_GPIO_Port, SCL2_Pin, GPIO_PIN_RESET)
#define SOFT_SDA2_HIGH() HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_SET)
#define SOFT_SDA2_LOW()  HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_RESET)
#define SOFT_SDA2_READ() (HAL_GPIO_ReadPin(SDA2_GPIO_Port, SDA2_Pin) == GPIO_PIN_SET ? 1 : 0)
#define SOFT_SCL2_READ() (HAL_GPIO_ReadPin(SCL2_GPIO_Port, SCL2_Pin) == GPIO_PIN_SET ? 1 : 0)

#endif /* I2C_SOFT_BUS_2 */

// clang-format off
#define SOFT_I2C_DELAY()              \
  {                                   \
    for (int xx = 0; xx < 12; xx++) { \
      asm("nop");                     \
    }                                 \
  }
// clang-format on

// 40 ~= 100kHz; 15 gives around 250kHz or so which is fast _and_ stable

#endif /* BSP_SEQURE_SOFTWARE_I2C_H_ */
