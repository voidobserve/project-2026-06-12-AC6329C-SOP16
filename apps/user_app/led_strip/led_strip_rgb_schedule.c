// RBG幻彩灯带，调度 .c 文件
#include "led_strip_rgb_schedule.h"
#include "led_strip_driver.h"
#include "WS2812FX.H"
#include "led_strip_rgb_anim.h" // 动画效果
#include "Adafruit_NeoPixel.H"
#include "ws2812fx_effect.h"

// volatile led_strip_rgb_t led_strip_rgb;
// volatile fc_effect_t led_strip_rgb;
void led_strip_rgb_schedule_init(void)
{
	fc_effect.on_off_flag = DEVICE_ON;
	fc_effect.led_num = 400;
	fc_effect.Now_state = IS_light_scene;
	fc_effect.rgb.r = 255;
	fc_effect.rgb.g = 0;
	fc_effect.rgb.b = 0;
	fc_effect.dream_scene.speed = 100;
	fc_effect.sequence = NEO_RGB;
	fc_effect.b = 255;
	fc_effect.b_per = 100;
	fc_effect.speed = 90;
	fc_effect.music.m = 1;
	fc_effect.music.s = 90;
	// 流星周期控制
	fc_effect.meteor_period = 8;						   // 默认8秒  周期值
	fc_effect.period_cnt = fc_effect.meteor_period * 1000; // ms,运行时的计数器
	fc_effect.mode_cycle = 0;							   // 模式完成一个循环的标志

	// zd_countdown[0].set_on_off = DEVICE_OFF;
	// zd_countdown[1].set_on_off = DEVICE_OFF;
	// zd_countdown[2].set_on_off = DEVICE_OFF;

	fc_effect.dream_scene.c_n = 1;
	fc_effect.dream_scene.rgb[0].r = 0;
	fc_effect.dream_scene.rgb[0].g = 255;
	fc_effect.dream_scene.rgb[0].b = 0;

	fc_effect.dream_scene.change_type = MODE_COLOR_METEOR;

#if 0
	// led_strip_rgb.dream_scene.speed = 100;
	// led_strip_rgb.dream_scene.c_n = 1;
	// led_strip_rgb.dream_scene.rgb[0].r = 0;
	// led_strip_rgb.dream_scene.rgb[0].g = 255;
	// led_strip_rgb.dream_scene.rgb[0].b = 0;
	// led_strip_rgb.meteor_period = 8;							   // 默认8秒  周期值
	// led_strip_rgb.period_cnt = led_strip_rgb.meteor_period * 1000; // ms,运行时的计数器
	// led_strip_rgb.mode_cycle = 0;								   // 模式完成一个循环的标志
	// led_strip_rgb.dream_scene.change_type = MODE_COLOR_METEOR;
#endif
}

void __led_strip_rgb_set_mode_without_color__(mode_ptr mode, u16 speed, u8 option)
{
	WS2812FX_setSegment_colorOptions(
		LED_STRIP_RGB_STAR_INDEX,							// 第0段
		LED_STRIP_RGB_STAR_INDEX,							// 起始位置
		LED_STRIP_RGB_STAR_INDEX + LED_STRIP_RGB_LEN - 1,	// 结束位置
		mode,												// 效果
		0,													// 颜色
		speed,												// 速度
		option);											// 选项，这里像素点大小：3 REVERSE决定方向
	WS2812FX_resetSegmentRuntime(LED_STRIP_RGB_STAR_INDEX); // 清除指定段的显示缓存
	WS2812FX_running_flag_set();
}

