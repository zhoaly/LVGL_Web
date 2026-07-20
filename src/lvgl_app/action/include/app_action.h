/**
 * @file app_action.h
 * @brief 动作（Action）系统公共头文件 —— 定义动作 ID、请求/任务结构和 API。
 *
 * 【架构说明】
 * Action 系统是 UI 框架与底层硬件/业务模块之间的命令通道。
 * 外部模块通过 app_action_submit() 提交动作请求，
 * 经由 UI Dispatcher 转换为 UI 事件队列消息。
 *
 * 【设计要点】
 *   - 动作 ID 枚举保持向后兼容（UI 相关 ID 追加在末尾）
 *   - 请求参数使用联合体支持不同类型的参数
 *   - 任务快照记录完整生命周期（提交→开始→结束）
 *   - 跨平台兼容：在非 ESP-IDF 环境下自行定义 esp_err_t
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef ESP_PLATFORM
#include "esp_err.h"
#else
/* 非 ESP-IDF 环境（如 PC 端编译）下自行定义 esp_err_t */
typedef int32_t esp_err_t;
#define ESP_OK 0
#define ESP_FAIL (-1)
#define ESP_ERR_NO_MEM 0x101
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_INVALID_STATE 0x103
#define ESP_ERR_NOT_FOUND 0x105
#define ESP_ERR_NOT_SUPPORTED 0x106
#define ESP_ERR_TIMEOUT 0x107
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** 任务快照消息缓冲区最大长度 */
#define APP_ACTION_MESSAGE_MAX_LEN 96

/** 任务 ID 类型（自增无符号整数） */
typedef uint32_t app_action_job_id_t;

/**
 * @brief 动作 ID 枚举
 *
 * 【兼容性说明】
 * 保持原始 ESP 动作 ID 的顺序不变，UI 相关 ID 追加在末尾，
 * 以确保已有代码中存储的数值不会因新增枚举值而改变。
 *
 * 【分组】
 *   - EPD (0x01-0x08)：墨水屏控制
 *   - WS2812 (0x09-0x12)：RGB LED 控制
 *   - Encoder (0x13-0x17)：旋转编码器
 *   - Key (0x18-0x1C)：按键
 *   - WiFi (0x1D-0x31)：WiFi 连接管理
 *   - HTTP (0x32-0x34)：HTTP 请求
 *   - OTA (0x35-0x3F)：OTA 升级
 *   - Time (0x40-0x46)：时间同步
 *   - GPIO (0x47-0x51)：GPIO 控制
 *   - Sys (0x52-0x55)：系统信息
 *   - UI (0x56-0x58)：UI 导航
 */
