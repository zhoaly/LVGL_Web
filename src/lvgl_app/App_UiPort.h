/**
 * @file App_UiPort.h
 * @brief 平台适配层（Port Abstraction Layer）抽象头文件。
 *
 * 【设计模式】适配器模式（Adapter Pattern）
 *
 * Port 层封装了所有平台相关的实现差异，提供统一的上层接口：
 *
 * ┌──────────────────────────────────────────────┐
 * │  上层代码（Pages / Controller / View）        │
 * │  不包含任何平台特定头文件                      │
 * ├──────────────────────────────────────────────┤
 * │           App_UiPort.h（统一接口）              │
 * ├──────────────────┬───────────────────────────┤
 * │ App_UiPort_Pc.c  │ App_UiPort_Esp32.c        │
 * │ (SDL + 模拟事件) │ (墨水屏 + 编码器)          │
 * └──────────────────┴───────────────────────────┘
 *
 * 【平台差异】
 *   - PC (Windows/Linux)：SDL 显示，鼠标/键盘，模拟事件注入
 *   - ESP32：墨水屏驱动，编码器输入，真实业务事件
 */

#ifndef APP_UI_PORT_H
#define APP_UI_PORT_H

#include <stdbool.h>

#include "include/App_Ui.h"

/**
 * @brief 端口层事件发射函数类型
 *
 * 用于端口层向 UI 总控投递模拟事件（如 PC 端的测试事件发生器）。
 *
 * @param event 要投递的事件指针
 * @param user_data 用户自定义数据
 * @return true 投递成功
 */
typedef bool (*app_ui_port_emit_fn)(const app_ui_event_t *event, void *user_data);

/**
 * @brief 初始化平台相关资源
 *
 * 具体行为取决于平台：
 *   - PC：初始化 SDL 窗口和输入设备
 *   - ESP32：初始化墨水屏显示驱动和编码器输入
 *
 * @return true 初始化成功
 * @return false 初始化失败
 */
bool App_UiPort_Init(void);

/**
 * @brief 请求屏幕刷新
 *
 * 【平台差异】
 *   - PC 端：直接由 LVGL 驱动刷新，此操作为空操作
 *   - ESP32 墨水屏端：根据 full_refresh 决定执行全刷还是局刷
 *
 * @param full_refresh true=全屏刷新（消除残影），false=局部刷新（更快但可能留残影）
 */
void App_UiPort_RequestFlush(bool full_refresh);

/**
 * @brief 启动模拟事件发生器（仅 PC 端有效）
 *
 * PC 端启动一个 LVGL 定时器，定期注入模拟的业务事件
 * （如心跳 APP_UI_EVENT_RUNTIME_TICK、堆变化 APP_UI_EVENT_HEAP_UPDATED），
 * 以便在没有真实硬件的情况下测试 UI 的响应和自动刷新逻辑。
 *
 * @param emit_fn 事件发射回调函数
 * @param user_data 回调用户自定义数据
 */
void App_UiPort_StartSimulation(app_ui_port_emit_fn emit_fn, void *user_data);

/**
 * @brief 进入临界区，保护 UI 事件队列的并发访问
 *
 * 当事件生产者在 LVGL 任务之外运行（如中断服务程序或其他 RTOS 任务）
 * 时，需要调用此函数保护队列操作的原子性。
 */
void App_UiPort_EnterCritical(void);

/**
 * @brief 退出临界区
 */
void App_UiPort_ExitCritical(void);

#endif /* APP_UI_PORT_H */
