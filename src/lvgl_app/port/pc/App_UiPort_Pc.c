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

static lv_indev_t *s_encoder_indev;

bool App_UiPort_Init(void)
{
    /* PC 端由 LVGL 的 SDL 驱动自动初始化，无需额外操作 */
    return true;
}

void App_UiPort_Deinit(void)
{
    s_encoder_indev = NULL;
}

bool App_UiPort_Lock(uint32_t timeout_ms)
{
    (void)timeout_ms;
    return true;
}

void App_UiPort_Unlock(void)
{
}

bool App_UiPort_Present(void)
{
    lv_refr_now(NULL);
    return true;
}

bool App_UiPort_SetInputGroup(lv_group_t *group)
{
    if(group == NULL) {
        return false;
    }
    if(s_encoder_indev != NULL) {
        lv_indev_set_group(s_encoder_indev, group);
    }
    return true;
}

bool App_UiPort_SetInputAvailable(bool available)
{
    (void)available;
    return true;
}

bool App_UiPort_BindCommandDispatcher(
    app_ui_command_submitter_fn dispatcher,
    void *user_data)
{
    return App_UiCommand_SetSubmitter(dispatcher, user_data);
}

void App_UiPort_PcSetEncoderIndev(lv_indev_t *indev)
{
    s_encoder_indev = indev;
}

void App_UiPort_EnterCritical(void)
{
    /* 单线程环境下无需临界区保护 */
}

void App_UiPort_ExitCritical(void)
{
    /* 单线程环境下无需临界区保护 */
}
