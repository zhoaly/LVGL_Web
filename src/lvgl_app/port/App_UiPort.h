/**
 * @file App_UiPort.h
 * @brief 平台适配层（Port Abstraction Layer）头文件。
 *
 * 【设计模式】适配器模式（Adapter Pattern）
 * Port 层封装平台相关实现，提供统一上层接口。
 *
 * 【平台差异】
 *   - PC（浏览器）：SDL 显示，无需临界区保护
 *   - ESP32：墨水屏驱动，FreeRTOS 临界区保护
 */

#ifndef APP_UI_PORT_H
#define APP_UI_PORT_H

#include <stdbool.h>

/**
 * @brief 初始化平台相关资源
 * @return true 初始化成功
 */
bool App_UiPort_Init(void);

/**
 * @brief 请求屏幕刷新
 * @param full_refresh true=全屏刷新，false=局部刷新
 */
void App_UiPort_RequestFlush(bool full_refresh);

/**
 * @brief 进入临界区（保护事件队列的并发访问）
 */
void App_UiPort_EnterCritical(void);

/**
 * @brief 退出临界区
 */
void App_UiPort_ExitCritical(void);

#endif /* APP_UI_PORT_H */
