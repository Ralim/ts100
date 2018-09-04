/*
 * LIS2DH12.hpp
 *
 *  Created on: 27Feb.,2018
 *      Author: Ralim
 */

#ifndef LIS2DH12_HPP_
#define LIS2DH12_HPP_
#include "stm32f1xx_hal.h"
#include "FRToSI2C.hpp"
#include "LIS2DH12_defines.hpp"
#include "hardware.h"

class LIS2DH12 {
public:
	static void initalize();
	//1 = rh, 2,=lh, 8=flat
	static Orientation getOrientation() {
		uint8_t val = (FRToSI2C::I2C_RegisterRead(LIS2DH_I2C_ADDRESS,LIS_INT2_SRC) >> 2);
		if(val==8)
			val=3;
		else
			val--;
		return static_cast<Orientation>( val); }
	static void getAxisReadings(int16_t *x, int16_t *y, int16_t *z);

private:
};

#endif /* LIS2DH12_HPP_ */
