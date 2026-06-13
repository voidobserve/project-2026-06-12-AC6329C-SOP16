#include "includes.h"
#include "led_driver.h"


#if 0
// --------------------------------------------------------------------------定时器
static const u16 timer_div[] = {
    /*0000*/ 1,
    /*0001*/ 4,
    /*0010*/ 16,
    /*0011*/ 64,
    /*0100*/ 2,
    /*0101*/ 8,
    /*0110*/ 32,
    /*0111*/ 128,
    /*1000*/ 256,
    /*1001*/ 4 * 256,
    /*1010*/ 16 * 256,
    /*1011*/ 64 * 256,
    /*1100*/ 2 * 256,
    /*1101*/ 8 * 256,
    /*1110*/ 32 * 256,
    /*1111*/ 128 * 256,
};

#define APP_TIMER_CLK (24000000) // clk_get("timer")
#define MAX_TIME_CNT 0x7fff
#define MIN_TIME_CNT 0x100
#define TIMER_UNIT 1

#define TIMER_CON JL_TIMER2->CON
#define TIMER_CNT JL_TIMER2->CNT
#define TIMER_PRD JL_TIMER2->PRD
#define TIMER_VETOR IRQ_TIME2_IDX

___interrupt
    AT_VOLATILE_RAM_CODE void user_timer2_isr(void)
{
    TIMER_CON |= BIT(14); // 清除中断标志
    user_125us_isr();
}

void user_timer_init(void)
{
    u32 prd_cnt;
    u8 index;

    //	printf("********* user_timer_init **********\n");
    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++)
    {
        prd_cnt = TIMER_UNIT * (APP_TIMER_CLK / 8000) / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT)
        {
            break;
        }
    }

    TIMER_CNT = 0;
    TIMER_PRD = prd_cnt;
    request_irq(TIMER_VETOR, 3, user_timer2_isr, 0);
    TIMER_CON = (0b0001 << 10) | (index << 4) | (0x01 << 0); // 选择晶振作为时钟源，分频系数，定时器计数模式
}
__initcall(user_timer_init);
#endif

static void user_init(void)
{
    led_driver_init();
}

void user_main(void)
{
    user_init();

    while (1)
    {
        // printf("user_main \n");
        // os_time_dly(100);



        os_time_dly(1);
    }
}
