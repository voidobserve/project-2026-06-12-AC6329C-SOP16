#include "led_strip_white_anim.h"
#include "led_strip_white_schedule.h"
#include "WS2812FX.H"

/**
 * @brief 单色灯带渐变灭灯,做流星效果   兼容正反方向
 *          动画固定使用最大亮度值
 *
 * @return uint16_t
 */
uint16_t led_strip_white_anim_comet_1_whit_max_brightness(void)
{
	// 补丁：
	if (0 == _seg_rt->counter_mode_call)
	{
		// 刚进入，清除之前的数据残留
		Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
	}

	WS2812FX_fade_out_with_max_brightness();
	u8 offset;
	offset = 13;
	if (IS_REVERSE)
	{
		if ((_seg->stop - _seg->start) >= _seg_rt->counter_mode_step)
		{
			WS2812FX_setPixelColor_with_max_brightness(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[0]);
		}
	}
	else
	{
		if (_seg_rt->counter_mode_step < _seg->stop + 1)
		{
			WS2812FX_setPixelColor_with_max_brightness(_seg->start + _seg_rt->counter_mode_step, _seg->colors[0]);
		}
	}
  
	_seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % (_seg_len + offset);
	if (_seg_rt->counter_mode_step == 0)
	{
		SET_CYCLE;
		return led_strip_white.period_cnt;
	}

	return (_seg->speed);
}

/**
 * @brief 堆积流水   兼容正反方向
 *          动画固定使用最大亮度值
 * @return uint16_t
 */
uint16_t led_strip_white_anim_comet_4_with_max_brightness(void)
{
	// 补丁：
	if (0 == _seg_rt->counter_mode_call)
	{
		// 刚进入，清除之前的数据残留
		Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
	}

	if (IS_REVERSE)
	{

		if (_seg_rt->counter_mode_step < _seg_len)
		{
			WS2812FX_setPixelColor_with_max_brightness(_seg->stop - _seg_rt->counter_mode_step, WHITE);
		}
		else
		{

			WS2812FX_setPixelColor_with_max_brightness(2 * _seg_len - _seg_rt->counter_mode_step, BLACK);
		}
	}
	else
	{

		if (_seg_rt->counter_mode_step < _seg_len)
		{
			WS2812FX_setPixelColor_with_max_brightness(_seg->start + _seg_rt->counter_mode_step, WHITE);
		}
		else
		{

			WS2812FX_setPixelColor_with_max_brightness(_seg->start + _seg_rt->counter_mode_step - _seg_len, BLACK);
		}
	}

	_seg_rt->counter_mode_step++;
	_seg_rt->counter_mode_step %= _seg_len * 2;
	if (_seg_rt->counter_mode_step == 0)
	{
		SET_CYCLE;
		return led_strip_white.period_cnt; // 控制一轮动画之间的时间间隔
	}
	return (_seg->speed); // 返回计数器结果
}
