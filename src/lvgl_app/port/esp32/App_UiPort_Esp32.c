/**
 * @file App_UiPort_Esp32.c
 * @brief ESP32 平台适配层实现。
 *
 * 【编译环境】
 * 此文件在 ESP-IDF 环境下编译，使用 FreeRTOS API。
 *
 * 【当前状态】
 * 骨架代码，实际移植到具体硬件时需要：
 *   - 初始化显示驱动并关联到 LVGL
 *   - 初始化输入设备（编码器/触摸）
 *   - 实现 RequestFlush 的局刷/全刷策略
 *
 * 【临界区】
 * 使用 FreeRTOS portMUX_TYPE 保护事件队列的并发访问。
 */

#include "../App_UiPort.h"

#include "freertos/FreeRTOS.h"

/** UI 事件队列临界区锁 */
static portMUX_TYPE s_ui_queue_mux = portMUX_INITIALIZER_UNLOCKED;

bool App_UiPort_Init(void)
{
    /* TODO: 在此初始化墨水屏显示驱动和输入设备 */
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    /* TODO: 实现墨水屏局刷/全刷切换 */
    (void)full_refresh;
}

void App_UiPort_EnterCritical(void)
{
    /* 进入 FreeRTOS 临界区 */
    portENTER_CRITICAL(&s_ui_queue_mux);
}

void App_UiPort_ExitCritical(void)
{
    /* 退出 FreeRTOS 临界区 */
    portEXIT_CRITICAL(&s_ui_queue_mux);
}
