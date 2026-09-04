/**
 * @file App_Ui.h
 * @brief UI 应用层主头文件 —— 定义公共类型、事件结构和顶层 API。
 *
 * 【架构位置】
 * 此文件位于 MVC 架构的最顶层，所有模块（Model/View/Nav/Port/Pages）
 * 都直接或间接引用此文件中定义的基础类型。
 *
 * 【职责范围】
 *   - 页面 ID 枚举：定义所有支持的页面
 *   - Dirty 标志位：按位标记哪些域需要刷新
 *   - 事件类型枚举和结构体：UI 内部通信的事件格式
 *   - 顶层 API 函数：初始化、启动、事件投递
 */

#ifndef APP_UI_H
#define APP_UI_H

#include <stdbool.h>
#include <stdint.h>
#include "App_UiWifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 页面 ID 枚举
 */
typedef enum {
    APP_UI_PAGE_NONE = 0,  /**< 无效页面，用于初始化和错误状态 */
    APP_UI_PAGE_HOME,      /**< 首页 */
    APP_UI_PAGE_TEXT,      /**< 纯文字页面 */
    APP_UI_PAGE_NETWORK,
    APP_UI_PAGE_HID_HUB,
    APP_UI_PAGE_SETTINGS,
    APP_UI_PAGE_DEBUG,
    APP_UI_PAGE_CONTROLS_GALLERY,
    APP_UI_PAGE_WIFI_SAVED,
    APP_UI_PAGE_WIFI_DETAIL,
    APP_UI_PAGE_COUNT,
} app_ui_page_id_t;

/**
 * @brief Dirty 标志位枚举（按位标记业务域变更）
 */
typedef enum {
    APP_UI_DIRTY_NONE = 0,             /**< 无变更 */
    APP_UI_DIRTY_SYSTEM = (1u << 0),   /**< 系统消息变更 */
    APP_UI_DIRTY_NAV = (1u << 1),      /**< 导航变更（页面切换） */
    APP_UI_DIRTY_STATUS = (1u << 2),   /**< 时间、天气和无线连接状态变更 */
    APP_UI_DIRTY_WIFI = (1u << 3),
    APP_UI_DIRTY_ALL = 0xFFFFFFFFu,    /**< 全量刷新标志 */
} app_ui_dirty_mask_t;

typedef enum {
    APP_UI_WIFI_DISCONNECTED = 0,
    APP_UI_WIFI_CONNECTING,
    APP_UI_WIFI_CONNECTED,
    APP_UI_WIFI_STATE_COUNT,
} app_ui_wifi_state_t;

typedef enum {
    APP_UI_BLUETOOTH_OFF = 0,
    APP_UI_BLUETOOTH_ADVERTISING,
    APP_UI_BLUETOOTH_CONNECTED,
    APP_UI_BLUETOOTH_STATE_COUNT,
} app_ui_bluetooth_state_t;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    bool synced;
} app_ui_time_state_t;

typedef struct {
    int16_t temperature_c;
    bool available;
} app_ui_weather_state_t;

typedef struct {
    app_ui_time_state_t time;
    app_ui_weather_state_t weather;
    app_ui_wifi_state_t wifi;
    app_ui_bluetooth_state_t bluetooth;
} app_ui_status_state_t;

/**
 * @brief UI 事件类型枚举
 */
typedef enum {
    APP_UI_EVENT_NONE = 0,               /**< 空事件 */
    APP_UI_EVENT_SHOW_MESSAGE,           /**< 显示 Toast 消息 */
    APP_UI_EVENT_TIME_UPDATED,           /**< 时间状态更新 */
    APP_UI_EVENT_WEATHER_UPDATED,        /**< 天气状态更新 */
    APP_UI_EVENT_WIFI_STATE_CHANGED,     /**< Wi-Fi 状态更新 */
    APP_UI_EVENT_BLUETOOTH_STATE_CHANGED,/**< 蓝牙状态更新 */
    APP_UI_EVENT_WIFI_RUNTIME,
    APP_UI_EVENT_WIFI_NETWORKS,
    APP_UI_EVENT_WIFI_PROFILES,
    APP_UI_EVENT_WIFI_OPERATION,
    APP_UI_EVENT_COUNT,
} app_ui_event_type_t;

/**
 * @brief UI 事件结构体
 */
typedef struct {
    app_ui_event_type_t type;  /**< 事件类型 */
    char text[48];             /**< SHOW_MESSAGE 消息文本 */
    union {
        app_ui_wifi_runtime_t wifi_runtime;
        app_ui_wifi_networks_t wifi_networks;
        app_ui_wifi_profiles_t wifi_profiles;
        app_ui_wifi_operation_t wifi_operation;
        app_ui_time_state_t time;
        app_ui_weather_state_t weather;
        app_ui_wifi_state_t wifi;
        app_ui_bluetooth_state_t bluetooth;
    } data;
} app_ui_event_t;

/* ======================== 顶层 API 函数声明 ======================== */

/**
 * @brief 初始化 UI 系统
 *
 * 依次初始化 Model、Nav、Port、Action 系统和 View。
 * @return true 初始化成功
 */
bool App_UiInit(void);

/**
 * @brief 启动 UI 系统，显示首页
 * @return true 启动成功
 */
bool App_UiStart(void);

/**
 * @brief 投递一个事件到 UI 事件队列
 * @param event 要投递的事件指针
 * @return true 投递成功
 */
bool App_UiPostEvent(const app_ui_event_t *event);

/**
 * @brief 快捷显示一条 Toast 消息
 * @param message 消息文本
 * @return true 投递成功
 */
bool App_UiShowMessage(const char *message);

/**
 * @brief 检查 UI 系统是否已初始化完毕
 * @return true 已就绪
 */
bool App_UiIsReady(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_UI_H */
