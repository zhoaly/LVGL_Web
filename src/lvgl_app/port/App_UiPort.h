/**
 * @file App_UiPort.h
 * @brief 平台适配层（Port Abstraction Layer）头文件。
 *
 * 【设计模式】适配器模式（Adapter Pattern）
 * Port 层封装平台相关实现，提供统一上层接口。
 *
 * 【平台差异】
 *   - PC（浏览器）：SDL 显示，无需临界区保护
 *   - ESP32：具体显示驱动、LVGL 任务锁和输入设备
 */

#ifndef APP_UI_PORT_H
#define APP_UI_PORT_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl/lvgl.h"

#include "../command/App_UiCommand.h"

/**
 * @brief 初始化平台相关资源
 * @return true 初始化成功
 */
bool App_UiPort_Init(void);

void App_UiPort_Deinit(void);
bool App_UiPort_Lock(uint32_t timeout_ms);
void App_UiPort_Unlock(void);
bool App_UiPort_Present(void);
bool App_UiPort_SetInputGroup(lv_group_t *group);
bool App_UiPort_SetInputAvailable(bool available);

/**
 * @brief 将 UI 内部命令处理器绑定到当前平台命令后端。
 */
bool App_UiPort_BindCommandDispatcher(
    app_ui_command_submitter_fn dispatcher,
    void *user_data);

/**
 * @brief 进入临界区（保护事件队列的并发访问）
 */
void App_UiPort_EnterCritical(void);

/**
 * @brief 退出临界区
 */
void App_UiPort_ExitCritical(void);

#endif /* APP_UI_PORT_H */
