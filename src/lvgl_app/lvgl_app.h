#ifndef LVGL_APP_H
#define LVGL_APP_H

/**
 * @file lvgl_app.h
 * @brief 兼容层入口声明 —— 与旧代码保持二进制兼容。
 *
 * 【设计说明】
 * 此文件是旧版 UI 入口 lvgl_app_init() 的声明头文件，
 * 仅为尚未迁移的旧代码提供向后兼容。
 *
 * 新项目应直接使用 include/App_Ui.h 中的 API，
 * 该框架采用 MVC 架构，职责划分更清晰。
 *
 * 【典型用法】
 *   1. 初始化 LVGL 和显示/输入驱动（平台相关）
 *   2. 调用 lvgl_app_init()（内部转发到 App_UiInit() + App_UiStart()）
 *   3. 在主循环中定期调用 lv_timer_handler() 驱动 LVGL 任务
 *
 * 【迁移路径】
 *   旧代码：main.c 中调用 lvgl_app_init()
 *   新代码：直接调用 App_UiInit() + App_UiStart()
 */

/**
 * @brief 初始化 UI 应用程序
 *
 * 内部依次调用：
 *   - App_UiInit()：初始化 Model、Nav、View、Port 和事件队列
 *   - App_UiStart()：显示首页并启动事件处理
 *
 * @note 此函数必须在 lv_init() 之后、lv_timer_handler() 循环之前调用
 */
void lvgl_app_init(void);

/**
 * @brief 反初始化 UI 应用程序，释放资源
 *
 * @note 当前为预留接口，框架为常驻模式暂不释放资源
 */
void lvgl_app_deinit(void);

#endif /* LVGL_APP_H */
