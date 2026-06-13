#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

#include "typedef.h"

#define LEDC_RGB_PIN IO_PORTA_07
#define LEDC_WHITE_PIN IO_PORTA_02

void led_driver_init(void);

#endif