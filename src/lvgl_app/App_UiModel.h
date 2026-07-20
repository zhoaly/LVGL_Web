/**
 * @file App_UiModel.h
 * @brief UI 状态快照（Model）—— 保存 UI 当前需要显示的所有数据。
 *
 * Model 层是业务状态在 UI 侧的只读缓存，职责单一：
 *   - 存储数据：保存 USB/WiFi/Runtime 等域的最新值
 *   - 维护 dirty 标志：标记哪些域发生了变更
 *   - 格式化输出：提供统一的字段值转字符串函数
 *
 * Model 不允许：
 *   - 调用任何 LVGL API
 *   - 直接读取硬件 GPIO
 *   - 主动发起 USB 或 WiFi 操作
 *   - 保存无限长度的日志
 */

#ifndef APP_UI_MODEL_H
#define APP_UI_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "include/App_Ui.h"

/**
 * @brief 数据值 Key 枚举 —— 定义所有 UI 可展示的数据字段。
 *
 * 这些 Key 用于：
 *   1. 页面配置中指定每个模板项对应的数据源
 *   2. App_UiModel_FormatValue() 中统一格式化显示字符串
 *   3. 视图层自动刷新时遍历查找对应的 Label
 */
typedef enum {
    APP_UI_VALUE_USB_MODE = 0,          /**< USB 模式：OFF/PC/ESP */
    APP_UI_VALUE_USB_HUB_ENABLED,       /**< USB HUB 电源状态：ON/OFF */
    APP_UI_VALUE_USB_DEVICE_COUNT,      /**< USB 下游设备数量 */
    APP_UI_VALUE_WIFI_STATE,            /**< WiFi 连接状态：Offline/Connecting/Connected/Error */
    APP_UI_VALUE_WIFI_SSID,             /**< WiFi 连接的 SSID */
    APP_UI_VALUE_WIFI_IP,               /**< WiFi 分配的 IP 地址 */
    APP_UI_VALUE_WIFI_RSSI,             /**< WiFi 信号强度 (dBm) */
    APP_UI_VALUE_UPTIME,                /**< 系统运行时间（秒） */
    APP_UI_VALUE_FREE_HEAP,             /**< 当前空闲堆内存 (KB) */
    APP_UI_VALUE_MIN_HEAP,              /**< 历史最小空闲堆内存 (KB) */
    APP_UI_VALUE_TASK_COUNT,            /**< 系统任务数 */
    APP_UI_VALUE_RESET_REASON,          /**< 上次重启原因 */
    APP_UI_VALUE_MAX,                   /**< 枚举最大值（用于边界检查） */
} app_ui_value_key_t;

/**
 * @brief UI Model 结构体 —— 保存所有需要在 UI 上展示的状态数据。
 *
 * 字段按业务域分组：
 *   - 系统运行时：uptime、堆内存、任务数
 *   - USB 状态：模式、HUB 使能、设备数
 *   - WiFi 状态：连接状态、SSID、IP、RSSI
 *   - 控制台：收发计数和最近结果
 *   - UI 自身状态：当前页面和 dirty 标志
 */
typedef struct {
    app_ui_page_id_t current_page;  /**< 当前显示的页面 ID */
    uint32_t dirty_mask;            /**< 脏标志位：按位标记哪些业务域发生了变化 */

    /* ---- 系统运行时 ---- */
    uint32_t uptime_sec;    /**< 运行时间（秒） */
    uint32_t free_heap_kb;  /**< 当前空闲堆 (KB) */
    uint32_t min_heap_kb;   /**< 最小空闲堆 (KB) */
    uint16_t task_count;    /**< 系统任务数 */

    /* ---- USB 状态 ---- */
    uint8_t usb_mode;           /**< USB 模式：0=OFF, 1=PC, 2=ESP */
    uint8_t usb_hub_enabled;    /**< HUB 电源：0=关闭, 1=开启 */
    uint8_t usb_device_count;   /**< 下游设备数量 */

    /* ---- WiFi 状态 ---- */
    uint8_t wifi_state;     /**< WiFi 状态：0=Offline, 1=Connecting, 2=Connected, 3=Error */
    int16_t wifi_rssi;      /**< 信号强度 (dBm) */
    char wifi_ssid[24];     /**< 连接的 Wi-Fi 名称 */
    char wifi_ip[20];       /**< 分配的 IP 地址 */

    char reset_reason[20];              /**< 重启原因描述 */
    char message[48];                   /**< 系统消息/提示信息 */
} app_ui_model_t;

/**
 * @brief 初始化 Model 为默认初始值。
 * @param model Model 指针
 */
void App_UiModel_Init(app_ui_model_t *model);

/**
 * @brief 将事件应用到 Model 上，更新对应字段并设置 dirty 标志。
 * @param model Model 指针
 * @param event 要处理的事件
 */
void App_UiModel_ApplyEvent(app_ui_model_t *model, const app_ui_event_t *event);

/**
 * @brief 设置当前页面 ID，同时设置 APP_UI_DIRTY_NAV 标志。
 * @param model Model 指针
 * @param page_id 当前页面 ID
 */
 * @param page_id 当前页面 ID
 */
void App_UiModel_SetCurrentPage(app_ui_model_t *model, app_ui_page_id_t page_id);

/**
 * @brief 将指定 Key 对应的值格式化为字符串。
 *
 * 【返回值策略】
 *   - 对于简单值（枚举/布尔）：直接返回静态字符串指针（如 "ON"、"OFF"、"ESP"）
 *   - 对于数值（如堆内存、时间）：使用 snprintf 写入 buf 后返回 buf 指针
 *
 * @param model Model 指针
 * @param key 数据值 Key
 * @param buf 输出缓冲区（用于数值格式化）
 * @param buf_size 缓冲区大小
 * @return 格式化后的字符串指针（可能指向静态内存或 buf）
 */
const char *App_UiModel_FormatValue(const app_ui_model_t *model, app_ui_value_key_t key, char *buf, size_t buf_size);

#endif /* APP_UI_MODEL_H */
