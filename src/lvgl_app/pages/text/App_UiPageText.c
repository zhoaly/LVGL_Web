/**
 * @file App_UiPageText.c
 * @brief 只显示文字的简单页面。
 */

#include "App_UiPageText.h"

#include "lvgl/lvgl.h"

#include "../../assets/App_UiTheme.h"

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_t *label;
    (void)model;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    label = lv_label_create(parent);
    lv_label_set_text(label, "This is a simple page.");
    lv_obj_set_style_text_font(
        label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_EMPHASIS),
        0);
    lv_obj_set_style_text_color(
        label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY),
        0);
}

static void refresh(const app_ui_model_t *model)
{
    (void)model;
}

const app_ui_page_t *App_UiPageText_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_TEXT,
        .title = "Text Page",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = true,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