void __led_strip_rgb_schedule_in_light_scene__(void)
{
	mode_ptr led_strip_rgb_mode = NULL; // 动画模式指针
	u16 speed = 0;
	u8 option = NO_OPTIONS;

	switch (fc_effect.dream_scene.change_type)
	{
#if 0
		case MODE_MUTIL_RAINBOW:      //彩虹
            strand_rainbow();
            // printf("\n IS_SCENE_RAINBOW");
        break;


        case MODE_MUTIL_BRAETH://呼吸模式
            strand_breath();
            break;

        case MODE_MUTIL_TWIHKLE://闪烁模式
            strand_twihkle();
            printf("\n IS_SCENE_TWIHKLE");
            break;

        case MODE_MUTIL_FLOW_WATER://流水模式
            strand_flow_water();
            printf("\n IS_SCENE_FLOW_WATER");
            break;

        case MODE_CHAS_LIGHT://追光模式
            strand_chas_light();
            printf("\n IS_SCENE_CHAS_LIGHT");
            break;

        case MODE_MUTIL_COLORFUL://炫彩模式
            strand_colorful();
            break;

        case MODE_MUTIL_SEG_GRADUAL://渐变模式
            mutil_seg_grandual();
            printf("\n IS_SCENE_GRADUAL_CHANGE");
            break;

        case MODE_JUMP:     //标准跳变
            standard_jump();
            break;

        case MODE_STROBE:   //标准频闪
            ls_strobe();
            break;

        case MODE_MUTIL_C_GRADUAL:  //多段同时渐变
            mutil_c_grandual();
            break;
        case MODE_2_C_FIX_FLOW:      //两种颜色混色流水
            tow_color_fix_flow();
            break;
        case MODE_SINGLE_FLASH_RANDOM:
            single_c_flash_random();
            break;
        case MODE_SEG_FLASH_RANDOM:
            seg_mutil_c_flash_random();
            break;
        case MODE_SINGLE_C_BREATH:   //单色呼吸
            printf("\n MODE_SINGLE_C_BREATH");
            single_c_breath();
            break;
        case MODE_SINGLE_METEOR:
            printf("\n MODE_SINGLE_METEOR");
            single_c_meteor();
            break;
        case MODE_B_G_METEOR:
            printf("\n MODE_B_G_METEOR");
            background_meteor();
            break;
        case MODE_OPEN:
            open_mode();
            break;
        case MODE_CLOSE:
            close_mode();
            break;
        case MODE_DOT_RUNNING:
            dot_running();
            break;

        case MODE_JUMP_METEORR:   //跳变效果的流星
            printf("\n MODE_JUMP_METEORR");
            color_jump_meteor();
            break;
        case MODE_GRADUAL_METEOR:   //渐变效果的流星
            printf("\n MODE_GRADUAL_METEOR");
            color_gradual_meteor();
            break;
        case MODE_RAINBOW_FLOW:
            rainbow_flow();
            break;
#endif
	case MODE_MUTIL_JUMP: // 跳变模式
		jump_mutil_c();
		// printf("\n IS_SCENE_JUMP_CHANGE");
		break;

	case MODE_COLOR_METEOR: // 无背景颜色的流星
		color_meteor();
		break;
	}
}

void led_strip_rgb_schedule(void)
{
	mode_ptr led_strip_rgb_mode = NULL; // 动画模式指针
	u16 speed = 0;
	u8 option = NO_OPTIONS;

	switch (fc_effect.Now_state)
	{
	case IS_light_scene:
		fc_effect.period_cnt = 0;
		__led_strip_rgb_schedule_in_light_scene__();
		break;
	// case ACT_TY_PAIR:
	// 	// 配对完成，要恢复fc_effect.Now_state
	// 	fc_pair_effect();
	// 	break;
	// case ACT_CUSTOM:
	// 	custom_effect();
	// 	break;
	// case IS_light_music:
	// 	fc_music();
	// 	break;
	// case IS_smear_adjust:
	// 	printf("\n IS_smear_adjust");
	// 	fc_smear_adjust();
	// 	break;
	case IS_STATIC:
		led_strip_rgb_mode = WS2812FX_mode_static;
		speed = 100;
		option = FADE_GLACIAL;
		WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
		ls_set_colors(1, &fc_effect.rgb);
		break;
	}

	switch (fc_effect.dream_scene.change_type)
	// switch (led_strip_rgb.dream_scene.change_type)
	{
	case MODE_COLOR_METEOR: // 无背景颜色的流星
		led_strip_rgb_mode = color_meteor;
		break;
	}

	WS2812FX_setSegment_colorOptions(
		LED_STRIP_RGB_STAR_INDEX,							// 第0段
		LED_STRIP_RGB_STAR_INDEX,							// 起始位置
		LED_STRIP_RGB_STAR_INDEX + LED_STRIP_RGB_LEN - 1,	// 结束位置
		led_strip_rgb_mode,									// 效果
		0,													// 颜色
		speed,												// 速度
		option);											// 选项，这里像素点大小：3 REVERSE决定方向
	WS2812FX_resetSegmentRuntime(LED_STRIP_RGB_STAR_INDEX); // 清除指定段的显示缓存
	WS2812FX_running_flag_set();
}

void meteor_period_sub(void)
{
	if (fc_effect.period_cnt > 10)
	{
		fc_effect.period_cnt -= 10;
	}
	else
	{
		fc_effect.period_cnt = 0; // 计数器清零
		if (fc_effect.mode_cycle) // 模式循环完成，更新
		{
			fc_effect.period_cnt = fc_effect.meteor_period * 1000;
			fc_effect.mode_cycle = 0;
		}
	}

#if 0
	if (led_strip_rgb.period_cnt > 10)
	{
		led_strip_rgb.period_cnt -= 10;
	}
	else
	{
		led_strip_rgb.period_cnt = 0; // 计数器清零
		if (led_strip_rgb.mode_cycle) // 模式循环完成，更新
		{
			led_strip_rgb.period_cnt = led_strip_rgb.meteor_period * 1000;
			led_strip_rgb.mode_cycle = 0;
		}
	}
#endif
}
