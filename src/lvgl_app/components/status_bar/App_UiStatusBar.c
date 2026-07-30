/**
 * @file App_UiStatusBar.c
 * @brief Home 顶部状态栏组件实现。
 */

#include "App_UiStatusBar.h"

#include <string.h>

#include "../../assets/App_UiAssets.h"
#include "../../assets/App_UiTheme.h"
#include "../App_UiComponents.h"
#include "../motion/App_UiMotion.h"

enum {
    STATUS_BAR_HEIGHT = 32,
    STATUS_BAR_SIDE_WIDTH = 64,
    STATUS_BAR_MENU_SIZE = 32,
    STATUS_BAR_ICON_BUTTON_SIZE = 28,
    STATUS_BAR_ICON_SIZE = 18,
};

static const app_ui_status_bar_state_t default_state = {
    .time_text = "--:--",
    .time_synced = false,
    .weather_text = "--C",
    .weather_available = false,
    .wifi_state = APP_UI_STATUS_WIFI_DISCONNECTED,
    .bluetooth_state = APP_UI_STATUS_BLUETOOTH_OFF,
};

static void menu_click_cb(lv_event_t *event)
{
    app_ui_status_bar_t *status_bar = lv_event_get_user_data(event);

    if(status_bar != NULL && status_bar->callbacks.on_menu != NULL) {
        status_bar->callbacks.on_menu(status_bar->callbacks.user_data);
    }
}

static void wifi_click_cb(lv_event_t *event)
{
    app_ui_status_bar_t *status_bar = lv_event_get_user_data(event);

    if(status_bar != NULL && status_bar->callbacks.on_wifi != NULL) {
        status_bar->callbacks.on_wifi(status_bar->callbacks.user_data);
    }
}

static void bluetooth_click_cb(lv_event_t *event)
{
    app_ui_status_bar_t *status_bar = lv_event_get_user_data(event);

    if(status_bar != NULL && status_bar->callbacks.on_bluetooth != NULL) {
        status_bar->callbacks.on_bluetooth(status_bar->callbacks.user_data);
    }
}

static void status_bar_delete_cb(lv_event_t *event)
{
    app_ui_status_bar_t *status_bar = lv_event_get_user_data(event);

    if(status_bar == NULL) {
        return;
    }

    App_UiMotion_StopObject(status_bar->wifi_icon);
    App_UiMotion_StopObject(status_bar->bluetooth_icon);
    status_bar->root = NULL;
    status_bar->wifi_icon = NULL;
    status_bar->bluetooth_icon = NULL;
    status_bar->state_initialized = false;
}

static lv_obj_t *create_slot(lv_obj_t *parent, int32_t width)
{
    lv_obj_t *slot = lv_obj_create(parent);

    lv_obj_remove_style_all(slot);
    lv_obj_set_size(slot, width, STATUS_BAR_HEIGHT);
    return slot;
}

static lv_obj_t *create_icon(lv_obj_t *parent, app_ui_icon_id_t icon_id)
{
    lv_obj_t *icon = lv_image_create(parent);

    lv_image_set_src(icon, App_UiAssets_GetIcon(icon_id));
    lv_obj_set_size(icon, STATUS_BAR_ICON_SIZE, STATUS_BAR_ICON_SIZE);
    lv_obj_set_style_image_recolor_opa(icon, LV_OPA_COVER, 0);
    lv_obj_center(icon);
    return icon;
}

static lv_obj_t *create_status_button(
    lv_obj_t *parent,
    app_ui_icon_id_t icon_id,
    lv_event_cb_t click_cb,
    app_ui_status_bar_t *status_bar,
    lv_obj_t **icon_out)
{
    lv_obj_t *button = lv_button_create(parent);

    lv_obj_remove_style_all(button);
    lv_obj_set_size(button, STATUS_BAR_ICON_BUTTON_SIZE,
                    STATUS_BAR_ICON_BUTTON_SIZE);
    lv_obj_set_style_radius(button, 10, 0);
    lv_obj_set_style_bg_color(
        button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_SURFACE_MUTED),
        LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, LV_STATE_PRESSED);
    App_UiComponent_ApplyFocusStyle(
        button, APP_UI_COMPONENT_FOCUS_LIGHT);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(button, click_cb, LV_EVENT_CLICKED, status_bar);

    *icon_out = create_icon(button, icon_id);
    return button;
}

