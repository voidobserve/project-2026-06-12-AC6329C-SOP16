#include "led_driver.h"
#include "ledc.h"
#include "gpio.h"

const ledc_platform_data_t ledc_rgb_data = {
	.index = 0,			  // 控制器号
	.port = LEDC_RGB_PIN, // 输出引脚
	.idle_level = 0,	  // 当前帧的空闲电平，0：低电平， 1：高电平
	.out_inv = 0,		  // 起始电平，0：高电平开始， 1：低电平开始
	.bit_inv = 1,		  // 取数据时高低位镜像，0：不镜像，1：8位镜像，2:16位镜像，3:32位镜像
	.t_unit = t_42ns,	  // 时间单位
	.t1h_cnt = 21,		  // 1码的高电平时间 = t1h_cnt * t_unit;21*42=882
	.t1l_cnt = 7,		  // 1码的低电平时间 = t1l_cnt * t_unit;7*42=294
	.t0h_cnt = 8,		  // 0码的高电平时间 = t0h_cnt * t_unit;8*42=336
	.t0l_cnt = 30,		  // 0码的低电平时间 = t0l_cnt * t_unit;*/30*42=1260
	.t_rest_cnt = 20000,  // 复位信号时间 = t_rest_cnt * t_unit;20000*42=840000
	.cbfun = NULL,		  // 中断回调函数
};

const ledc_platform_data_t ledc_white_data = {
	.index = 1,				// 控制器号
	.port = LEDC_WHITE_PIN, // 输出引脚
	.idle_level = 0,		// 当前帧的空闲电平，0：低电平， 1：高电平
	.out_inv = 0,			// 起始电平，0：高电平开始， 1：低电平开始
	.bit_inv = 1,			// 取数据时高低位镜像，0：不镜像，1：8位镜像，2:16位镜像，3:32位镜像
	.t_unit = t_42ns,		// 时间单位
	.t1h_cnt = 21,			// 1码的高电平时间 = t1h_cnt * t_unit;21*42=882
	.t1l_cnt = 7,			// 1码的低电平时间 = t1l_cnt * t_unit;7*42=294
	.t0h_cnt = 8,			// 0码的高电平时间 = t0h_cnt * t_unit;8*42=336
	.t0l_cnt = 30,			// 0码的低电平时间 = t0l_cnt * t_unit;*/30*42=1260
	.t_rest_cnt = 20000,	// 复位信号时间 = t_rest_cnt * t_unit;20000*42=840000
	.cbfun = NULL,			// 中断回调函数
};

void led_driver_init(void)
{
	ledc_init(&ledc_rgb_data);
	ledc_init(&ledc_white_data);
}