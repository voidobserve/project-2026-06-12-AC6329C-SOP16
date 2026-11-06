#ifndef __USER_BLE_NOTIFY_DRIVER_H__
#define __USER_BLE_NOTIFY_DRIVER_H__

#include "typedef.h"

// 发送缓冲区中，一条指令最大的长度：
#define USER_BLE_NOTIFY_SEND_BUFF_MAX_LEN 30
// 发送缓冲区中，最大的指令数量：
#define USER_BLE_NOTIFY_SEND_BUFF_MAX_NUM 30

void user_ble_notify_param_put(u8 *buff, u16 len);
void user_ble_notify_param_handle(void);

#endif