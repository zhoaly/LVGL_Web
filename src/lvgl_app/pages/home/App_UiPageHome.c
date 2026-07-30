/**
 * @file App_UiPageHome.c
 * @brief Home 页面：组合顶部状态栏、菜单抽屉与页面主体。
 */

#include "App_UiPageHome.h"

#include <stdio.h>

#include "lvgl/lvgl.h"

#include "../../assets/App_UiTheme.h"
#include "../../components/App_UiComponents.h"
#include "../../components/menu_drawer/App_UiMenuDrawer.h"
#include "../../components/motion/App_UiMotion.h"
#include "../../components/status_bar/App_UiStatusBar.h"

static app_ui_action_binding_t s_text_page_binding;
static app_ui_status_bar_t s_status_bar;
static app_ui_menu_drawer_t s_menu_drawer;
static lv_obj_t *s_ready_label;
static lv_obj_t *s_open_button;

static const app_ui_menu_drawer_item_t s_menu_items[] = {
    {
        .icon_id = APP_UI_ICON_MENU_NETWORK,
        .label = "Network",
        .target_page = APP_UI_PAGE_NETWORK,
    },
    {
        .icon_id = APP_UI_ICON_MENU_HID_HUB,
        .label = "HID Hub",
        .target_page = APP_UI_PAGE_HID_HUB,
    },
    {
        .icon_id = APP_UI_ICON_MENU_SETTINGS,
        .label = "Settings",
        .target_page = APP_UI_PAGE_SETTINGS,
    },
};

static app_ui_status_wifi_state_t map_wifi_state(app_ui_wifi_state_t state)
{
    switch(state) {
    case APP_UI_WIFI_CONNECTING:
        return APP_UI_STATUS_WIFI_CONNECTING;
    case APP_UI_WIFI_CONNECTED:
        return APP_UI_STATUS_WIFI_CONNECTED;
    case APP_UI_WIFI_DISCONNECTED:
    default:
        return APP_UI_STATUS_WIFI_DISCONNECTED;
    }
}

static app_ui_status_bluetooth_state_t map_bluetooth_state(
    app_ui_bluetooth_state_t state)
{
    switch(state) {
    case APP_UI_BLUETOOTH_ADVERTISING:
        return APP_UI_STATUS_BLUETOOTH_ADVERTISING;
    case APP_UI_BLUETOOTH_CONNECTED:
        return APP_UI_STATUS_BLUETOOTH_CONNECTED;
    case APP_UI_BLUETOOTH_OFF:
    default:
        return APP_UI_STATUS_BLUETOOTH_OFF;
    }
}

static void make_status_bar_state(
    const app_ui_model_t *model,
    app_ui_status_bar_state_t *state,
    char *time_text,
    size_t time_text_size,
    char *weather_text,
    size_t weather_text_size)
{
    if(state == NULL || time_text == NULL || weather_text == NULL) {
        return;
    }

    snprintf(time_text, time_text_size, "%s", "--:--");
    snprintf(weather_text, weather_text_size, "%s", "--C");

    state->time_text = time_text;
    state->time_synced = false;
    state->weather_text = weather_text;
    state->weather_available = false;
    state->wifi_state = APP_UI_STATUS_WIFI_DISCONNECTED;
    state->bluetooth_state = APP_UI_STATUS_BLUETOOTH_OFF;

    if(model == NULL) {
        return;
    }

    state->time_synced = model->status.time.synced;
    if(model->status.time.synced) {
        snprintf(
            time_text,
            time_text_size,
            "%02u:%02u",
            (unsigned int)model->status.time.hour,
            (unsigned int)model->status.time.minute);
    }

    state->weather_available = model->status.weather.available;
    if(model->status.weather.available) {
        snprintf(
            weather_text,
            weather_text_size,
            "%dC",
            (int)model->status.weather.temperature_c);
    }

    state->wifi_state = map_wifi_state(model->status.wifi);
    state->bluetooth_state =
        map_bluetooth_state(model->status.bluetooth);
}

static void menu_open_cb(void *user_data)
{
    app_ui_menu_drawer_t *drawer = user_data;

    (void)App_UiMenuDrawer_Open(
        drawer,
        s_menu_items,
        sizeof(s_menu_items) / sizeof(s_menu_items[0]));
}

static void home_root_delete_cb(lv_event_t *event)
{
    App_UiMenuDrawer_Destroy(lv_event_get_user_data(event));
    s_ready_label = NULL;
    s_open_button = NULL;
}

static void build(lv_obj_t *parent, const app_ui_model_t *model)
{
    static const app_ui_status_bar_callbacks_t status_callbacks = {
        .on_menu = menu_open_cb,
        .on_wifi = NULL,
        .on_bluetooth = NULL,
        .user_data = &s_menu_drawer,
    };
    lv_obj_t *body;
    lv_obj_t *open_button;
    lv_obj_t *open_button_label;
    lv_obj_t *page_root;
    lv_obj_t *ready_label;
    app_ui_status_bar_state_t status_state;
    char time_text[6];
    char weather_text[8];

    App_UiMenuDrawer_Destroy(&s_menu_drawer);
    make_status_bar_state(
        model,
        &status_state,
        time_text,
        sizeof(time_text),
        weather_text,
        sizeof(weather_text));

    page_root = lv_obj_create(parent);
    lv_obj_remove_style_all(page_root);
    lv_obj_set_width(page_root, LV_PCT(100));
    lv_obj_set_flex_grow(page_root, 1);
    lv_obj_set_flex_flow(page_root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(page_root, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(page_root, 8, 0);
    lv_obj_add_event_cb(
        page_root,
        home_root_delete_cb,
        LV_EVENT_DELETE,
        &s_menu_drawer);

    (void)App_UiStatusBar_Create(
        page_root,
        &s_status_bar,
        &status_state,
        &status_callbacks);

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
        s_status_bar.root, APP_UI_MOTION_OPACITY_NONE, -4, 0);
    App_UiMotion_AnimateEnter(
        s_ready_label, APP_UI_MOTION_OPACITY_TEXT, 4, 20);
    App_UiMotion_AnimateEnter(
        s_open_button, APP_UI_MOTION_OPACITY_BACKGROUND, 4, 40);
}

static void refresh(const app_ui_model_t *model)
{
    app_ui_status_bar_state_t status_state;
    char time_text[6];
    char weather_text[8];

    make_status_bar_state(
        model,
        &status_state,
        time_text,
        sizeof(time_text),
        weather_text,
        sizeof(weather_text));
    App_UiStatusBar_Update(&s_status_bar, &status_state);
}

const app_ui_page_t *App_UiPageHome_Get(void)
{
    static const app_ui_page_t page = {
        .id = APP_UI_PAGE_HOME,
        .title = "",
        .dirty_mask = APP_UI_DIRTY_STATUS,
        .show_back = false,
        .build = build,
        .refresh = refresh,
        .enter = enter,
    };
    return &page;
}
