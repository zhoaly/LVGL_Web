/**
 * @file App_UiPageNetwork.c
 * @brief Network 菜单跳转测试页面。
 */

#include "App_UiPageNetwork.h"

#include "lvgl.h"

#include "../../assets/App_UiAssets.h"
#include "../../assets/App_UiTheme.h"

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_t *description;
    lv_obj_t *icon;
    lv_obj_t *icon_box;
    lv_obj_t *status;
    lv_obj_t *status_label;
    (void)model;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(parent, 10, 0);

    icon_box = lv_obj_create(parent);
    lv_obj_remove_style_all(icon_box);
    lv_obj_set_size(icon_box, 44, 44);
    lv_obj_set_style_radius(icon_box, 14, 0);
    lv_obj_set_style_bg_color(
        icon_box,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_BACKGROUND),
        0);
    lv_obj_set_style_bg_opa(icon_box, LV_OPA_COVER, 0);

    icon = lv_image_create(icon_box);
    lv_image_set_src(icon, App_UiAssets_GetIcon(APP_UI_ICON_MENU_NETWORK));
    lv_obj_set_style_image_recolor(
        icon,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_FOREGROUND),
        0);
    lv_obj_set_style_image_recolor_opa(icon, LV_OPA_COVER, 0);
    lv_obj_center(icon);

    description = lv_label_create(parent);
    lv_label_set_text(description, "Wi-Fi configuration");
    lv_obj_set_style_text_font(
        description, App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY), 0);
    lv_obj_set_style_text_color(
        description,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY),
        0);

    status = lv_obj_create(parent);
    lv_obj_remove_style_all(status);
    lv_obj_set_size(status, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(status, 10, 0);
    lv_obj_set_style_bg_color(
        status,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_SURFACE_MUTED),
        0);
    lv_obj_set_style_bg_opa(status, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(status, 10, 0);
    lv_obj_set_style_pad_ver(status, 5, 0);

    status_label = lv_label_create(status);
    lv_label_set_text(status_label, "Not configured");
    lv_obj_set_style_text_font(
        status_label, App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY), 0);
    lv_obj_set_style_text_color(
        status_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED),
        0);
}

static void refresh(const app_ui_model_t *model)
{
    (void)model;
}

const app_ui_page_t *App_UiPageNetwork_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_NETWORK,
        .title = "Network",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = true,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
