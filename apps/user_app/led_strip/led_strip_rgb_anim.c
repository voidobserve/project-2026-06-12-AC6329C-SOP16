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
uint16_t led_strip_rgb_anim_single_color_meteor(void)
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

u16 led_strip_rgb_anim_mutil_fade(void)
{
	uint8_t size = fc_effect.dream_scene.seg_size;
	uint16_t j;
	uint8_t cnt0 = 0, cnt1 = 1;
	uint32_t color, color1, color0;
	static uint32_t c1[MAX_NUM_COLORS];
	int lum = _seg_rt->counter_mode_step;

	if (size > (_seg->stop - _seg->start) && size == 0)
		return 0;

	if (lum > 255)
		lum = 511 - lum; // lum = 0 -> 255 -> 0
	_seg_rt->aux_param = 0;
	_seg_rt->aux_param2 = 0;

	if (_seg_rt->aux_param3 == 0)
	{
		_seg_rt->aux_param3 = 1;
		memcpy(c1, _seg->colors, MAX_NUM_COLORS * 4);
	}

	while (_seg_rt->aux_param2 < _seg->stop)
	{
		color0 = _seg->colors[cnt0];
		cnt0++;
		cnt0 %= _seg->c_n;
		color1 = c1[cnt1];
		cnt1++;
		cnt1 %= _seg->c_n;
		color = WS2812FX_color_blend(color1, color0, lum);
		for (j = 0; j < size; j++)
		{
			WS2812FX_setPixelColor(_seg->start + _seg_rt->aux_param2,
								   color);
			_seg_rt->aux_param2++;
			if (_seg_rt->aux_param2 > _seg->stop)
			{
				break;
			}
		}
	}

	_seg_rt->counter_mode_step += 4;

	// 此时颜色停留在color1
	if (_seg_rt->counter_mode_step > 511)
	{
		_seg_rt->counter_mode_step = 0;
		// color0的颜色池左移1

		SET_CYCLE;
	}

	// 此时颜色停留在color1,把color0颜色变换,color0向左转盘
	if (_seg_rt->counter_mode_step == 0)
	{
		uint32_t c_tmp;
		c_tmp = _seg->colors[0];

		memmove(_seg->colors, _seg->colors + 1, (_seg->c_n - 1) * 4);

		_seg->colors[_seg->c_n - 1] = c_tmp;
	}

	// 此时颜色停留再color0
	if (_seg_rt->counter_mode_step == 256)
	{
		// color1的颜色池左移1
		uint32_t c_tmp;
		c_tmp = c1[0];
		memmove(&c1[0], &c1[1], (_seg->c_n - 1) * 4);

		c1[_seg->c_n - 1] = c_tmp;
	}
	return (_seg->speed / 32);
}

