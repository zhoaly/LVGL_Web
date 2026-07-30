/**
 * @file App_UiStatusBar.h
 * @brief Home 顶部状态栏组件：菜单、时间、天气、Wi-Fi 与蓝牙状态。
 */

#ifndef APP_UI_STATUS_BAR_H
#define APP_UI_STATUS_BAR_H

#include <stdbool.h>

#include "lvgl/lvgl.h"

typedef enum {
    APP_UI_STATUS_WIFI_DISCONNECTED = 0,
    APP_UI_STATUS_WIFI_CONNECTING,
    APP_UI_STATUS_WIFI_CONNECTED,
} app_ui_status_wifi_state_t;

typedef enum {
    APP_UI_STATUS_BLUETOOTH_OFF = 0,
    APP_UI_STATUS_BLUETOOTH_ADVERTISING,
    APP_UI_STATUS_BLUETOOTH_CONNECTED,
} app_ui_status_bluetooth_state_t;

typedef struct {
    const char *time_text;
    bool time_synced;
    const char *weather_text;
    bool weather_available;
    app_ui_status_wifi_state_t wifi_state;
    app_ui_status_bluetooth_state_t bluetooth_state;
} app_ui_status_bar_state_t;

typedef void (*app_ui_status_bar_action_cb_t)(void *user_data);

typedef struct {
    app_ui_status_bar_action_cb_t on_menu;
    app_ui_status_bar_action_cb_t on_wifi;
    app_ui_status_bar_action_cb_t on_bluetooth;
    void *user_data;
} app_ui_status_bar_callbacks_t;

/**
 * @brief 状态栏实例上下文。
 *
 * 页面持有该结构，组件负责填充对象引用。页面重建时可以复用同一实例。
 */
typedef struct {
    lv_obj_t *root;
    lv_obj_t *menu_button;
    lv_obj_t *time_label;
    lv_obj_t *weather_label;
    lv_obj_t *wifi_button;
    lv_obj_t *wifi_icon;
    lv_obj_t *bluetooth_button;
    lv_obj_t *bluetooth_icon;
    app_ui_status_wifi_state_t wifi_state;
    app_ui_status_bluetooth_state_t bluetooth_state;
    bool state_initialized;
    app_ui_status_bar_callbacks_t callbacks;
} app_ui_status_bar_t;

/**
 * @brief 创建顶部状态栏并应用初始状态。
 * @return 状态栏根对象；参数无效时返回 NULL。
 */
lv_obj_t *App_UiStatusBar_Create(
    lv_obj_t *parent,
    app_ui_status_bar_t *status_bar,
    const app_ui_status_bar_state_t *state,
    const app_ui_status_bar_callbacks_t *callbacks);

/**
 * @brief 原位更新状态栏显示，不重建对象树。
 */
void App_UiStatusBar_Update(
    app_ui_status_bar_t *status_bar,
    const app_ui_status_bar_state_t *state);

#endif /* APP_UI_STATUS_BAR_H */
