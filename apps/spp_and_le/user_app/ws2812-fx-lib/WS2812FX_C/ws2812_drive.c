
#include "ws2812_bsp.h"
#include "debug.h"

static unsigned long tick_ms;
void ws281x_init()
{
}

void ws281x_show(unsigned char *pixels_pattern, unsigned short pattern_size)
{

    // 幻彩驱动
    extern void ledc_drive_buf(u8 * buf, u16 num);
    ledc_drive_buf(pixels_pattern, pattern_size);
}

// 周期10ms
unsigned long HAL_GetTick(void)
{
    return tick_ms;
}

// 每10ms调用一次
void run_tick_per_10ms(void)
{
    tick_ms += 10;
}
