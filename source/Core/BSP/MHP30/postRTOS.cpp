#include "BSP.h"
#include "FreeRTOS.h"
#include "I2C_Wrapper.hpp"
#include "QC3.h"
#include "Settings.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "power.hpp"
#include "stdlib.h"
#include "task.h"

// Initialisation to be performed with scheduler active
void postRToSInit() {}