static lv_color_t wifi_color(app_ui_status_wifi_state_t state)
{
    switch(state) {
        case APP_UI_STATUS_WIFI_CONNECTED:
            return App_UiTheme_GetColor(APP_UI_THEME_COLOR_ACCENT);
        case APP_UI_STATUS_WIFI_CONNECTING:
            return App_UiTheme_GetColor(APP_UI_THEME_COLOR_WARNING);
        case APP_UI_STATUS_WIFI_DISCONNECTED:
        default:
            return App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED);
    }
}

static lv_color_t bluetooth_color(app_ui_status_bluetooth_state_t state)
{
    switch(state) {
        case APP_UI_STATUS_BLUETOOTH_CONNECTED:
            return App_UiTheme_GetColor(APP_UI_THEME_COLOR_ACCENT);
        case APP_UI_STATUS_BLUETOOTH_ADVERTISING:
            return App_UiTheme_GetColor(APP_UI_THEME_COLOR_WARNING);
        case APP_UI_STATUS_BLUETOOTH_OFF:
        default:
            return App_UiTheme_GetColor(APP_UI_THEME_COLOR_TEXT_MUTED);
    }
}

lv_obj_t *App_UiStatusBar_Create(
    lv_obj_t *parent,
    app_ui_status_bar_t *status_bar,
    const app_ui_status_bar_state_t *state,
    const app_ui_status_bar_callbacks_t *callbacks)
{
    lv_obj_t *left_slot;
    lv_obj_t *center_slot;
    lv_obj_t *center_spacer;
    lv_obj_t *right_slot;
    lv_obj_t *menu_icon;

    if(parent == NULL || status_bar == NULL) {
        return NULL;
    }

    memset(status_bar, 0, sizeof(*status_bar));
    if(callbacks != NULL) {
        status_bar->callbacks = *callbacks;
    }

    status_bar->root = lv_obj_create(parent);
    lv_obj_remove_style_all(status_bar->root);
    lv_obj_set_size(status_bar->root, LV_PCT(100), STATUS_BAR_HEIGHT);
    lv_obj_set_flex_flow(status_bar->root, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_bar->root, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_event_cb(
        status_bar->root,
        status_bar_delete_cb,
        LV_EVENT_DELETE,
        status_bar);

    left_slot = create_slot(status_bar->root, STATUS_BAR_SIDE_WIDTH);
    status_bar->menu_button = lv_button_create(left_slot);
    lv_obj_remove_style_all(status_bar->menu_button);
    lv_obj_set_size(status_bar->menu_button, STATUS_BAR_MENU_SIZE,
                    STATUS_BAR_MENU_SIZE);
    lv_obj_set_style_radius(status_bar->menu_button, 11, 0);
    lv_obj_set_style_bg_color(
        status_bar->menu_button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_BACKGROUND),
        0);
    lv_obj_set_style_bg_opa(status_bar->menu_button, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(
        status_bar->menu_button,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_PRESSED),
        LV_STATE_PRESSED);
    App_UiComponent_ApplyFocusStyle(
        status_bar->menu_button, APP_UI_COMPONENT_FOCUS_DARK);
    lv_obj_add_flag(status_bar->menu_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(status_bar->menu_button, menu_click_cb,
                        LV_EVENT_CLICKED, status_bar);

    menu_icon = create_icon(
        status_bar->menu_button, APP_UI_ICON_STATUS_MENU);
    lv_obj_set_style_image_recolor(
        menu_icon,
        App_UiTheme_GetColor(APP_UI_THEME_COLOR_CONTROL_FOREGROUND),
        0);

    center_slot = create_slot(status_bar->root, 0);
    lv_obj_set_flex_grow(center_slot, 1);
    lv_obj_set_flex_flow(center_slot, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(center_slot, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    status_bar->time_label = lv_label_create(center_slot);
    lv_obj_set_style_text_font(
        status_bar->time_label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_EMPHASIS),
        0);

    center_spacer = lv_obj_create(center_slot);
    lv_obj_remove_style_all(center_spacer);
    lv_obj_set_size(center_spacer, 5, 1);

    status_bar->weather_label = lv_label_create(center_slot);
    lv_obj_set_style_text_font(
        status_bar->weather_label,
        App_UiTheme_GetFont(APP_UI_THEME_FONT_BODY),
        0);

    right_slot = create_slot(status_bar->root, STATUS_BAR_SIDE_WIDTH);
    lv_obj_set_flex_flow(right_slot, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right_slot, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    status_bar->wifi_button = create_status_button(
        right_slot,
        APP_UI_ICON_STATUS_WIFI,
        wifi_click_cb,
        status_bar,
        &status_bar->wifi_icon);
    status_bar->bluetooth_button = create_status_button(
        right_slot,
        APP_UI_ICON_STATUS_BLUETOOTH,
        bluetooth_click_cb,
        status_bar,
        &status_bar->bluetooth_icon);

    App_UiStatusBar_Update(status_bar, state != NULL ? state : &default_state);
    return status_bar->root;
}

void App_UiStatusBar_Update(
    app_ui_status_bar_t *status_bar,
    const app_ui_status_bar_state_t *state)
{
    bool wifi_changed;
    bool bluetooth_changed;

    if(status_bar == NULL || status_bar->root == NULL) {
        return;
    }
    if(state == NULL) {
        state = &default_state;
    }

    lv_label_set_text(
        status_bar->time_label,
        state->time_text != NULL ? state->time_text : default_state.time_text);
    lv_obj_set_style_text_color(
        status_bar->time_label,
        App_UiTheme_GetColor(
            state->time_synced
                ? APP_UI_THEME_COLOR_TEXT_PRIMARY
                : APP_UI_THEME_COLOR_WARNING),
        0);
    lv_label_set_text(
        status_bar->weather_label,
        state->weather_text != NULL
            ? state->weather_text
            : default_state.weather_text);
    lv_obj_set_style_text_color(
        status_bar->weather_label,
        App_UiTheme_GetColor(
            state->weather_available
                ? APP_UI_THEME_COLOR_TEXT_PRIMARY
                : APP_UI_THEME_COLOR_TEXT_MUTED),
        0);
    wifi_changed =
        !status_bar->state_initialized ||
        status_bar->wifi_state != state->wifi_state;
    bluetooth_changed =
        !status_bar->state_initialized ||
        status_bar->bluetooth_state != state->bluetooth_state;

    if(wifi_changed) {
        lv_obj_set_style_image_recolor(
            status_bar->wifi_icon, wifi_color(state->wifi_state), 0);
        if(state->wifi_state == APP_UI_STATUS_WIFI_CONNECTING) {
            App_UiMotion_StartImagePulse(status_bar->wifi_icon);
        } else if(status_bar->state_initialized) {
            App_UiMotion_SettleImage(status_bar->wifi_icon);
        } else {
            App_UiMotion_StopObject(status_bar->wifi_icon);
            lv_obj_set_style_image_opa(
                status_bar->wifi_icon, LV_OPA_COVER, 0);
        }
        status_bar->wifi_state = state->wifi_state;
    }

    if(bluetooth_changed) {
        lv_obj_set_style_image_recolor(
            status_bar->bluetooth_icon,
            bluetooth_color(state->bluetooth_state),
            0);
        if(state->bluetooth_state ==
           APP_UI_STATUS_BLUETOOTH_ADVERTISING) {
            App_UiMotion_StartImagePulse(status_bar->bluetooth_icon);
        } else if(status_bar->state_initialized) {
            App_UiMotion_SettleImage(status_bar->bluetooth_icon);
        } else {
            App_UiMotion_StopObject(status_bar->bluetooth_icon);
            lv_obj_set_style_image_opa(
                status_bar->bluetooth_icon, LV_OPA_COVER, 0);
        }
        status_bar->bluetooth_state = state->bluetooth_state;
    }
    status_bar->state_initialized = true;
}
