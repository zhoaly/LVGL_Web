/**
 * @file App_UiPort_Pc.c
 * @brief PC 平台适配实现 —— 支持 SDL 显示后端和模拟事件发生器。
 *
 * 【编译环境】
 * 此文件不包含任何 ESP-IDF 或 FreeRTOS 头文件，
 * 在标准 C 编译器（如 MSVC、GCC）下可直接编译。
 *
 * 【功能说明】
 * PC 端适配层提供两个关键功能：
 *
 *   1. 【显示/输入】
 *      通过 LVGL 的 SDL 驱动实现显示和鼠标/键盘输入。
 *      SDL 的初始化在 lvgl 库内部完成，此层无需额外操作。
 *
 *   2. 【模拟事件发生器】
 *      由于 PC 端没有真实的 USB/WiFi 硬件，
 *      App_UiPort_StartSimulation() 会创建一个 LVGL 定时器，
 *      定期注入模拟的业务事件：
 *        - 每秒：APP_UI_EVENT_RUNTIME_TICK（运行时间心跳）
 *        - 每 5 秒：APP_UI_EVENT_HEAP_UPDATED（堆内存波动）
 *
 * 【临界区】
 * PC 端为单线程 LVGL 任务，不需要临界区保护，
 * EnterCritical/ExitCritical 为空操作。
 */

#include "../App_UiPort.h"

#include "lvgl/lvgl.h"

/**
 * 模拟事件发生器静态变量
 */
static app_ui_port_emit_fn s_emit_fn;       /**< 事件发射回调函数指针 */
static void *s_emit_user;                   /**< 回调用户自定义数据 */
static lv_timer_t *s_sim_timer;             /**< 模拟事件 LVGL 定时器 */
static uint32_t s_tick;                     /**< 运行计数（自启动以来的秒数） */

/**
 * @brief 模拟事件定时器回调函数
 *
 * 每秒由 LVGL 定时器触发一次，执行以下操作：
 *   1. 发送 APP_UI_EVENT_RUNTIME_TICK 事件（每秒递增运行时间）
 *   2. 每 5 秒额外发送 APP_UI_EVENT_HEAP_UPDATED 事件
 *      （模拟堆内存的波动变化，演示 UI 自动刷新效果）
 *
 * @param timer 触发此回调的 LVGL 定时器指针（未使用）
 */
static void sim_timer_cb(lv_timer_t *timer)
{
    app_ui_event_t event;
    (void)timer;

    /* 检查回调是否已注册 */
    if(s_emit_fn == NULL) {
        return;
    }

    /* ---- 每秒发送心跳事件 ---- */
    event.type = APP_UI_EVENT_RUNTIME_TICK;
    event.a = 0;
    event.b = 0;
    event.text[0] = '\0';
    s_emit_fn(&event, s_emit_user);

    /* ---- 每 5 秒模拟堆内存波动 ---- */
    if((s_tick % 5u) == 0u) {
        event.type = APP_UI_EVENT_HEAP_UPDATED;
        event.a = 320 - (int32_t)(s_tick % 30u);  /* 空闲堆在 290~320KB 间波动 */
        event.b = 280;                              /* 最小堆保持在 280KB */
        s_emit_fn(&event, s_emit_user);
    }

    s_tick++;
}

bool App_UiPort_Init(void)
{
    /* PC 端不需要额外的平台初始化操作（SDL 由 LVGL 库内部初始化） */
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    /*
     * PC 端由 LVGL 的 SDL 驱动自动处理屏幕刷新，
     * 无需软件触发刷新操作。
     */
    (void)full_refresh;
}

void App_UiPort_StartSimulation(app_ui_port_emit_fn emit_fn, void *user_data)
{
    s_emit_fn = emit_fn;
    s_emit_user = user_data;

    /* 创建 1 秒周期的 LVGL 定时器，驱动模拟事件发生器 */
    if(s_sim_timer == NULL) {
        s_sim_timer = lv_timer_create(sim_timer_cb, 1000, NULL);
    }
}

void App_UiPort_EnterCritical(void)
{
    /* PC 端单线程环境下无需临界区保护 */
}

void App_UiPort_ExitCritical(void)
{
    /* PC 端单线程环境下无需临界区保护 */
}
