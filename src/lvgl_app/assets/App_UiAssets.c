/**
 * @file App_UiAssets.c
 * @brief 将稳定的语义资源 ID 映射到自动生成的 LVGL 资源。
 */

#include "App_UiAssets.h"

#include "generated/App_UiIcons.h"

const lv_image_dsc_t *App_UiAssets_GetIcon(app_ui_icon_id_t id)
{
    static const lv_image_dsc_t *const icons[APP_UI_ICON_COUNT] = {
        [APP_UI_ICON_NAV_BACK] = &app_ui_icon_nav_back,
        [APP_UI_ICON_NAV_HOME] = &app_ui_icon_nav_home,
    };

    if(id < 0 || id >= APP_UI_ICON_COUNT) {
        return NULL;
    }
    return icons[id];
}

const lv_font_t *App_UiAssets_GetFont(app_ui_font_asset_id_t id)
{
    static const lv_font_t *const fonts[APP_UI_FONT_COUNT] = {
        [APP_UI_FONT_BODY_14] = &lv_font_montserrat_14,
        [APP_UI_FONT_BODY_16] = &lv_font_montserrat_16,
    };

    if(id < 0 || id >= APP_UI_FONT_COUNT) {
        return LV_FONT_DEFAULT;
    }
    return fonts[id];
}
