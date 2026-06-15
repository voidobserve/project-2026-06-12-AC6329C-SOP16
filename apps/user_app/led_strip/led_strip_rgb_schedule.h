#ifndef __LED_STRIP_RGB_SCHEDULE_H__
#define __LED_STRIP_RGB_SCHEDULE_H__
// RBG幻彩灯带，调度 头文件

#include "typedef.h"
#include "WS2812FX.H"
#include "led_strand_effect.h"


#if 0
// 变化方式
typedef enum
{
	MODE_MUTIL_RAINBOW = 2,		   // 彩虹(多段颜色)
	MODE_MUTIL_JUMP = 10,		   // 跳变模式(多段颜色)
	MODE_MUTIL_BRAETH = 11,		   // 呼吸模式(多段颜色)
	MODE_MUTIL_TWIHKLE = 12,	   // 闪烁模式(多段颜色)
	MODE_MUTIL_FLOW_WATER = 13,	   // 流水模式(多段颜色)
	MODE_CHAS_LIGHT = 14,		   // 追光模式
	MODE_MUTIL_COLORFUL = 15,	   // 炫彩模式(多段颜色)
	MODE_MUTIL_SEG_GRADUAL = 16,   // 渐变模式(多段颜色)
	MODE_JUMP,					   // 标准跳变
	MODE_STROBE,				   // 频闪，颜色之间插入黑mode
	MODE_MUTIL_C_GRADUAL,		   // 多种颜色切换整条渐变
	MODE_2_C_FIX_FLOW,			   // 两种颜色混合流水，渐变色流水
	MODE_SINGLE_FLASH_RANDOM = 21, // 星空效果，单灯随机闪烁
	MODE_SEG_FLASH_RANDOM = 22,	   // 星云效果，一段随机闪烁
	MODE_SINGLE_METEOR = 23,	   // 流星效果
	MODE_SINGLE_C_BREATH = 24,	   // 单色呼吸
	MODE_B_G_METEOR = 25,		   // 带背景色流星
	MODE_OPEN = 26,				   // 开幕式
	MODE_CLOSE = 27,			   // 闭幕式
	MODE_DOT_RUNNING = 28,		   // 多个点跑马 ，点和点直接固定间隔5，支持每个点不同颜色，支持设置背景色
	MODE_COLOR_METEOR = 29,		   // 无背景色，指定颜色的流星
	MODE_JUMP_METEORR = 30,		   // 跳变效果的流星
	MODE_GRADUAL_METEOR = 31,	   // 渐变效果的流星
	MODE_RAINBOW_FLOW = 32,

} change_type_e;

// 方向
typedef enum
{
	IS_forward = 0, // 正向
	IS_back = 16	// 反向
} direction_e;

typedef struct
{ 
	u8 r;
	u8 g;
	u8 b; 
} color_t;

typedef struct
{
	change_type_e change_type; // 变化类型、模式
	direction_e direction;
	unsigned char seg_size; // 段大小
	unsigned char c_n;		// 颜色数量
	color_t rgb[MAX_NUM_COLORS];
	unsigned short speed; // 由档位决定
} dream_scene_t;
 
typedef struct
{
	dream_scene_t dream_scene; // 幻彩情景数据

	color_t rgb_data;

	u8 meteor_period; // 周期值，单位秒
	u8 mode_cycle;     // 1:模式完成一个循环。0：正在跑，和 meteor_period 搭配用   流星
	u16 period_cnt;  
} led_strip_rgb_t;
 

// extern volatile led_strip_rgb_t led_strip_rgb;
// extern volatile fc_effect_t led_strip_rgb;
#endif


typedef u16 (*led_strip_rgb_handler_t)(void);

// RGB幻彩灯的情景->处理函数映射表
typedef struct
{
	u8 light_scene; // 灯光情景
	led_strip_rgb_handler_t handler; // 灯光情景对应的处理函数
} led_strip_rgb_handler_map_t;

void led_strip_rgb_schedule_set_mode(mode_ptr mode, u16 speed, u8 option);

void led_strip_rgb_schedule_init(void);
void led_strip_rgb_schedule(void);

#endif
