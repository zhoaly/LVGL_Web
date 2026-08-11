/**
 * @file App_UiPageHome.c
 * @brief Home 页面主体；屏幕级状态栏和菜单抽屉由 View 持有。
 */

#include "App_UiPageHome.h"

#include "lvgl/lvgl.h"

#include "../../assets/App_UiTheme.h"
#include "../../components/App_UiComponents.h"
#include "../../components/motion/App_UiMotion.h"

static app_ui_action_binding_t s_text_page_binding;
static lv_obj_t *s_ready_label;
static lv_obj_t *s_open_button;

static void home_root_delete_cb(lv_event_t *event)
{
    (void)event;
    s_ready_label = NULL;
    s_open_button = NULL;
}

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    lv_obj_t *body;
    lv_obj_t *open_button;
    lv_obj_t *open_button_label;
    lv_obj_t *page_root;
    lv_obj_t *ready_label;
    (void)model;

    page_root = lv_obj_create(parent);
    lv_obj_remove_style_all(page_root);
    lv_obj_set_width(page_root, LV_PCT(100));
    lv_obj_set_flex_grow(page_root, 1);
    lv_obj_set_flex_flow(page_root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(page_root, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_event_cb(page_root,
                        home_root_delete_cb,
                        LV_EVENT_DELETE,
                        NULL);

    body = lv_obj_create(page_root);
    lv_obj_remove_style_all(body);
    lv_obj_set_width(body, LV_PCT(100));
    lv_obj_set_flex_grow(body, 1);
    lv_obj_set_flex_flow(body, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(body, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(body, 8, 0);

    ready_label = lv_label_create(body);
    s_ready_label = ready_label;
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
    s_open_button = open_button;
    lv_obj_remove_style_all(open_button);
    lv_obj_set_size(open_button, 132, 40);
    lv_obj_set_style_radius(open_button, 12, 0);
    lv_obj_set_style_bg_color(
        open_button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_BACKGROUND),
        0);
    lv_obj_set_style_bg_opa(open_button, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(
        open_button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_PRESSED),
        LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(open_button, LV_OPA_COVER, LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(open_button, 8, 0);
    lv_obj_set_style_shadow_color(
        open_button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_NAV_SHADOW),
        0);
    lv_obj_set_style_shadow_opa(open_button, LV_OPA_20, 0);
    lv_obj_set_style_shadow_offset_y(open_button, 3, 0);
    App_UiComponent_ApplyFocusStyle(
        open_button, APP_UI_COMPONENT_FOCUS_DARK);

    open_button_label = lv_label_create(open_button);
    lv_label_set_text(open_button_label, "Open Page");
    lv_obj_set_style_text_font(
        open_button_label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_EMPHASIS),
        0);
    lv_obj_set_style_text_color(
        open_button_label,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_FOREGROUND),
        0);
    lv_obj_center(open_button_label);

    App_UiComponent_InitAction(&s_text_page_binding,
                               APP_ACTION_ID_UI_NAV_PUSH,
                               APP_UI_PAGE_TEXT);
    App_UiComponent_BindAction(open_button, &s_text_page_binding);
}

static void enter(app_ui_page_transition_t transition)
{
    if(transition == APP_UI_PAGE_TRANSITION_INITIAL) {
        return;
    }

    App_UiMotion_AnimateEnter(
        s_ready_label, APP_UI_MOTION_OPACITY_TEXT, 4, 20);
    App_UiMotion_AnimateEnter(
        s_open_button, APP_UI_MOTION_OPACITY_BACKGROUND, 4, 40);
}

static void refresh(const app_ui_model_t *model)
{
    (void)model;
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
        .enter = enter,
    };
    return &page;
}