// 功能：颜色块跳变效果，多个颜色块组成背景,以块为单位步进做流水,
// _seg->c_n:有效颜色数量
// SIZE_OPTION：决定颜色块大小
// IS_REVERSE:0 反向流水 ；1正向流水，WS2812FX_setOptions(REVERSE)来设置
u16 led_strip_rgb_anim_single_block_scan(void)
{
	uint8_t size = fc_effect.dream_scene.seg_size;
	uint8_t j;
	uint32_t c;
	// printf("\n b=%d",Adafruit_NeoPixel_getBrightness());
	if (size > (_seg->stop - _seg->start))
		return 0;

	_seg_rt->counter_mode_step = 0;
	_seg_rt->aux_param = 0;
	while (_seg_rt->counter_mode_step < _seg->stop)
	{
		for (j = 0; j < size; j++)
		{
			if (IS_REVERSE)
			{
				WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
			}
			else
			{
				WS2812FX_setPixelColor(_seg->stop - _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
			}
			_seg_rt->counter_mode_step++;
			if (_seg_rt->counter_mode_step > _seg->stop)
			{
				break;
			}
		}
		_seg_rt->aux_param++;
		_seg_rt->aux_param %= _seg->c_n;
	}

	c = _seg->colors[0];
	// 重新开始，对颜色转盘
	for (j = 1; j < _seg->c_n; j++)
	{
		// 把后面的颜色提前
		_seg->colors[j - 1] = _seg->colors[j];
	}
	_seg->colors[j - 1] = c;

	return _seg->speed * 4;
}

uint16_t led_strip_rgb_anim_mutil_breath(void)
{
	uint8_t size = fc_effect.dream_scene.seg_size;
	uint8_t j;
	uint16_t lum = _seg_rt->aux_param3;
	uint32_t color;

	if (lum > 255)
	{
		lum = 511 - lum;
	}
	if (size > (_seg->stop - _seg->start))
		return 0;
	_seg_rt->counter_mode_step = 0;
	_seg_rt->aux_param = 0;
	while (_seg_rt->counter_mode_step <= _seg->stop)
	{
		for (j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
		{
			color = WS2812FX_color_blend(0, _seg->colors[_seg_rt->aux_param], lum);
			WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, color);
			_seg_rt->counter_mode_step++;
		}
		_seg_rt->aux_param++;
		_seg_rt->aux_param %= _seg->c_n;
	}

	_seg_rt->aux_param3 += 2;
	if (_seg_rt->aux_param3 > (512 - 5))
	{
		_seg_rt->aux_param3 = 5;
	}

	return _seg->speed;
}

uint16_t led_strip_rgb_anim_mutil_twihkle(void)
{
	uint8_t size = (SIZE_OPTION << 1) + 1;
	uint8_t j;
	if (size > (_seg->stop - _seg->start))
		return 0;

	_seg_rt->counter_mode_step = 0;
	_seg_rt->aux_param = 0;
	if (_seg_rt->aux_param3)
	{
		while (_seg_rt->counter_mode_step <= _seg->stop)
		{
			for (j = 0; (j < size) && (_seg_rt->counter_mode_step <= _seg->stop); j++)
			{
				WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
				_seg_rt->counter_mode_step++;
			}
			_seg_rt->aux_param++;
			_seg_rt->aux_param %= _seg->c_n;
		}
	}
	else
	{
		Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
	}

	_seg_rt->aux_param3 = !_seg_rt->aux_param3;

	return _seg->speed;
}

uint16_t led_strip_rgb_anim_multi_block_scan(void)
{
	uint8_t size = (SIZE_OPTION << 1) + 1;
	uint8_t j;
	uint16_t i;
	if (size > (_seg->stop - _seg->start))
		return 0;

	if (IS_REVERSE) // 反向流水
	{
		WS2812FX_copyPixels(_seg->start, _seg->start + 1, (_seg_len - 1));
		WS2812FX_setPixelColor(_seg->stop, _seg->colors[_seg_rt->aux_param]);
	}
	else
	{
		WS2812FX_copyPixels(_seg->start + 1, _seg->start, (_seg_len - 1));
		WS2812FX_setPixelColor(_seg->start, _seg->colors[_seg_rt->aux_param]);
	}

	_seg_rt->counter_mode_step++;

	_seg_rt->aux_param3++;
	if (_seg_rt->aux_param3 >= size)
	{
		_seg_rt->aux_param3 = 0;
		_seg_rt->aux_param++;
		_seg_rt->aux_param %= _seg->c_n;
	}

	_seg_rt->counter_mode_step %= _seg_len;

	return _seg->speed;
}

u16 led_strip_rgb_anim_multi_colors_gradual(void)
{
	static uint8_t index;
	uint32_t rgb;
	static uint32_t c0, c1;
	int lum = _seg_rt->counter_mode_step;
	if (lum > 255)
		lum = 511 - lum; // lum = 0 -> 255 -> 0
	if (_seg_rt->aux_param == 0)
	{
		_seg_rt->aux_param = 1;
		index = 0;
		c1 = _seg->colors[index];
		index++;
		c0 = _seg->colors[index];
	}
	// _seg->colors[1]:目标颜色
	uint32_t color = WS2812FX_color_blend(c1, c0, lum);

	Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

	if (_seg_rt->counter_mode_step == 256)
	{
		index++;
		index %= _seg->c_n;
		c1 = _seg->colors[index];
	}

	_seg_rt->counter_mode_step++;
	if (_seg_rt->counter_mode_step > 511)
	{
		_seg_rt->counter_mode_step = 0;
		index++;
		index %= _seg->c_n;
		c0 = _seg->colors[index];
		SET_CYCLE;
	}

	return (_seg->speed / 5);
}

u16 led_strip_rgb_anim_starry_sky(void)
{
	const static uint8_t led[266][16] = {
		{0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 0},
		{2, 0, 0, 2, 50, 0, 0, 0, 0, 0, 0, 2, 0, 50, 0, 2},
		{4, 0, 0, 4, 45, 0, 0, 0, 0, 0, 0, 4, 0, 45, 0, 4},
		{6, 0, 0, 6, 40, 0, 0, 0, 0, 0, 0, 6, 0, 40, 0, 6},
		{8, 0, 0, 8, 35, 0, 0, 0, 0, 0, 0, 8, 0, 35, 0, 8},
		{10, 0, 0, 10, 30, 0, 0, 0, 0, 0, 0, 10, 0, 30, 0, 10},
		{12, 0, 0, 12, 25, 0, 0, 0, 0, 0, 0, 12, 0, 25, 0, 12},
		{14, 0, 0, 14, 20, 0, 0, 0, 0, 0, 0, 14, 0, 20, 0, 14},
		{16, 0, 0, 16, 15, 0, 0, 0, 0, 0, 0, 16, 0, 15, 0, 16},
		{18, 0, 0, 18, 10, 0, 0, 0, 0, 0, 0, 18, 0, 10, 0, 18},
		{20, 0, 0, 20, 5, 0, 0, 0, 0, 0, 0, 20, 0, 5, 0, 20},
		{22, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 22},
		{24, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 24},
		{26, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 26},
		{28, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 28},
		{30, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 30},
		{32, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 32},
		{34, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 34},
		{36, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 36, 0, 0, 0, 36},
		{38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 38},
		{40, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 40, 0, 0, 0, 40},
		{42, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 42, 0, 0, 0, 42},
		{44, 0, 0, 44, 0, 0, 0, 0, 0, 0, 0, 44, 0, 0, 0, 44},
		{46, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 46, 0, 0, 0, 46},
		{48, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 48},
		{50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 50, 0, 0, 0, 50},
		{52, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 52, 0, 0, 0, 52},
		{54, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 54},
		{56, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 56},
		{58, 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 58, 0, 0, 0, 58},
		{60, 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 60, 0, 0, 0, 60},
		{62, 0, 0, 62, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 62},
		{64, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 64},
		{66, 2, 0, 66, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 66},
		{68, 4, 0, 68, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 68},
		{70, 6, 0, 70, 0, 0, 0, 0, 0, 0, 0, 70, 0, 0, 0, 70},
		{72, 8, 0, 72, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 72},
		{74, 10, 0, 74, 0, 0, 0, 0, 0, 0, 0, 74, 0, 0, 0, 74},
		{76, 12, 0, 76, 0, 0, 0, 0, 0, 0, 0, 76, 0, 0, 0, 76},
		{78, 14, 0, 78, 0, 0, 0, 0, 0, 0, 0, 78, 0, 0, 0, 78},
		{80, 16, 0, 80, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 80},
		{82, 18, 0, 82, 0, 0, 0, 0, 0, 0, 0, 82, 0, 0, 0, 82},
		{84, 20, 0, 84, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 84},
		{86, 22, 0, 86, 0, 0, 0, 0, 0, 0, 0, 86, 0, 0, 0, 86},
		{88, 24, 0, 88, 0, 0, 0, 0, 0, 0, 0, 88, 0, 0, 0, 88},
		{90, 26, 0, 90, 0, 0, 0, 0, 0, 0, 0, 90, 0, 0, 0, 90},
		{92, 28, 0, 92, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 92},
		{94, 30, 0, 94, 0, 0, 0, 0, 0, 0, 0, 94, 0, 0, 0, 94},
		{96, 32, 0, 96, 0, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 96},
		{98, 34, 0, 98, 0, 0, 0, 0, 0, 0, 0, 98, 0, 0, 0, 98},
		{100, 36, 0, 100, 0, 0, 2, 0, 0, 0, 0, 100, 0, 0, 0, 100},
		{100, 38, 0, 100, 0, 0, 4, 0, 0, 0, 0, 100, 0, 0, 0, 98},
		{100, 40, 0, 100, 0, 0, 6, 0, 0, 0, 0, 100, 0, 0, 0, 96},
		{100, 42, 0, 100, 0, 0, 8, 0, 0, 0, 0, 100, 0, 0, 0, 94},
		{100, 44, 0, 100, 0, 0, 10, 0, 0, 0, 0, 100, 0, 0, 0, 92},
		{100, 46, 0, 100, 0, 0, 12, 0, 0, 0, 0, 100, 0, 0, 0, 90},
		{100, 48, 0, 100, 0, 0, 14, 0, 0, 0, 0, 100, 0, 0, 0, 88},
		{100, 50, 0, 100, 0, 0, 16, 0, 0, 0, 0, 100, 0, 0, 0, 86},
		{100, 52, 0, 100, 0, 0, 18, 0, 0, 0, 0, 100, 0, 0, 0, 84},
		{100, 54, 0, 100, 0, 0, 20, 0, 0, 0, 0, 100, 0, 0, 0, 82},
		{100, 56, 0, 100, 0, 0, 22, 0, 0, 0, 0, 100, 0, 0, 0, 80},
		{100, 58, 0, 100, 0, 0, 24, 0, 0, 0, 0, 100, 0, 0, 0, 78},
		{100, 60, 0, 98, 0, 0, 26, 0, 0, 0, 0, 98, 0, 0, 0, 76},
		{98, 62, 0, 96, 0, 0, 28, 0, 0, 0, 0, 96, 0, 0, 0, 74},
		{96, 64, 0, 94, 0, 0, 30, 0, 0, 0, 0, 94, 0, 0, 0, 72},
		{94, 66, 0, 92, 0, 0, 32, 0, 0, 0, 0, 92, 0, 0, 0, 70},
		{92, 68, 0, 90, 0, 0, 34, 0, 0, 0, 0, 90, 0, 0, 0, 68},
		{90, 70, 0, 88, 0, 0, 36, 0, 0, 0, 0, 88, 0, 0, 0, 66},
		{88, 72, 0, 86, 0, 0, 38, 0, 0, 0, 0, 86, 0, 0, 0, 64},
		{86, 74, 0, 84, 0, 0, 40, 0, 0, 0, 0, 84, 0, 0, 0, 62},
		{84, 76, 0, 82, 0, 0, 42, 0, 0, 0, 0, 82, 0, 0, 0, 60},
		{82, 78, 0, 80, 0, 0, 44, 0, 0, 0, 0, 80, 0, 0, 0, 58},
		{80, 80, 0, 78, 0, 0, 46, 0, 0, 0, 0, 78, 0, 0, 0, 56},
		{78, 82, 0, 76, 0, 0, 48, 0, 0, 0, 0, 76, 0, 0, 0, 54},
		{76, 84, 0, 74, 0, 0, 50, 0, 0, 0, 0, 74, 0, 0, 0, 52},
		{74, 86, 0, 72, 0, 0, 52, 0, 0, 0, 0, 72, 0, 0, 0, 50},
		{72, 88, 0, 70, 0, 0, 54, 0, 0, 70, 0, 0, 0, 48},
		{70, 90, 0, 68, 0, 0, 56, 0, 0, 68, 0, 0, 0, 46},
		{68, 92, 0, 66, 0, 0, 58, 0, 0, 66, 0, 0, 0, 44},
		{66, 94, 0, 64, 0, 0, 60, 0, 0, 64, 0, 0, 0, 42},
		{64, 96, 0, 62, 0, 0, 62, 0, 0, 62, 0, 0, 0, 40},
		{62, 98, 0, 60, 0, 0, 64, 0, 0, 60, 0, 0, 0, 38},
		{60, 100, 2, 58, 0, 0, 66, 0, 0, 58, 0, 0, 0, 36},
		{58, 100, 4, 56, 0, 0, 68, 0, 0, 56, 0, 2, 0, 34},
		{56, 100, 6, 54, 0, 0, 70, 0, 0, 54, 0, 4, 0, 32},
		{54, 100, 8, 52, 0, 0, 72, 0, 0, 52, 0, 6, 0, 30},
		{52, 100, 10, 50, 0, 0, 74, 0, 0, 50, 0, 8, 0, 28},
		{50, 100, 12, 48, 0, 0, 76, 0, 0, 48, 0, 10, 0, 26},
		{48, 100, 14, 46, 0, 0, 78, 0, 0, 46, 0, 12, 0, 24},
		{46, 100, 16, 44, 2, 0, 80, 0, 0, 44, 0, 14, 0, 22},
		{44, 100, 18, 42, 4, 0, 82, 0, 0, 42, 0, 16, 0, 20},
		{42, 100, 20, 40, 6, 0, 84, 0, 0, 40, 0, 18, 0, 18},
		{40, 100, 22, 38, 8, 0, 86, 0, 0, 38, 0, 20, 0, 16},
		{38, 100, 24, 36, 10, 0, 88, 0, 0, 36, 0, 22, 0, 14},
		{36, 98, 26, 34, 12, 0, 90, 0, 0, 34, 0, 24, 0, 12},
		{34, 96, 28, 32, 14, 0, 92, 0, 0, 32, 0, 26, 0, 10},
		{32, 94, 30, 30, 16, 0, 94, 0, 0, 30, 0, 28, 0, 8},
		{30, 92, 32, 28, 18, 0, 96, 0, 0, 28, 0, 30, 0, 6},
		{28, 90, 34, 26, 20, 0, 98, 0, 0, 26, 0, 32, 0, 4},
		{26, 88, 36, 24, 22, 0, 100, 0, 0, 24, 0, 34, 0, 2},
		{24, 86, 38, 22, 24, 0, 100, 0, 0, 22, 0, 36, 0, 0},
		{22, 84, 40, 20, 26, 0, 100, 0, 0, 20, 0, 38, 0, 0},
		{20, 82, 42, 18, 28, 0, 100, 0, 0, 18, 0, 40, 0, 0},
		{18, 80, 44, 16, 30, 0, 100, 0, 0, 16, 0, 42, 0, 0},
		{16, 78, 46, 14, 32, 0, 100, 0, 0, 14, 0, 44, 0, 0},
		{14, 76, 48, 12, 34, 0, 100, 0, 0, 12, 0, 46, 0, 0},
		{12, 74, 50, 10, 36, 0, 100, 0, 0, 10, 0, 48, 0, 0},
		{10, 72, 52, 8, 38, 0, 100, 0, 0, 8, 0, 50, 0, 0},
		{8, 70, 54, 6, 40, 0, 100, 0, 0, 6, 0, 52, 0, 0},
		{6, 68, 56, 4, 42, 0, 100, 0, 0, 4, 0, 54, 0, 0},
		{4, 66, 58, 2, 44, 0, 100, 0, 0, 2, 0, 56, 0, 0},
		{2, 64, 60, 0, 46, 0, 98, 0, 0, 0, 0, 58, 0, 0},
		{0, 62, 62, 0, 48, 2, 96, 0, 0, 0, 0, 60, 0, 0},
		{0, 60, 64, 0, 50, 4, 94, 0, 0, 0, 2, 62, 2, 0},
		{0, 58, 66, 0, 52, 6, 92, 0, 0, 0, 4, 64, 4, 0},
		{0, 56, 68, 0, 54, 8, 90, 0, 0, 0, 6, 66, 6, 0},
		{0, 54, 70, 0, 56, 10, 88, 0, 0, 0, 8, 68, 8, 0},
		{0, 52, 72, 0, 58, 12, 86, 0, 0, 0, 10, 70, 10, 0},
		{0, 50, 74, 0, 60, 14, 84, 0, 0, 0, 12, 72, 12, 0},
		{0, 48, 76, 0, 62, 16, 82, 0, 0, 0, 14, 74, 14, 0},
		{0, 46, 78, 0, 64, 18, 80, 0, 0, 0, 16, 76, 16, 0},
		{0, 44, 80, 0, 66, 20, 78, 0, 0, 0, 18, 78, 18, 0},
		{0, 42, 82, 0, 68, 22, 76, 0, 0, 0, 20, 80, 20, 0},
		{0, 40, 84, 0, 70, 24, 74, 0, 0, 0, 22, 82, 22, 0},
		{0, 38, 86, 0, 72, 26, 72, 2, 0, 0, 0, 0, 24, 84, 24, 0},
		{0, 36, 88, 0, 74, 28, 70, 4, 0, 0, 0, 0, 26, 86, 26, 0},
		{0, 34, 90, 0, 76, 30, 68, 6, 0, 0, 0, 0, 28, 88, 28, 0},
		{0, 32, 92, 0, 78, 32, 66, 8, 0, 0, 0, 0, 30, 90, 30, 0},
		{0, 30, 94, 0, 80, 34, 64, 10, 0, 0, 0, 0, 32, 92, 32, 0},
		{0, 28, 96, 0, 82, 36, 62, 12, 0, 0, 0, 0, 34, 94, 34, 0},
		{0, 26, 98, 0, 84, 38, 60, 14, 0, 0, 0, 0, 36, 96, 36, 0},
		{0, 24, 100, 0, 86, 40, 58, 16, 2, 0, 0, 0, 38, 98, 38, 0},
		{0, 22, 100, 0, 88, 42, 56, 18, 4, 0, 0, 0, 40, 100, 40, 0},
		{0, 20, 100, 0, 90, 44, 54, 20, 6, 0, 0, 0, 42, 100, 42, 0},
		{0, 18, 100, 0, 92, 46, 52, 22, 8, 0, 0, 0, 44, 100, 44, 0},
		{0, 16, 100, 0, 94, 48, 50, 24, 10, 0, 0, 0, 46, 100, 46, 0},
		{0, 14, 100, 0, 96, 50, 48, 26, 12, 0, 0, 0, 48, 100, 48, 0},
		{0, 12, 100, 0, 98, 52, 46, 28, 14, 0, 0, 0, 50, 100, 50, 0},
		{0, 10, 100, 0, 100, 54, 44, 30, 16, 0, 0, 0, 52, 100, 52, 0},
		{0, 8, 100, 0, 100, 56, 42, 32, 18, 0, 0, 0, 54, 100, 54, 0},
		{0, 6, 100, 0, 100, 58, 40, 34, 20, 2, 0, 0, 56, 100, 56, 0},
		{0, 4, 100, 0, 100, 60, 38, 36, 22, 4, 0, 0, 58, 100, 58, 0},
		{0, 2, 100, 0, 100, 62, 36, 38, 24, 6, 0, 0, 60, 100, 60, 0},
		{0, 0, 98, 0, 100, 64, 34, 40, 26, 8, 0, 0, 62, 100, 62, 0},
		{0, 0, 96, 0, 100, 66, 32, 42, 28, 10, 0, 0, 64, 98, 64, 0},
		{0, 0, 94, 0, 100, 68, 30, 44, 30, 12, 0, 0, 66, 96, 66, 0},
		{0, 0, 92, 0, 100, 70, 28, 46, 32, 14, 0, 0, 68, 94, 68, 0},
		{0, 0, 90, 0, 100, 72, 26, 48, 34, 16, 0, 0, 70, 92, 70, 0},
		{0, 0, 88, 0, 100, 74, 24, 50, 36, 18, 0, 0, 72, 90, 72, 0},
		{0, 0, 86, 0, 100, 76, 22, 52, 38, 20, 0, 0, 74, 88, 74, 0},
		{0, 0, 84, 0, 98, 78, 20, 54, 40, 22, 0, 0, 76, 86, 76, 0},
		{0, 0, 82, 0, 96, 80, 18, 56, 42, 24, 0, 0, 78, 84, 78, 0},
		{0, 0, 80, 0, 94, 82, 16, 58, 44, 26, 0, 0, 80, 82, 80, 0},
		{0, 0, 78, 0, 92, 84, 14, 60, 46, 28, 0, 0, 82, 80, 82, 0},
		{0, 0, 76, 0, 90, 86, 12, 62, 48, 30, 0, 0, 84, 78, 84, 0},
		{0, 0, 74, 0, 88, 88, 10, 64, 50, 32, 0, 0, 86, 76, 86, 0},
		{0, 0, 72, 0, 86, 90, 8, 66, 52, 34, 2, 0, 88, 74, 88, 0},
		{0, 0, 70, 0, 84, 92, 6, 68, 54, 36, 4, 0, 90, 72, 90, 0},
		{0, 0, 68, 0, 82, 94, 4, 70, 56, 38, 6, 0, 92, 70, 92, 0},
		{0, 0, 66, 0, 80, 96, 2, 72, 58, 40, 8, 0, 94, 68, 94, 0},
		{0, 0, 64, 0, 78, 98, 0, 74, 60, 42, 10, 0, 96, 66, 96, 0},
		{0, 0, 62, 0, 76, 100, 0, 76, 62, 44, 12, 0, 98, 64, 98, 0},
		{0, 0, 60, 0, 74, 100, 0, 78, 64, 46, 14, 0, 100, 62, 100, 0},
		{0, 0, 58, 0, 72, 100, 0, 80, 66, 48, 16, 0, 100, 60, 100, 0},
		{0, 0, 56, 0, 70, 100, 0, 82, 68, 50, 18, 0, 100, 58, 100, 0},
		{0, 0, 54, 0, 68, 100, 0, 84, 70, 52, 20, 0, 100, 56, 100, 0},
		{0, 0, 52, 0, 66, 100, 0, 86, 72, 54, 22, 0, 100, 54, 100, 0},
		{0, 0, 50, 0, 64, 100, 0, 88, 74, 56, 24, 0, 100, 52, 100, 0},
		{0, 0, 48, 0, 62, 100, 0, 90, 76, 58, 26, 0, 100, 50, 100, 0},
		{0, 0, 46, 0, 60, 100, 0, 92, 78, 60, 28, 0, 100, 48, 100, 0},
		{0, 0, 44, 0, 58, 100, 0, 94, 80, 62, 30, 0, 100, 46, 100, 0},
		{0, 0, 42, 0, 56, 100, 0, 96, 82, 64, 32, 0, 100, 44, 100, 0},
		{0, 0, 40, 0, 54, 100, 0, 98, 84, 66, 34, 0, 100, 42, 100, 0},
		{0, 0, 38, 0, 52, 98, 0, 100, 86, 68, 36, 0, 100, 40, 100, 0},
		{0, 0, 36, 0, 50, 96, 0, 100, 88, 70, 38, 0, 98, 38, 98, 0},
		{0, 0, 34, 0, 48, 94, 0, 100, 90, 72, 40, 0, 96, 36, 96, 0},
		{0, 0, 32, 0, 46, 92, 0, 100, 92, 74, 42, 0, 94, 34, 94, 0},
		{0, 0, 30, 0, 44, 90, 0, 100, 94, 76, 44, 0, 92, 32, 92, 0},
		{0, 0, 28, 0, 42, 88, 0, 100, 96, 78, 46, 0, 90, 30, 90, 0},
		{0, 0, 26, 0, 40, 86, 0, 100, 98, 80, 48, 0, 88, 28, 88, 0},
		{0, 0, 24, 0, 38, 84, 0, 100, 100, 82, 50, 0, 86, 26, 86, 0},
		{0, 0, 22, 0, 36, 82, 0, 100, 100, 84, 52, 0, 84, 24, 84, 0},
		{0, 0, 20, 0, 34, 80, 0, 100, 100, 86, 54, 0, 82, 22, 82, 0},
		{0, 0, 18, 0, 32, 78, 0, 100, 100, 88, 56, 0, 80, 20, 80, 0},
		{0, 0, 16, 0, 30, 76, 0, 100, 100, 90, 58, 0, 78, 18, 78, 0},
		{0, 0, 14, 0, 28, 74, 0, 98, 100, 92, 60, 0, 76, 16, 76, 0},
		{0, 0, 12, 0, 26, 72, 0, 96, 100, 94, 62, 0, 74, 14, 74, 0},
		{0, 2, 10, 0, 24, 70, 0, 94, 100, 96, 64, 0, 72, 12, 72, 0},
		{0, 4, 8, 0, 22, 68, 0, 92, 100, 98, 66, 0, 70, 10, 70, 0},
		{0, 6, 6, 0, 20, 66, 0, 90, 100, 100, 68, 0, 68, 8, 68, 0},
		{0, 8, 4, 0, 18, 64, 0, 88, 100, 100, 70, 0, 66, 6, 66, 0},
		{0, 10, 2, 0, 16, 62, 0, 86, 100, 100, 72, 0, 64, 4, 64, 0},
		{0, 12, 0, 0, 14, 60, 0, 84, 98, 100, 74, 0, 62, 2, 62, 0},
		{0, 14, 0, 0, 12, 58, 0, 82, 96, 100, 76, 0, 60, 0, 60, 0},
		{0, 16, 0, 0, 10, 56, 0, 80, 94, 100, 78, 0, 58, 0, 58, 0},
		{0, 18, 0, 0, 8, 54, 0, 78, 92, 100, 80, 0, 56, 0, 56, 0},
		{0, 20, 0, 0, 6, 52, 0, 76, 90, 100, 82, 0, 54, 0, 54, 0},
		{0, 22, 0, 0, 4, 50, 0, 74, 88, 100, 84, 0, 52, 0, 52, 0},
		{0, 24, 0, 0, 2, 48, 0, 72, 86, 100, 86, 0, 50, 0, 50, 0},
		{0, 26, 0, 0, 0, 46, 0, 70, 84, 100, 88, 0, 48, 0, 48, 0},
		{0, 28, 0, 0, 0, 44, 0, 68, 82, 100, 90, 0, 46, 0, 46, 0},
		{0, 30, 0, 0, 0, 42, 0, 66, 80, 98, 92, 0, 44, 0, 44, 0},
		{0, 32, 0, 0, 0, 40, 0, 64, 78, 96, 94, 0, 42, 0, 42, 0},
		{0, 34, 0, 0, 0, 38, 0, 62, 76, 94, 96, 0, 40, 0, 40, 0},
		{0, 36, 0, 0, 0, 36, 0, 60, 74, 92, 98, 0, 38, 0, 38, 0},
		{0, 38, 0, 0, 0, 34, 0, 58, 72, 90, 100, 0, 36, 0, 36, 0},
		{0, 40, 0, 0, 0, 32, 0, 56, 70, 88, 100, 0, 34, 0, 34, 0},
		{0, 42, 0, 0, 0, 30, 0, 54, 68, 86, 100, 0, 32, 0, 32, 0},
		{0, 44, 0, 0, 0, 28, 0, 52, 66, 84, 100, 0, 30, 0, 30, 0},
		{0, 46, 0, 0, 0, 26, 0, 50, 64, 82, 100, 0, 28, 0, 28, 0},
		{0, 48, 0, 0, 0, 24, 0, 48, 62, 80, 100, 0, 26, 0, 26, 0},
		{0, 50, 0, 0, 0, 22, 0, 46, 60, 78, 100, 0, 24, 0, 24, 0},
		{0, 52, 0, 0, 0, 20, 0, 44, 58, 76, 100, 0, 22, 0, 22, 0},
		{0, 54, 0, 0, 0, 18, 0, 42, 56, 74, 100, 0, 20, 0, 20, 0},
		{0, 56, 0, 0, 0, 16, 0, 40, 54, 72, 100, 0, 18, 0, 18, 0},
		{0, 58, 0, 0, 0, 14, 0, 38, 52, 70, 100, 0, 16, 0, 16, 0},
		{0, 60, 0, 0, 0, 12, 0, 36, 50, 68, 100, 0, 14, 0, 14, 0},
		{0, 62, 0, 0, 0, 10, 0, 34, 48, 66, 98, 0, 12, 0, 12, 0},
		{0, 64, 0, 0, 0, 8, 0, 32, 46, 64, 96, 0, 10, 0, 10, 0},
		{0, 66, 0, 0, 0, 6, 0, 30, 44, 62, 94, 0, 8, 0, 8, 0},
		{0, 68, 0, 0, 0, 4, 0, 28, 42, 60, 92, 0, 6, 0, 6, 0},
		{0, 70, 0, 0, 0, 2, 0, 26, 40, 58, 90, 0, 4, 0, 4, 0},
		{0, 72, 0, 0, 0, 0, 0, 24, 38, 56, 88, 0, 2, 0, 2, 0},
		{0, 74, 0, 0, 0, 0, 0, 22, 36, 54, 86, 0, 0, 0, 0, 0},
		{0, 76, 0, 0, 0, 0, 0, 20, 34, 52, 84, 0, 0, 0, 0, 0},
		{0, 78, 0, 0, 0, 0, 0, 18, 32, 50, 82, 0, 0, 0, 0, 0},
		{0, 80, 0, 0, 0, 0, 0, 16, 30, 48, 80, 0, 0, 0, 0, 0},
		{0, 82, 0, 0, 0, 0, 0, 14, 28, 46, 78, 0, 0, 0, 0, 0},
		{0, 84, 0, 0, 0, 0, 0, 12, 26, 44, 76, 0, 0, 0, 0, 0},
		{0, 86, 0, 0, 0, 0, 0, 10, 24, 42, 74, 0, 0, 0, 0, 0},
		{0, 88, 0, 0, 0, 0, 0, 8, 22, 40, 72, 0, 0, 0, 0, 0},
		{0, 90, 0, 0, 0, 0, 0, 6, 20, 38, 70, 0, 0, 0, 0, 0},
		{0, 92, 0, 0, 0, 0, 0, 4, 18, 36, 68, 0, 0, 0, 0, 0},
		{0, 94, 0, 0, 0, 0, 0, 2, 16, 34, 66, 0, 0, 0, 0, 0},
		{0, 96, 0, 0, 0, 0, 0, 0, 14, 32, 64, 0, 0, 0, 0, 0},
		{0, 98, 0, 0, 0, 0, 0, 0, 12, 30, 62, 0, 0, 0, 0, 0},
		{0, 100, 0, 0, 0, 0, 0, 0, 10, 28, 60, 0, 0, 0, 0, 0},
		{0, 100, 0, 0, 0, 0, 0, 0, 8, 26, 58, 0, 0, 0, 0, 0},
		{0, 100, 0, 0, 0, 0, 0, 0, 6, 24, 56, 0, 0, 0, 0, 0},
		{0, 98, 0, 0, 0, 0, 0, 0, 4, 22, 54, 0, 0, 0, 0, 0},
		{0, 96, 0, 0, 0, 0, 0, 0, 2, 20, 52, 0, 0, 0, 0, 0},
		{0, 94, 0, 0, 0, 0, 0, 90, 0, 18, 50, 0, 0, 0, 0, 0},
		{0, 92, 0, 0, 0, 0, 0, 95, 0, 16, 48, 0, 0, 0, 0, 0},
		{0, 90, 0, 0, 0, 0, 0, 100, 0, 14, 46, 0, 0, 0, 0, 0},
		{0, 88, 0, 0, 0, 0, 0, 100, 0, 12, 44, 0, 0, 0, 0, 0},
		{0, 86, 0, 0, 0, 0, 0, 100, 0, 10, 42, 0, 0, 0, 0, 0},
		{0, 84, 0, 0, 0, 0, 0, 95, 0, 8, 40, 0, 0, 0, 0, 0},
		{0, 82, 0, 0, 0, 0, 0, 90, 0, 6, 38, 0, 0, 0, 0, 0},
		{0, 80, 0, 0, 0, 0, 0, 85, 0, 4, 36, 0, 0, 0, 0, 0},
		{0, 78, 0, 0, 0, 0, 0, 80, 0, 2, 34, 0, 0, 0, 0, 0},
		{0, 76, 0, 0, 0, 0, 0, 75, 0, 0, 32, 0, 0, 0, 0, 0},
		{0, 74, 0, 0, 0, 0, 0, 70, 0, 0, 30, 0, 0, 0, 0, 0},
		{0, 72, 0, 0, 0, 0, 0, 65, 0, 0, 28, 0, 0, 0, 0, 0},
		{0, 70, 0, 0, 0, 0, 0, 60, 0, 0, 26, 0, 0, 0, 0, 0},
		{0, 68, 0, 0, 0, 0, 0, 55, 0, 0, 24, 0, 0, 0, 0, 0},
		{0, 66, 0, 0, 5, 0, 0, 50, 0, 0, 22, 5, 0, 5, 0, 0},
		{0, 64, 0, 0, 10, 0, 0, 45, 0, 0, 20, 10, 0, 10, 0, 0},
		{0, 62, 0, 0, 15, 0, 0, 40, 0, 0, 18, 15, 0, 15, 0, 0},
		{0, 60, 0, 0, 20, 0, 0, 35, 0, 0, 16, 20, 0, 20, 0, 0},
		{0, 58, 0, 0, 25, 0, 0, 30, 0, 0, 14, 25, 0, 25, 0, 0},
		{0, 56, 0, 0, 30, 0, 0, 25, 0, 0, 12, 30, 0, 30, 0, 0},
		{0, 54, 0, 0, 35, 0, 0, 20, 0, 0, 10, 35, 0, 35, 0, 0},
		{0, 52, 0, 0, 40, 0, 0, 15, 0, 0, 8, 40, 0, 40, 0, 0},
		{0, 50, 0, 0, 45, 0, 0, 10, 0, 0, 6, 45, 0, 45, 0, 0},
		{0, 48, 0, 0, 50, 0, 0, 5, 0, 0, 4, 50, 0, 50, 0, 0},
		{0, 46, 0, 0, 55, 0, 0, 0, 0, 0, 2, 55, 0, 55, 0, 0},

	};
	if (_seg_rt->aux_param3 == 0)
	{
		Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
	}
	if (_seg_rt->counter_mode_step == 0)
	{
		if (_seg_rt->aux_param3 < 255)
			_seg_rt->aux_param3++;
		else
			_seg_rt->aux_param3 = 0;
	}

	int led_r = (_seg->colors[0] >> 16) & 0xff;
	int led_g = (_seg->colors[0] >> 8) & 0xff;
	int led_b = _seg->colors[0] & 0xff;
	// printf("led_r = %d ,led_g = %d ,led_b = %d",led_r,led_g,led_b);

	for (int deng = 0; deng < 16; deng++)
	{
		// printf("led[%d][%d] = %d" , _seg_rt->counter_mode_step, deng, led[_seg_rt->counter_mode_step][deng]);
		int _r = (led_r * led[_seg_rt->counter_mode_step][deng]) / 100;
		int _g = (led_g * led[_seg_rt->counter_mode_step][deng]) / 100;
		int _b = (led_b * led[_seg_rt->counter_mode_step][deng]) / 100;
		// printf("deng = %d, _r = %d , _g  = %d, _b = %d",deng, _r , _g , _b);
		WS2812FX_setPixelColor_rgb(deng, _r, _g, _b);
	}

	_seg_rt->counter_mode_step++;
	_seg_rt->counter_mode_step %= 266;
	return _seg->speed;
}

u16 led_strip_rgb_anim_meteor_signle_color(void)
{
	const u8 max_rate = 12; // 流星长度
	uint16_t index;
	static uint8_t i = 0;
	uint32_t r1, g1, b1, w1;
	const uint8_t rate[max_rate] = {100, 75, 45, 30, 20, 15, 10, 7, 5, 3, 2, 0, 0, 0, 0, 0, 0};
	_seg_rt->aux_param3++;
	if (_seg_rt->aux_param3 < 9)
	{
		i = 0;
		// _seg->options |= FADE_XSLOW;
		/* 渐灭当前灯 */
		WS2812FX_fade_out_targetColor(BLACK);
		return (_seg->speed);
	}

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

	// if(_seg_rt->counter_mode_step == 0)
	// for( index = _seg->start; index < _seg->stop; index++)
	index = _seg_rt->counter_mode_step;
	{
		r1 = r * rate[i] / 100;
		g1 = g * rate[i] / 100;
		b1 = b * rate[i] / 100;
		w1 = w * rate[i] / 100;

		if (IS_REVERSE) // 反向
		{
			WS2812FX_setPixelColor_rgbw(_seg->stop, r1, g1, b1, w1);
		}
		else
		{
			WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1);
		}

		i++;
		i %= max_rate;
		if (i == 0)
		{
			_seg_rt->aux_param++;
			_seg_rt->aux_param %= _seg->c_n;
		}
	}
	_seg_rt->counter_mode_step++;
	_seg_rt->counter_mode_step %= _seg_len;

	return (_seg->speed);
}

