/**
 * @file App_UiPort_Esp32.c
 * @brief ESP32 平台适配层实现（骨架代码）。
 *
 * 【编译环境】
 * 此文件在 ESP-IDF 环境下编译，使用 FreeRTOS API。
 *
 * 【当前状态】
 * 当前为初始骨架，实际移植到具体硬件时需要完成：
 *
 *   - 【显示驱动】初始化墨水屏/SPI 显示驱动并关联到 LVGL
 *     （如使用 lvgl_port_... 或 esp_lcd 框架）
 *
 *   - 【输入设备】初始化编码器/触摸输入并映射为 LVGL 输入设备
 *     （如使用 esp_touch 或 encoder 驱动）
 *
 *   - 【刷新策略】实现 RequestFlush 的局刷/全刷切换逻辑：
 *     全刷（full_refresh=true）：执行全屏刷新，用于消除残影
 *     局刷（full_refresh=false）：仅刷新变化区域，速度更快
 *
 *   - 【事件来源】不启用模拟事件发生器，
 *     真实业务模块会通过 App_UiPostEvent() 发送事件
 *
 * 【临界区保护】
 * 使用 FreeRTOS 的 portMUX_TYPE 实现临界区，
 * 保护 UI 事件队列在多任务环境下的并发访问。
 */

#include "../App_UiPort.h"

#include "freertos/FreeRTOS.h"

/** UI 事件队列临界区保护锁 */
static portMUX_TYPE s_ui_queue_mux = portMUX_INITIALIZER_UNLOCKED;

bool App_UiPort_Init(void)
{
    /* TODO: 在此初始化墨水屏显示驱动和编码器输入设备 */
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    /*
     * TODO: 墨水屏刷新策略实现
     *   full_refresh = true  -> 全屏刷新（清除残影）
     *   full_refresh = false -> 局部增量刷新（更快）
     */
    (void)full_refresh;
}

void App_UiPort_StartSimulation(app_ui_port_emit_fn emit_fn, void *user_data)
{
    /*
     * ESP32 端不需要模拟事件发生器。
     * 真实业务模块（USB、WiFi 等）会通过 App_UiPostEvent() 主动发送事件。
     */
    (void)emit_fn;
    (void)user_data;
}

void App_UiPort_EnterCritical(void)
{
    /* 进入 FreeRTOS 临界区，保护共享资源（事件队列） */
    portENTER_CRITICAL(&s_ui_queue_mux);
}

void App_UiPort_ExitCritical(void)
{
    /* 退出 FreeRTOS 临界区 */
    portEXIT_CRITICAL(&s_ui_queue_mux);
}
