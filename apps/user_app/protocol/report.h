#ifndef __REPORT_H__
#define __REPORT_H__

#include "typedef.h"

void report_sound_control_type(u8 type);
void report_brightness(u8 brightness);
void report_speed(u8 speed);
void report_meteor_period(u8 period);
void report_sound_control_sensitivity(u8 sensitivity);
void report_led_strip_rgb_len(u16 nums);

#endif