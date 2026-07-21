#include "App_UiPageHome.h"

#include "lvgl/lvgl.h"

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_t *ready_label;
    (void)model;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    ready_label = lv_label_create(parent);
    lv_label_set_text(ready_label, "Ready");
    lv_obj_set_style_text_font(ready_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(ready_label, lv_color_hex(0x4A5568), 0);
}

static void refresh(const app_ui_model_t *model)
{
    (void)model;
}

const app_ui_page_t *App_UiPageHome_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_HOME,
        .title = "LVGL App",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = false,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
