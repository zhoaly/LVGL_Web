/**
 * @file lvgl_app.c
 * @brief 兼容门面层（Facade）：将旧入口 lvgl_app_init() 映射到新 App_Ui 框架。
 *
 * 【设计模式】门面模式（Facade Pattern）
 * 此文件作为旧版入口与新框架之间的适配层，隐藏了 App_Ui 框架的初始化细节。
 *
 * 使用旧入口的文件（如 main.c 中调用 ui_init() 或 lvgl_app_init()）
 * 无需修改即可自动使用新框架。新代码应直接使用 include/App_Ui.h 中的 API。
 */

#include "lvgl_app.h"

#include "include/App_Ui.h"

void lvgl_app_init(void)
{
    /* 初始化 UI 模块（Model/Nav/View/Port）并启动首页显示 */
    (void)App_UiInit();
    (void)App_UiStart();
}

void lvgl_app_deinit(void)
{
    /*
     * 框架当前为常驻模式，此接口保留供后续释放资源。
     * 如需完整的生命周期管理，后续可在此处添加：
     *   - 停止 LVGL 定时器
     *   - 销毁所有 LVGL 对象
     *   - 释放平台相关资源
     */
}