u16 led_strip_rgb_anim_background_meteor(void)
{
	const u8 max_rate = 12;
	static uint8_t i = 0;
	uint32_t r1, g1, b1, w1;

	const uint8_t rate[max_rate] = {100, 70, 45, 30, 20, 15, 10, 7, 5, 4, 3, 2};
	// const uint8_t rate[meteor_len] = {100,60,45,30,20,15,10,7,5,4};  //亮度值 0-100
	int w = (_seg->colors[0] >> 24) & 0xff;
	int r = (_seg->colors[0] >> 16) & 0xff;
	int g = (_seg->colors[0] >> 8) & 0xff;
	int b = _seg->colors[0] & 0xff;
	if (_seg_rt->aux_param == 0)
	{
		i = 0;
		_seg_rt->aux_param = 1;
		Adafruit_NeoPixel_fill(_seg->colors[1], _seg->start, _seg_len);
	}

	WS2812FX_copyPixels(_seg->start + 1, _seg->start, _seg_len - 1);

	r1 = r * rate[i] / 100;
	g1 = g * rate[i] / 100;
	b1 = b * rate[i] / 100;
	w1 = w * rate[i] / 100;

	if (i >= max_rate)
	{
		r1 = _seg->colors[1] >> 16;
		g1 = _seg->colors[1] >> 8;
		b1 = _seg->colors[1] >> 0;
	}
	WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1);
	if (i < max_rate)
		i++;

	_seg_rt->counter_mode_step++;
	_seg_rt->counter_mode_step %= _seg_len - 1 + max_rate * 2;

	if (_seg_rt->counter_mode_step == 0)
	{
		i = 0;
	}

	return (_seg->speed);
}

