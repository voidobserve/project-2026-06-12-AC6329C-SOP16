#include "report.h"
#include "user_ble_notify.h"

/*
	USER_TO_DO 给app反馈数据的接口，可以优化成一下形式：
	可以额外参考 app_msg_typedef.c 和 app_msg_typedef.h 中的程序
*/
void report_msg(u8 msg_type, u8 msg_data)
{
	// 根据数据类型，查表
}

// 向app反馈声控类型：手机麦或者设备麦
void report_sound_control_type(u8 type)
{
	uint8_t buf[10];
	u8 len = 0;

	buf[len++] = 0x2F;
	buf[len++] = 0x06;
	buf[len++] = type;

	user_ble_notify_obj.param_put(buf, len);
}

void report_brightness(u8 brightness)
{
	uint8_t buf[10];
	u8 len = 0;

	buf[len++] = 0x04;
	buf[len++] = 0x03;
	buf[len++] = brightness;

	user_ble_notify_obj.param_put(buf, len);
}

void report_speed(u8 speed)
{
	uint8_t buf[10];
	u8 len = 0;

	buf[len++] = 0x04;
	buf[len++] = 0x04;
	buf[len++] = speed;

	user_ble_notify_obj.param_put(buf, len);
}

void report_meteor_period(u8 period)
{
	uint8_t buf[10];
	u8 len = 0;

	buf[len++] = 0x2F;
	buf[len++] = 0x03;
	buf[len++] = period;

	user_ble_notify_obj.param_put(buf, len);
}

void report_sound_control_sensitivity(u8 sensitivity)
{
	uint8_t buf[10];
	u8 len = 0;

	buf[len++] = 0x2F;
	buf[len++] = 0x05;
	buf[len++] = sensitivity;

	user_ble_notify_obj.param_put(buf, len);
}

// 反馈 RGB 灯带的长度(灯珠数量)
void report_led_strip_rgb_len(u16 nums)
{
	uint8_t buf[10]; 
	u8 len;

	buf[len++] = 0x04;
	buf[len++] = 0x08;
	buf[len++] = (nums >> 8) & 0xFF;
	buf[len++] = (nums >> 0) & 0xFF;

	user_ble_notify_obj.param_put(buf, len);
}
