/**
 * @file App_UiPort.h
 * @brief 平台适配层（Port）抽象头文件。
 *
 * Port 层封装了所有平台相关的实现差异：
 *   - PC (Windows/Linux)：使用 SDL 显示，鼠标/键盘输入，模拟事件注入
 *   - ESP32：使用墨水屏驱动，编码器输入，真实业务事件
 *
 * 页面代码和 UI 总控不包含任何平台特定的头文件，
 * 所有平台差异都隔离在此层之下。
 */

#ifndef APP_UI_PORT_H
#define APP_UI_PORT_H

#include <stdbool.h>

#include "include/App_Ui.h"

/**
 * @brief 端口层事件发射函数类型。
 * 用于端口层向 UI 总控投递模拟事件（如 PC 端的测试事件发生器）。
 * @param event 事件指针
 * @param user_data 用户数据
 * @return true 投递成功
 */
typedef bool (*app_ui_port_emit_fn)(const app_ui_event_t *event, void *user_data);

/**
 * @brief 初始化平台相关资源。
 * 在 PC 上可能初始化 SDL 窗口，在 ESP32 上初始化显示驱动和编码器。
 * @return true 初始化成功
 */
bool App_UiPort_Init(void);

/**
 * @brief 请求屏幕刷新。
 * PC 端直接由 LVGL 驱动刷新，此操作为空操作；
 * ESP32 墨水屏端根据 full_refresh 决定执行全刷还是局刷。
 * @param full_refresh true 请求全屏刷新（消除残影），false 局部刷新
 */
void App_UiPort_RequestFlush(bool full_refresh);

/**
 * @brief 启动模拟事件发生器（仅 PC 端有效）。
 * PC 端启动一个定时器，定期注入模拟的业务事件（如心跳、堆变化）
 * 以便在没有真实硬件的情况下测试 UI 的响应和刷新逻辑。
 * @param emit_fn 事件发射回调
 * @param user_data 回调用户数据
 */
void App_UiPort_StartSimulation(app_ui_port_emit_fn emit_fn, void *user_data);

/** Protect the UI queue when producers run outside the LVGL task. */
void App_UiPort_EnterCritical(void);
void App_UiPort_ExitCritical(void);

#endif /* APP_UI_PORT_H */
