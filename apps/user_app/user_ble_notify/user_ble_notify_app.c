#include "user_ble_notify_app.h"
#include "user_ble_notify_driver.h"

#include "led_strand_effect.h" // ALARM_CLOCK 类型定义

#include <string.h>

static const uint8_t prefix[] = {0x01, 0xE9, 0x00};
static volatile uint8_t send_buf[USER_BLE_NOTIFY_SEND_BUFF_MAX_LEN] = {0};
static volatile u8 send_buf_len = 0;

// 向app反馈声控类型：手机麦或者设备麦
void user_ble_notify_sound_control_type(u8 type)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x06;
    send_buf[send_buf_len++] = 0x07;
    send_buf[send_buf_len++] = type;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_brightness(u8 brightness)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x04;
    send_buf[send_buf_len++] = 0x03;
    send_buf[send_buf_len++] = brightness;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_speed(u8 speed)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x04;
    send_buf[send_buf_len++] = 0x04;
    send_buf[send_buf_len++] = speed;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_meteor_on_off_status(u8 on_off_status)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x2F;
    send_buf[send_buf_len++] = 0x02;
    send_buf[send_buf_len++] = on_off_status;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_meteor_period(u8 period)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x2F;
    send_buf[send_buf_len++] = 0x03;
    send_buf[send_buf_len++] = period;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_meteor_speed(u8 speed)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x2F;
    send_buf[send_buf_len++] = 0x01;
    send_buf[send_buf_len++] = speed;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_sound_control_sensitivity(u8 sensitivity)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x2F;
    send_buf[send_buf_len++] = 0x05;
    send_buf[send_buf_len++] = sensitivity;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

// 反馈 RGB 灯带的长度(灯珠数量)
void user_ble_notify_led_strip_rgb_len(u16 nums)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x04;
    send_buf[send_buf_len++] = 0x08;
    send_buf[send_buf_len++] = (nums >> 8) & 0xFF;
    send_buf[send_buf_len++] = (nums >> 0) & 0xFF;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

// 反馈设备类型
void user_ble_notify_dev_type(u8 dev_type)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x07;
    send_buf[send_buf_len++] = 0x01;
    send_buf[send_buf_len++] = 0x01;
    send_buf[send_buf_len++] = dev_type;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_dev_on_off_state(u8 on_off_state)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x01;
    send_buf[send_buf_len++] = 0x01;
    send_buf[send_buf_len++] = on_off_state;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_rgb_sequence(u8 sequence)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x04;
    send_buf[send_buf_len++] = 0x05;
    send_buf[send_buf_len++] = sequence;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

void user_ble_notify_alarm_data(u8 alarm_index, alarm_t alarm_data)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x05;
    switch (alarm_index) {
    case 0:
        // 闹钟 0
        send_buf[send_buf_len++] = 0x00;
        break;
    case 1:
        // 闹钟 1
        send_buf[send_buf_len++] = 0x01;
        break;
    case 2:
        // 闹钟 2
        send_buf[send_buf_len++] = 0x02;
        break;
    default:
        return;
        break;
    }

    send_buf[send_buf_len++] = alarm_data.hour;
    send_buf[send_buf_len++] = alarm_data.minute;
    send_buf[send_buf_len++] = alarm_data.enable;
    send_buf[send_buf_len++] = alarm_data.mode;

    user_ble_notify_param_put(send_buf, send_buf_len);
}

// 反馈声控模式对应的子模式
void user_ble_notify_sound_control_mode(u8 mode)
{
    memcpy(send_buf, prefix, ARRAY_SIZE(prefix));
    send_buf_len = ARRAY_SIZE(prefix);

    send_buf[send_buf_len++] = 0x06;
    send_buf[send_buf_len++] = 0x06;
    send_buf[send_buf_len++] = mode;

    user_ble_notify_param_put(send_buf, send_buf_len++);
}