uint32_t __fade_in__(uint32_t c, uint8_t *f)
{
	static float i = 1;
	uint8_t r, g, b;
	r = c >> 16;
	g = c >> 8;
	b = c >> 0;

	r = r * (i) / 255;
	g = g * (i) / 255;
	b = b * (i) / 255;
	if (i < 128)
		i = i * 2;
	else
		i = i * 1.2;
	*f = 0;
	if (i >= 255)
	{
		i = 1;
		*f = 1;
		return c;
	}

	c = r << 16 | g << 8 | b;
	return c;
}

/* 开幕式 ,支持多种颜色
1、慢慢渐亮到目标颜色
2、拉开帷幕
3、完成开幕后是全黑的
4、开始新的开幕，重复从“1”开始
*/
u16 led_strip_rgb_anim_open(void)
{
	uint8_t f;
	if (_seg_rt->counter_mode_step == 0)
	{
		Adafruit_NeoPixel_fill(__fade_in__(_seg->colors[_seg_rt->aux_param], &f), _seg->start, _seg_len);
		if (f == 0)
			return (_seg->speed);
	}

	WS2812FX_setPixelColor(_seg_len / 2 - _seg_rt->counter_mode_step, BLACK);
	WS2812FX_setPixelColor(_seg_len / 2 + _seg_rt->counter_mode_step, BLACK);

	_seg_rt->counter_mode_step++;
	_seg_rt->counter_mode_step %= _seg_len / 2;
	if (_seg_rt->counter_mode_step == 0)
	{
		_seg_rt->aux_param++;
		_seg_rt->aux_param %= _seg->c_n;
	}
	return (_seg->speed);
}

