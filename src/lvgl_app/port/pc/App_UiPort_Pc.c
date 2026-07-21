/**
 * @file App_UiPort_Pc.c
 * @brief PC 平台适配层实现（浏览器 Emscripten 环境）。
 *
 * 【编译环境】
 * 此文件通过 Emscripten 编译，使用 SDL 显示后端。
 * 不包含任何 ESP-IDF 或 FreeRTOS 头文件。
 *
 * 【说明】
 *   - Init：PC 端无需额外初始化
 *   - RequestFlush：LVGL SDL 驱动自动处理刷新
 *   - Enter/ExitCritical：单线程环境无需临界区保护
 */

#include "../App_UiPort.h"

bool App_UiPort_Init(void)
{
    /* PC 端由 LVGL 的 SDL 驱动自动初始化，无需额外操作 */
    return true;
}

void App_UiPort_RequestFlush(bool full_refresh)
{
    /* PC 端由 LVGL 自动处理屏幕刷新 */
    (void)full_refresh;
}

void App_UiPort_EnterCritical(void)
{
    /* 单线程环境下无需临界区保护 */
}

void App_UiPort_ExitCritical(void)
{
    /* 单线程环境下无需临界区保护 */
}
