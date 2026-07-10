/**
 * @file App_UiPort_Esp32.c
 * @brief ESP32 平台适配骨架实现。
 *
 * 此文件在 ESP-IDF 环境下编译，接入真实的墨水屏显示驱动和编码器输入。
 * 当前为初始骨架，实际移植时需要：
 *   - 初始化墨水屏显示驱动并关联到 LVGL
 *   - 初始化编码器输入设备并映射为 LVGL 编码器输入
 *   - 实现 RequestFlush 的局刷/全刷策略
 *   - 不启用模拟事件发生器（真实业务模块会通过 App_UiPostEvent 发送事件）
 */

#include "../App_UiPort.h"

#include "freertos/FreeRTOS.h"

static portMUX_TYPE s_ui_queue_mux = portMUX_INITIALIZER_UNLOCKED;

bool App_UiPort_Init(void)
{
    /* TODO: 在此初始化墨水屏驱动和编码器输入 */
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    /* TODO: 墨水屏刷新策略：局刷和全刷切换 */
    (void)full_refresh;
}

void App_UiPort_StartSimulation(app_ui_port_emit_fn emit_fn, void *user_data)
{
    /* ESP32 端不需要模拟事件，由真实业务模块通过 App_UiPostEvent 驱动 */
    (void)emit_fn;
    (void)user_data;
}

void App_UiPort_EnterCritical(void)
{
    portENTER_CRITICAL(&s_ui_queue_mux);
}

void App_UiPort_ExitCritical(void)
{
    portEXIT_CRITICAL(&s_ui_queue_mux);
}