/* 闭幕式 ,支持多种颜色*/
u16 led_strip_rgb_anim_close(void)
{
	if (_seg_rt->counter_mode_step == 0)
	{
		_seg_rt->aux_param3++;
		if (_seg_rt->aux_param3 < 9)
		{

			WS2812FX_fade_out_targetColor(BLACK);
			return (_seg->speed);
		}
		_seg_rt->aux_param3 = 0;
		_seg_rt->aux_param++;
		_seg_rt->aux_param %= _seg->c_n;
	}
	WS2812FX_setPixelColor(_seg->start + _seg_rt->counter_mode_step, _seg->colors[_seg_rt->aux_param]);
	WS2812FX_setPixelColor(_seg_len - _seg_rt->counter_mode_step - 1, _seg->colors[_seg_rt->aux_param]);

	_seg_rt->counter_mode_step++;
	_seg_rt->counter_mode_step %= _seg_len / 2;

	return (_seg->speed);
}

u16 led_strip_rgb_anim_multi_dot_running(void)
{
	uint32_t color;
	uint16_t i;
	for (i = 0; i < _seg_len; i++)
	{
		if (_seg_rt->aux_param3 == 0)
		{
			// if(_seg_rt->counter_mode_step % 1==0)
			{
				// _seg_rt->counter_mode_step = 0;
				_seg_rt->aux_param3 = 1;
			}
			/* 0-_seg->c_n-2 是前景色 */
			color = _seg->colors[_seg_rt->aux_param2];
			_seg_rt->aux_param2++;
			_seg_rt->aux_param2 %= _seg->c_n - 1;
			// color = WS2812FX_color_wheel(_seg_rt->aux_param2+=70);//随机颜色
		}
		else
		{
			/* 构造背景色 */
			if (_seg_rt->counter_mode_step % 5 == 0)
			{
				// _seg_rt->counter_mode_step = 0;

				_seg_rt->aux_param3 = 0;
			}
			/* 最后一个颜色是背景色 */
			color = _seg->colors[_seg->c_n - 1];
		}

		if (IS_REVERSE) // 反向
		{
			WS2812FX_setPixelColor(_seg->start + i, color);
		}
		else
		{
			WS2812FX_setPixelColor(_seg->stop - i, color);
		}
		_seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % _seg_len;
	}
	_seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) % _seg_len;
	return (_seg->speed);
}

