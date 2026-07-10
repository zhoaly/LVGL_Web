/**
 * @file App_UiPort_Pc.c
 * @brief PC 平台适配实现。
 *
 * PC 端使用 SDL 作为 LVGL 显示后端，通过鼠标/键盘操作。
 * 此实现包含一个模拟事件发生器，在没有真实硬件的情况下
 * 定期向 UI 注入模拟的心跳事件和堆变化事件，以演示 UI 的自动刷新功能。
 *
 * 此文件不包含任何 ESP-IDF 或 FreeRTOS 头文件，在标准 C 编译器下可直接编译。
 */

#include "../App_UiPort.h"

#include "lvgl/lvgl.h"

/** 模拟事件发生器静态变量 */
static app_ui_port_emit_fn s_emit_fn;       /**< 事件发射回调 */
static void *s_emit_user;                   /**< 回调用户数据 */
static lv_timer_t *s_sim_timer;             /**< 模拟定时器 */
static uint32_t s_tick;                     /**< 运行计数 */

/**
 * @brief 模拟定时器回调：每秒执行一次。
 * 每次触发发送 APP_UI_EVENT_RUNTIME_TICK 事件（更新时间计数），
 * 每 5 秒额外发送一次 APP_UI_EVENT_HEAP_UPDATED（模拟堆变化）。
 */
static void sim_timer_cb(lv_timer_t *timer)
{
    app_ui_event_t event;
    (void)timer;

    if(s_emit_fn == NULL) {
        return;
    }

    /* 每秒发送心跳事件 */
    event.type = APP_UI_EVENT_RUNTIME_TICK;
    event.a = 0;
    event.b = 0;
    event.text[0] = '\0';
    s_emit_fn(&event, s_emit_user);

    /* 每 5 秒模拟堆内存波动 */
    if((s_tick % 5u) == 0u) {
        event.type = APP_UI_EVENT_HEAP_UPDATED;
        event.a = 320 - (int32_t)(s_tick % 30u);
        event.b = 280;
        s_emit_fn(&event, s_emit_user);
    }

    s_tick++;
}

bool App_UiPort_Init(void)
{
    /* PC 端不需要额外的平台初始化 */
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    /* PC 端由 LVGL 自动处理刷新，无需额外操作 */
    (void)full_refresh;
}

void App_UiPort_StartSimulation(app_ui_port_emit_fn emit_fn, void *user_data)
{
    s_emit_fn = emit_fn;
    s_emit_user = user_data;

    /* 创建 1 秒定时器驱动模拟事件 */
    if(s_sim_timer == NULL) {
        s_sim_timer = lv_timer_create(sim_timer_cb, 1000, NULL);
    }
}

void App_UiPort_EnterCritical(void)
{
}

void App_UiPort_ExitCritical(void)
{
}
