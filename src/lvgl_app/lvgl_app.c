/**
 * @file lvgl_app.c
 * @brief 兼容门面层：将旧入口 lvgl_app_init() 映射到新 App_Ui 框架。
 *
 * 此文件用于保持向后兼容。
 * 使用旧入口的文件（如 main.c 中调用 ui_init() 或 lvgl_app_init()）
 * 无需修改即可自动使用新框架。新代码应直接使用 include/App_Ui.h 中的 API。
 */

#include "lvgl_app.h"

#include "include/App_Ui.h"

void lvgl_app_init(void)
{
    /* 初始化 UI 模块并启动首页 */
    (void)App_UiInit();
    (void)App_UiStart();
}

void lvgl_app_deinit(void)
{
    /* 框架当前为常驻模式，此接口保留供后续释放资源 */
}