typedef enum {
    APP_ACTION_ID_INVALID = 0,           /**< 无效动作 ID */
    APP_ACTION_ID_EPD_INFO,              /**< 获取墨水屏信息 */
    APP_ACTION_ID_EPD_INIT,              /**< 初始化墨水屏 */
    APP_ACTION_ID_EPD_CLEAR,             /**< 清空墨水屏 */
    APP_ACTION_ID_EPD_BLACK,             /**< 墨水屏全黑 */
    APP_ACTION_ID_EPD_BW,                /**< 墨水屏黑白切换 */
    APP_ACTION_ID_EPD_SLEEP,             /**< 墨水屏进入睡眠 */
    APP_ACTION_ID_EPD_WAKE,              /**< 唤醒墨水屏 */
    APP_ACTION_ID_WS2812_INIT,           /**< 初始化 WS2812 LED */
    APP_ACTION_ID_WS2812_DEINIT,         /**< 反初始化 WS2812 */
    APP_ACTION_ID_WS2812_STATUS,         /**< 查询 WS2812 状态 */
    APP_ACTION_ID_WS2812_ON,             /**< 开启 WS2812 */
    APP_ACTION_ID_WS2812_OFF,            /**< 关闭 WS2812 */
    APP_ACTION_ID_WS2812_RGB,            /**< 设置 WS2812 RGB 颜色 */
    APP_ACTION_ID_WS2812_PIXEL,          /**< 设置单个像素 */
    APP_ACTION_ID_WS2812_CLEAR,          /**< 清除 WS2812 显示 */
    APP_ACTION_ID_WS2812_FRAME,          /**< 发送 WS2812 帧数据 */
    APP_ACTION_ID_WS2812_DEMO,           /**< 启动 WS2812 演示模式 */
    APP_ACTION_ID_ENCODER_STATUS,        /**< 查询编码器状态 */
    APP_ACTION_ID_ENCODER_START,         /**< 启动编码器 */
    APP_ACTION_ID_ENCODER_STOP,          /**< 停止编码器 */
    APP_ACTION_ID_ENCODER_RESET,         /**< 重置编码器计数 */
    APP_ACTION_ID_ENCODER_REVERSE,       /**< 反转编码器方向 */
    APP_ACTION_ID_KEY_STATUS,            /**< 查询按键状态 */
    APP_ACTION_ID_KEY_RAW,               /**< 读取按键原始值 */
    APP_ACTION_ID_KEY_START,             /**< 启动按键扫描 */
    APP_ACTION_ID_KEY_STOP,              /**< 停止按键扫描 */
    APP_ACTION_ID_KEY_RESET_STAT,        /**< 重置按键统计 */
    APP_ACTION_ID_WIFI_INIT,             /**< 初始化 WiFi */
    APP_ACTION_ID_WIFI_DEINIT,           /**< 反初始化 WiFi */
    APP_ACTION_ID_WIFI_STATUS,           /**< 查询 WiFi 状态 */
    APP_ACTION_ID_WIFI_STA_START,        /**< 启动 Station 模式 */
    APP_ACTION_ID_WIFI_STA_STOP,         /**< 停止 Station 模式 */
    APP_ACTION_ID_WIFI_STA_CONNECT,      /**< 连接 WiFi */
    APP_ACTION_ID_WIFI_STA_DISCONNECT,   /**< 断开 WiFi */
    APP_ACTION_ID_WIFI_STA_RECONNECT,    /**< 重新连接 WiFi */
    APP_ACTION_ID_WIFI_STA_CONNECT_SAVED, /**< 连接已保存的网络 */
    APP_ACTION_ID_WIFI_SCAN_START,       /**< 开始扫描 WiFi */
    APP_ACTION_ID_WIFI_SCAN_WAIT,        /**< 等待扫描完成 */
    APP_ACTION_ID_WIFI_SCAN_LIST,        /**< 获取扫描结果列表 */
    APP_ACTION_ID_WIFI_NVS_SAVE,         /**< 保存 WiFi 配置到 NVS */
    APP_ACTION_ID_WIFI_NVS_LOAD,         /**< 从 NVS 加载 WiFi 配置 */
    APP_ACTION_ID_WIFI_NVS_CLEAR,        /**< 清除 NVS 中的 WiFi 配置 */
    APP_ACTION_ID_WIFI_NVS_HAS,          /**< 检查 NVS 中是否有 WiFi 配置 */
    APP_ACTION_ID_HTTP_INIT,             /**< 初始化 HTTP 客户端 */
    APP_ACTION_ID_HTTP_GET,              /**< 发送 HTTP GET 请求 */
    APP_ACTION_ID_HTTP_POST_JSON,        /**< 发送 HTTP POST JSON 请求 */
    APP_ACTION_ID_OTA_VERSION,           /**< 获取 OTA 版本信息 */
    APP_ACTION_ID_OTA_STATUS,            /**< 查询 OTA 状态 */
    APP_ACTION_ID_OTA_CHECK,             /**< 检查 OTA 更新 */
    APP_ACTION_ID_OTA_UPGRADE_MANIFEST,  /**< 获取 OTA 升级清单 */
    APP_ACTION_ID_OTA_UPGRADE,           /**< 执行 OTA 升级 */
    APP_ACTION_ID_OTA_UPGRADE_CHECKED,   /**< 执行经过校验的 OTA 升级 */
    APP_ACTION_ID_OTA_CONFIRM,           /**< 确认 OTA 升级成功 */
    APP_ACTION_ID_OTA_ROLLBACK,          /**< 回滚 OTA 版本 */
    APP_ACTION_ID_TIME_INIT,             /**< 初始化时间服务 */
    APP_ACTION_ID_TIME_START,            /**< 启动时间同步 */
    APP_ACTION_ID_TIME_SYNC,             /**< 执行时间同步 */
    APP_ACTION_ID_TIME_NOW,              /**< 获取当前时间 */
    APP_ACTION_ID_TIME_STATUS,           /**< 查询时间服务状态 */
    APP_ACTION_ID_TIME_DEINIT,           /**< 反初始化时间服务 */
    APP_ACTION_ID_GPIO_INFO,             /**< 获取 GPIO 信息 */
    APP_ACTION_ID_GPIO_GET,              /**< 读取 GPIO 电平 */
    APP_ACTION_ID_GPIO_SET,              /**< 设置 GPIO 电平 */
    APP_ACTION_ID_GPIO_INPUT,            /**< 设置 GPIO 为输入模式 */
    APP_ACTION_ID_GPIO_OUTPUT,           /**< 设置 GPIO 为输出模式 */
    APP_ACTION_ID_GPIO_PULLUP,           /**< 启用 GPIO 上拉 */
    APP_ACTION_ID_GPIO_PULLDOWN,         /**< 启用 GPIO 下拉 */
    APP_ACTION_ID_GPIO_TOGGLE,           /**< 切换 GPIO 电平 */
    APP_ACTION_ID_GPIO_WATCH,            /**< 监测 GPIO 变化 */
    APP_ACTION_ID_SYS_INFO,              /**< 获取系统信息 */
    APP_ACTION_ID_SYS_RESTART,           /**< 重启系统 */
    APP_ACTION_ID_SYS_HEAP,              /**< 获取堆内存信息 */
    APP_ACTION_ID_RUNTIME,               /**< 获取运行时信息 */
    APP_ACTION_ID_TASKLIST,              /**< 获取任务列表 */
    APP_ACTION_ID_UI_NAV_BACK,           /**< UI 导航：返回上一页 */
    APP_ACTION_ID_UI_NAV_HOME,           /**< UI 导航：回到首页 */
    APP_ACTION_ID_UI_NAV_PUSH,           /**< UI 导航：推入新页面 */
    APP_ACTION_ID_MAX,                   /**< 枚举最大值（用于边界检查） */
} app_action_id_t;

