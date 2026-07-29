/**
 * @file App_UiPageHome.c
 * @brief Home 页面：组合可复用组件，不在页面内实现组件细节。
 */

#include "App_UiPageHome.h"

#include "lvgl/lvgl.h"

#include "../../assets/App_UiTheme.h"
#include "../../components/App_UiComponents.h"
#include "../../components/status_bar/App_UiStatusBar.h"

static app_ui_action_binding_t s_text_page_binding;
static app_ui_status_bar_t s_status_bar;
static const app_ui_status_bar_state_t s_status_bar_state = {
    .time_text = "--:--",
    .time_synced = false,
    .weather_text = "--C",
    .weather_available = false,
    .wifi_state = APP_UI_STATUS_WIFI_DISCONNECTED,
    .bluetooth_state = APP_UI_STATUS_BLUETOOTH_OFF,
};

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_t *body;
    lv_obj_t *open_button;
    lv_obj_t *open_button_label;
    lv_obj_t *ready_label;
    (void)model;

    /* Home 只组合组件；状态栏自行负责内部布局和视觉状态。 */
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    (void)App_UiStatusBar_Create(
        parent, &s_status_bar, &s_status_bar_state, NULL);

    /* 主体填充状态栏下方空间，后续端口卡片组件在此区域组合。 */
    body = lv_obj_create(parent);
    lv_obj_remove_style_all(body);
    lv_obj_set_width(body, LV_PCT(100));
    lv_obj_set_flex_grow(body, 1);
    lv_obj_set_flex_flow(body, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(body, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(body, 8, 0);

    ready_label = lv_label_create(body);
    lv_label_set_text(ready_label, "Ready");
    lv_obj_set_style_text_font(
        ready_label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_EMPHASIS),
        0);
    lv_obj_set_style_text_color(
        ready_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_PRIMARY),
        0);

    open_button = lv_button_create(body);
    open_button_label = lv_label_create(open_button);
    lv_label_set_text(open_button_label, "Open Page");
    lv_obj_center(open_button_label);

    App_UiComponent_InitAction(&s_text_page_binding,
                               APP_ACTION_ID_UI_NAV_PUSH,
                               APP_UI_PAGE_TEXT);
    App_UiComponent_BindAction(open_button, &s_text_page_binding);
}

static void refresh(const app_ui_model_t *model)
{
    (void)model;
    App_UiStatusBar_Update(&s_status_bar, &s_status_bar_state);
}

const app_ui_page_t *App_UiPageHome_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_HOME,
        .title = "",
        .dirty_mask = APP_UI_DIRTY_NONE,
        .show_back = false,
        .build = build,
        .refresh = refresh,
    };
    return &page;
}
