/**
 * @file App_UiTheme.c
 * @brief 默认主题到资源层的映射。
 */

#include "App_UiTheme.h"

#include "App_UiAssets.h"

lv_color_t App_UiTheme_GetColor(app_ui_theme_color_id_t id)
{
    static const uint32_t colors[APP_UI_THEME_COLOR_COUNT] = {
        [APP_UI_THEME_COLOR_SCREEN_BACKGROUND] = 0xFAFBFD,
        [APP_UI_THEME_COLOR_ACCENT] = 0x2778FF,
        [APP_UI_THEME_COLOR_TEXT_PRIMARY] = 0x4A5568,
        [APP_UI_THEME_COLOR_TEXT_MUTED] = 0x718096,
        [APP_UI_THEME_COLOR_SURFACE_MUTED] = 0xEDF2F7,
        [APP_UI_THEME_COLOR_WARNING] = 0xD97706,
        [APP_UI_THEME_COLOR_NAV_BACKGROUND] = 0x111111,
        [APP_UI_THEME_COLOR_NAV_FOREGROUND] = 0xFFFFFF,
        [APP_UI_THEME_COLOR_NAV_PRESSED_FOREGROUND] = 0x111111,
        [APP_UI_THEME_COLOR_NAV_SHADOW] = 0x000000,
    };

    if(id < 0 || id >= APP_UI_THEME_COLOR_COUNT) {
        return lv_color_hex(0x000000);
    }
    return lv_color_hex(colors[id]);
}

const lv_font_t *App_UiTheme_GetFont(app_ui_theme_font_id_t id)
{
    static const app_ui_font_asset_id_t fonts[APP_UI_THEME_FONT_COUNT] = {
        [APP_UI_THEME_FONT_BODY] = APP_UI_FONT_BODY_14,
        [APP_UI_THEME_FONT_EMPHASIS] = APP_UI_FONT_BODY_16,
        [APP_UI_THEME_FONT_TITLE] = APP_UI_FONT_BODY_16,
    };

    if(id < 0 || id >= APP_UI_THEME_FONT_COUNT) {
        return LV_FONT_DEFAULT;
    }
    return App_UiAssets_GetFont(fonts[id]);
}