/**
 * @brief 任务状态枚举
 */
typedef enum {
    APP_ACTION_JOB_STATE_INVALID = 0,   /**< 无效状态 */
    APP_ACTION_JOB_STATE_PENDING,       /**< 等待执行 */
    APP_ACTION_JOB_STATE_RUNNING,       /**< 正在执行 */
    APP_ACTION_JOB_STATE_DONE,          /**< 执行成功 */
    APP_ACTION_JOB_STATE_FAILED,        /**< 执行失败 */
} app_action_job_state_t;

/**
 * @brief 动作请求结构体
 *
 * 使用 tagged union 设计，id 字段标识动作类型，
 * params 联合体根据 id 的不同包含不同的参数结构。
 * reserved 字段预留用于未来扩展（768 字节）。
 */
typedef struct {
    app_action_id_t id;                 /**< 动作 ID */
    union {
        struct {
            uint32_t page_id;           /**< UI 导航目标页面 ID */
        } ui_navigation;                /**< UI 导航参数 */
        uint8_t reserved[768];           /**< 预留参数空间（供其他动作类型使用） */
    } params;
} app_action_request_t;

/**
 * @brief 任务快照结构体
 *
 * 记录一个动作请求的完整生命周期信息，包括：
 *   - 提交时间、开始时间、结束时间（微秒级时间戳）
 *   - 执行结果和状态
 *   - 执行消息
 */
typedef struct {
    app_action_job_id_t job_id;                 /**< 任务 ID */
    app_action_id_t action_id;                   /**< 动作 ID */
    app_action_job_state_t state;                /**< 任务状态 */
    esp_err_t result;                            /**< 执行结果 */
    int64_t submitted_at_us;                     /**< 提交时间戳（微秒） */
    int64_t started_at_us;                       /**< 开始时间戳（微秒） */
    int64_t finished_at_us;                      /**< 完成时间戳（微秒） */
    char message[APP_ACTION_MESSAGE_MAX_LEN];    /**< 执行消息（如错误描述） */
} app_action_job_snapshot_t;

/**
 * @brief UI 调度器函数类型
 *
 * Action 系统通过此回调将 UI 相关动作转发到 UI 控制器。
 *
 * @param request 动作请求指针
 * @param user_data 用户自定义数据
 * @return ESP_OK 成功，其他值表示失败
 */
typedef esp_err_t (*app_action_ui_dispatcher_fn)(const app_action_request_t *request, void *user_data);

/* ======================== API 函数声明 ======================== */

/**
 * @brief 初始化 Action 系统
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t app_action_init(void);

/**
 * @brief 注册 UI 调度器回调
 *
 * 注册的回调将在 app_action_submit() 提交 UI 相关动作时被调用。
 *
 * @param dispatcher 调度器函数指针
 * @param user_data 传递给调度器的用户数据
 * @return ESP_OK 成功
 */
esp_err_t app_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher, void *user_data);

/**
 * @brief 提交一个动作请求
 *
 * 将动作请求提交到 Action 系统执行。
 * 对于 UI 导航动作，会通过 UI 调度器转发到 UI 事件队列。
 *
 * @param request 动作请求指针
 * @param job_id [out] 返回分配的任务 ID
 * @return ESP_OK 成功
 */
esp_err_t app_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id);

/**
 * @brief 根据任务 ID 获取任务快照
 * @param job_id 任务 ID
 * @param snapshot [out] 任务快照输出缓冲区
 * @return ESP_OK 成功
 */
esp_err_t app_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot);

/**
 * @brief 获取最近一次提交的任务快照
 * @param snapshot [out] 任务快照输出缓冲区
 * @return ESP_OK 成功
 */
esp_err_t app_action_get_latest(app_action_job_snapshot_t *snapshot);

/**
 * @brief 将动作 ID 转换为可读名称字符串
 * @param id 动作 ID
 * @return 动作名称字符串（如 "ui_nav_back"）
 */
const char *app_action_id_to_name(app_action_id_t id);

#ifdef __cplusplus
}
#endif