u16 led_strip_rgb_anim_meteor_jump(void)
{
	const u8 max_rate = 12;
	static uint8_t i = 0;
	if (_seg_rt->counter_mode_step == 0)
	{
		i = 0;
		SET_CYCLE;
		// fc_effect.mode_cycle = 1;
	}

	//-------------------- 周期 ---------------------------------------------
	if ((get_effect_p() == 1) && (fc_effect.mode_cycle == 1)) // 计时中 && 完成一个循环
	{
		return (_seg->speed);
	}
	// uint8_t size = (SIZE_OPTION << 1) + 1;

	uint32_t r1 = 0, g1 = 0, b1 = 0, w1 = 0; // 必须初始化
	int w = 0, r = 0, g = 0, c = 0, b = 0;	 // 必须初始化

	// const uint8_t rate[MAX_RATE] = {100,50,40,30,20,10};  //亮度值 0-100
	const uint8_t rate[max_rate] = {100, 60, 45, 30, 20, 15, 10, 7, 5, 4}; // 亮度值 0-100

	uint8_t n;

	//---------------------- 效果过程 --------------------------------------
	w = (_seg->colors[_seg_rt->aux_param] >> 24) & 0xff;
	r = (_seg->colors[_seg_rt->aux_param] >> 16) & 0xff;
	g = (_seg->colors[_seg_rt->aux_param] >> 8) & 0xff;
	b = _seg->colors[_seg_rt->aux_param] & 0xff;

	WS2812FX_copyPixels(_seg->start + 1, _seg->start, _seg_len - 1); // 颜色平移

	// 调节亮度
	if (i < max_rate) // 避免数组溢出
	{
		r1 = r * rate[i] / 100;
		g1 = g * rate[i] / 100;
		b1 = b * rate[i] / 100;
		w1 = w * rate[i] / 100;
		i++;
	}

	WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1); // 颜色值
	// 整段变色
	//  n = _seg_rt->counter_mode_step;
	//  if((n % (fc_effect.led_num / _seg->c_n) )== 0)  //6是流星的长度，根据流星长度而变
	n = _seg_rt->counter_mode_step % (3);
	// n =  _seg_rt->counter_mode_step % (fc_effect.led_num / _seg->c_n);
	if (n == 0)
	{
		_seg_rt->aux_param++;
		_seg_rt->aux_param %= _seg->c_n; // 颜色数量

		for (uint8_t m = 0, i = 0; m < max_rate; m++, i++)
		{
			w = (_seg->colors[_seg_rt->aux_param] >> 24) & 0xff;
			r = (_seg->colors[_seg_rt->aux_param] >> 16) & 0xff;
			g = (_seg->colors[_seg_rt->aux_param] >> 8) & 0xff;
			b = _seg->colors[_seg_rt->aux_param] & 0xff;

			r1 = r * rate[i] / 100;
			g1 = g * rate[i] / 100;
			b1 = b * rate[i] / 100;
			w1 = w * rate[i] / 100;
			// if(_seg_rt->counter_mode_step < 15)
			WS2812FX_setPixelColor_rgbw(_seg_rt->counter_mode_step - m, r1, g1, b1, w1); // 颜色值
		}
	}

	//----------------- 结束条件 ------------------------------------------

	_seg_rt->counter_mode_step++;
	_seg_rt->counter_mode_step %= _seg_len + max_rate;
	if (_seg_rt->counter_mode_step == 0)
		fc_effect.mode_cycle = 1;

	return (_seg->speed);
}

