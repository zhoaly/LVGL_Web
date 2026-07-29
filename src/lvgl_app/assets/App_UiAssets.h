/**
 * @file App_UiAssets.h
 * @brief UI 资源注册层。页面和组件只通过语义 ID 获取图标与字体。
 */

#ifndef APP_UI_ASSETS_H
#define APP_UI_ASSETS_H

#include "lvgl/lvgl.h"

typedef enum {
    APP_UI_ICON_NAV_BACK = 0,
    APP_UI_ICON_NAV_HOME,
    APP_UI_ICON_STATUS_MENU,
    APP_UI_ICON_STATUS_WIFI,
    APP_UI_ICON_STATUS_BLUETOOTH,
    APP_UI_ICON_COUNT,
} app_ui_icon_id_t;

typedef enum {
    APP_UI_FONT_BODY_14 = 0,
    APP_UI_FONT_BODY_16,
    APP_UI_FONT_COUNT,
} app_ui_font_asset_id_t;

/**
 * @brief 获取编译期图标资源
 * @return 对应的 LVGL 图片描述符，ID 无效时返回 NULL
 */
const lv_image_dsc_t *App_UiAssets_GetIcon(app_ui_icon_id_t id);

/**
 * @brief 获取编译期字体资源
 * @return 对应的 LVGL 字体，ID 无效时返回 LV_FONT_DEFAULT
 */
const lv_font_t *App_UiAssets_GetFont(app_ui_font_asset_id_t id);

#endif /* APP_UI_ASSETS_H */
