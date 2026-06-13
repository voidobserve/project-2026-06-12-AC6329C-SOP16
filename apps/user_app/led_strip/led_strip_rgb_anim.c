#include "led_strip_rgb_anim.h"
#include "WS2812FX.H"
#include "led_strip_rgb_schedule.h"
#include "led_strip_driver.h"

// 单色流星
/**
 * @brief
 *
 * @return uint16_t
 */
uint16_t single_color_meteor(void)
{
	u8 offset;
	offset = 13;
	static uint8_t temp_color = 100;
	uint32_t r1, g1, b1, w1;

	uint8_t rate = 0;

	uint8_t size = 1 << SIZE_OPTION;
	uint8_t meteor_len = 2;
	if (size == 1)
	{
		meteor_len = 10;
	}
	else if (size == 2)
	{
		meteor_len = 5;
	}
	else if (size == 4)
	{
		meteor_len = 2;
	}
	else if (size == 8)
	{
		meteor_len = 1;
	}

	if (_seg_rt->counter_mode_step == 0) // 该判断放在这里，解决效果切换时，立即切换
	{
		temp_color = 100;
		SET_CYCLE;
		// fc_effect.mode_cycle = 1;
	}
	if ((get_effect_p() == 1) && (fc_effect.mode_cycle == 1)) // 计时中 && 完成一个循环
	{
		return (_seg->speed);
	}

	rate = temp_color;
	int w = (_seg->colors[_seg_rt->aux_param] >> 24) & 0xff;
	int r = (_seg->colors[_seg_rt->aux_param] >> 16) & 0xff;
	int g = (_seg->colors[_seg_rt->aux_param] >> 8) & 0xff;
	int b = _seg->colors[_seg_rt->aux_param] & 0xff;

	if (IS_REVERSE) // 反向
	{
		WS2812FX_copyPixels(_seg->start, _seg->start + 1, _seg_len - 1);
	}
	else
	{
		WS2812FX_copyPixels(_seg->start + 1, _seg->start, _seg_len - 1);
	}

	r1 = r * rate / 100;
	g1 = g * rate / 100;
	b1 = b * rate / 100;
	w1 = w * rate / 100;

	if (temp_color >= meteor_len)
		temp_color -= meteor_len;
	else
		temp_color = 0;
	if (IS_REVERSE) // 反向
	{
		WS2812FX_setPixelColor_rgbw(_seg->stop, r1, g1, b1, w1);
	}
	else
	{
		WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1);
	}

	_seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + offset);
	if (_seg_rt->counter_mode_step == 0)
	{
		SET_CYCLE;
		fc_effect.mode_cycle = 1;
		temp_color = 100;
	}

	return (_seg->speed);
}

// 多段颜色跳变
// void jump_mutil_c(void)
// {
//     uint8_t option;
//     // 正向
//     if(fc_effect.dream_scene.direction == IS_forward)
//     {
//         option = 0;
//     }
//     else{
//         option =  REVERSE;
//     }
//     WS2812FX_setSegment_colorOptions(
//         0,                                      //第0段
//         0,fc_effect.led_num-1,                  //起始位置，结束位置
//         &WS2812FX_mode_single_block_scan,       //效果
//         0,                                      //颜色，WS2812FX_setColors设置
//         fc_effect.dream_scene.speed,            //速度
//         option);                           //选项，这里像素点大小：3

//     WS2812FX_set_coloQty(0,fc_effect.dream_scene.c_n);
//     ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

//     WS2812FX_start();
// }

// 单个，全彩颜色的流星效果
void color_meteor(void)
{
	uint8_t option;
	if (fc_effect.dream_scene.direction == IS_forward)
	{
		option = 0 | SIZE_XLARGE;
	}
	else
	{
		option = REVERSE | SIZE_XLARGE;
	}

	WS2812FX_setSegment_colorOptions(
		LED_STRIP_RGB_STAR_INDEX,						  // 第0段
		LED_STRIP_RGB_STAR_INDEX,						  // 起始位置
		LED_STRIP_RGB_STAR_INDEX + LED_STRIP_RGB_LEN - 1, // 结束位置
		&single_color_meteor,							  // 效果
		0,												  // 颜色，WS2812FX_setColors设置
		fc_effect.dream_scene.speed,					  // 速度
		option);										  // 选项，这里像素点大小：3

	WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
	ls_set_colors(
		fc_effect.dream_scene.c_n,
		&fc_effect.dream_scene.rgb);
	// USER_TO_DO 需要清除所在段对应的颜色数据
	WS2812FX_running_flag_set();

	// WS2812FX_start();
}