/**
 * @brief 颜色渐变发生器
 * 		每次调用返回一个介于当前颜色对之间的过渡色，
 * 		周期性地从颜色池中轮转取色，实现多种颜色之间的平滑渐变
 *
 * @param add_step 每次递进的步长
 *
 */
u32 __get_next_gradual_color__(uint8_t add_step)
{
	static int gadual_step = 0;
	static uint8_t index;
	static uint32_t c0, c1;
	int lum = gadual_step;
	uint32_t color;
	if (lum > 255)
		lum = 511 - lum; // lum = 0 -> 255 -> 0
	if (_seg_rt->aux_param == 0)
	{
		_seg_rt->aux_param = 1;
		index = 0;
		c1 = _seg->colors[index];
		index++;
		c0 = _seg->colors[index];
	}
	// c1目标颜色
	color = WS2812FX_color_blend(c1, c0, lum);
	if (gadual_step == 256)
	{

		index++;
		index %= _seg->c_n; // 颜色池的颜色数量
		c1 = _seg->colors[index];
	}
	gadual_step += add_step; // 修改渐变速度
	if (gadual_step > 511)
	{

		gadual_step = 0;
		index++;
		index %= _seg->c_n; // 颜色池的颜色数量
		c0 = _seg->colors[index];
	}
	return color;
}

