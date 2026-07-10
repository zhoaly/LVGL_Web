#ifndef LVGL_APP_H
#define LVGL_APP_H

/**
 * @file lvgl_app.h
 * @brief 兼容层入口声明 —— 与旧代码保持二进制兼容。
 *
 * 新项目应直接使用 include/App_Ui.h 中的 API，
 * 此文件保留仅供尚未迁移的旧代码引用。
 *
 * 用法：
 *   1. 初始化 LVGL 和显示/输入驱动（平台相关）
 *   2. 调用 lvgl_app_init()（内部转发到 App_UiInit() + App_UiStart()）
 *   3. 定期调用 lv_timer_handler()
 */

void lvgl_app_init(void);
void lvgl_app_deinit(void);

#endif /* LVGL_APP_H */
