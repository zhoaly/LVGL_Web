/**
 * @file App_UiTheme.h
 * @brief UI 主题语义层。集中管理颜色和字体角色。
 */

#ifndef APP_UI_THEME_H
#define APP_UI_THEME_H

#include "lvgl/lvgl.h"

typedef enum {
    APP_UI_THEME_COLOR_SCREEN_BACKGROUND = 0,
    APP_UI_THEME_COLOR_ACCENT,
    APP_UI_THEME_COLOR_TEXT_PRIMARY,
    APP_UI_THEME_COLOR_TEXT_MUTED,
    APP_UI_THEME_COLOR_NAV_BACKGROUND,
    APP_UI_THEME_COLOR_NAV_FOREGROUND,
    APP_UI_THEME_COLOR_NAV_PRESSED_FOREGROUND,
    APP_UI_THEME_COLOR_NAV_SHADOW,
    APP_UI_THEME_COLOR_COUNT,
} app_ui_theme_color_id_t;

typedef enum {
    APP_UI_THEME_FONT_BODY = 0,
    APP_UI_THEME_FONT_EMPHASIS,
    APP_UI_THEME_FONT_TITLE,
    APP_UI_THEME_FONT_COUNT,
} app_ui_theme_font_id_t;

lv_color_t App_UiTheme_GetColor(app_ui_theme_color_id_t id);
const lv_font_t *App_UiTheme_GetFont(app_ui_theme_font_id_t id);

#endif /* APP_UI_THEME_H */