u16 led_strip_rgb_anim_meteor_gradual(void)
{
	const u8 max_rate = 12;
	static uint8_t i = 0;
	if (_seg_rt->counter_mode_step == 0)
	{
		i = 0;
		SET_CYCLE;
		// fc_effect.mode_cycle = 1;
		// gadual_step = 0; //新的周期
	}

	if ((get_effect_p() == 1) && (fc_effect.mode_cycle == 1)) // 计时中 && 完成一个循环
	{
		return (_seg->speed);
	}

	uint32_t r1 = 0, g1 = 0, b1 = 0, w1 = 0; // 必须初始化
	int w = 0, r = 0, g = 0, c = 0, b = 0;	 // 必须初始化
	// const uint8_t rate[MAX_RATE] = {100,50,40,30,20,10};  //亮度值 0-100
	const uint8_t rate[max_rate] = {100, 60, 45, 30, 20, 15, 10, 7, 5, 4}; // 亮度值 0-100

	uint32_t color;
	// 获取颜色
	color = __get_next_gradual_color__(20);
	w = (color >> 24) & 0xff;
	r = (color >> 16) & 0xff;
	g = (color >> 8) & 0xff;
	b = color & 0xff;

	WS2812FX_copyPixels(_seg->start + 1, _seg->start, _seg_len - 1); // 颜色平移  控制流星移动

	// 调节亮度
	if (i < max_rate) // 避免数组溢出
	{
		r1 = r * rate[i] / 100;
		g1 = g * rate[i] / 100;
		b1 = b * rate[i] / 100;
		w1 = w * rate[i] / 100;
		i++;
	}

	WS2812FX_setPixelColor_rgbw(_seg->start, r1, g1, b1, w1); // 颜色值
	if (_seg_rt->counter_mode_step > 5)
	//  if(_seg_rt->counter_mode_step > (fc_effect.led_num / _seg->c_n))
	{
		for (int n = 0, i = 0; n < max_rate; n++, i++) // i和n都是流星的长度
		{
			// color = __get_next_gradual_color__(5);
			w = (color >> 24) & 0xff;
			r = (color >> 16) & 0xff;
			g = (color >> 8) & 0xff;
			b = color & 0xff;

			r1 = r * rate[i] / 100;
			g1 = g * rate[i] / 100;
			b1 = b * rate[i] / 100;
			w1 = w * rate[i] / 100;
			WS2812FX_setPixelColor_rgbw(_seg_rt->counter_mode_step - n, r1, g1, b1, w1); // 颜色值
		}
	}

	// 控制灯的流动长度
	_seg_rt->counter_mode_step++;
	_seg_rt->counter_mode_step %= _seg_len + max_rate;

	if (_seg_rt->counter_mode_step == 0)
		fc_effect.mode_cycle = 1;

	return (_seg->speed);
}

/*
 * Cycles a rainbow over the entire string of LEDs.
 彩虹渐变颜色流水效果
 60 可修改彩虹颜色长度
 */
u16 led_strip_rgb_anim_rainbow_flow(void)
{
	for (uint16_t i = 0; i < 60; i++)
	{
		uint32_t color = WS2812FX_color_wheel(((i * 256 / 60) + _seg_rt->counter_mode_step) & 0xFF);
		WS2812FX_setPixelColor(_seg->stop - i, color);
	}

	_seg_rt->counter_mode_step = (_seg_rt->counter_mode_step + 1) & 0xFF;

	if (_seg_rt->counter_mode_step == 0)
		SET_CYCLE;

	return (_seg->speed / 256);
}

// 调整RGB线序时，对应的动画效果
u16 led_strip_rgb_anim_when_adjust_sequence(void)
{
	u32 color = BLUE;
	switch (_seg_rt->counter_mode_step)
	{
	case 0:
		color = RED;
		break;
	case 1:
		color = GREEN;
		break;
	case 2:
		color = BLUE;
		break;
	}

	Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

	if (_seg_rt->counter_mode_step < 3)
	{
		_seg_rt->counter_mode_step++;
	}
	return 1000;
}
