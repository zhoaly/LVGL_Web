/**
 * @file ui.h
 * @brief 旧 UI 兼容头文件。
 * @deprecated 请直接使用 lvgl_app/lvgl_app.h 中的 lvgl_app_init()。
 *             此文件保留仅为向后兼容，已将 ui_init 宏定义为 lvgl_app_init。
 */

#ifndef LVGL_WEB_UI_H
#define LVGL_WEB_UI_H

#include "lvgl_app/lvgl_app.h"

/** 旧版 API 宏重定向到新版兼容入口 */
#define ui_init lvgl_app_init

#endif /* LVGL_WEB_UI_H */

