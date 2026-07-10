#ifndef LVGL_APP_H
#define LVGL_APP_H

/**
 * @file lvgl_app.h
 * @brief 可移植的 LVGL 应用入口 — 与平台无关，可在 Web(SDL) 和 ESP32 间共用。
 *
 * 用法：
 *   1. 初始化 LVGL 和显示/输入驱动（平台相关）
 *   2. 调用 lvgl_app_init()
 *   3. 定期调用 lv_timer_handler()
 */

void lvgl_app_init(void);

#endif /* LVGL_APP_H */
