/**
 * @file app_action.h
 * @brief 动作（Action）系统头文件 —— 定义动作 ID、请求/任务结构和 API。
 *
 * 【架构说明】
 * Action 系统是 UI 框架的命令通道，将外部请求（如页面导航）
 * 转换为内部可处理的任务。外部模块通过 app_action_submit() 提交请求，
 * 经由 UI Dispatcher 转发到 UI 事件队列。
 *
 * 【跨平台兼容】
 * 在非 ESP-IDF 环境（如 PC 浏览器端）下自行定义 esp_err_t 类型和错误码，
 * 确保代码可在标准 C 编译器下编译。
 */

#ifndef APP_ACTION_H
#define APP_ACTION_H

#include <stdint.h>

#ifdef ESP_PLATFORM
#include "esp_err.h"
#else
/* 非 ESP-IDF 环境：自行定义 esp_err_t 和常用错误码 */
typedef int32_t esp_err_t;
#define ESP_OK 0
#define ESP_FAIL (-1)
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
#define APP_ACTION_MESSAGE_MAX_LEN 48

/** 任务 ID 类型（自增无符号整数） */
typedef uint32_t app_action_job_id_t;

/**
 * @brief 动作 ID 枚举
 *
 * 当前仅支持 UI 导航动作，后续可扩展其他类型。
 */
typedef enum {
    APP_ACTION_ID_INVALID = 0,       /**< 无效动作 ID */
    APP_ACTION_ID_UI_NAV_BACK,       /**< UI 导航：返回上一页 */
    APP_ACTION_ID_UI_NAV_HOME,       /**< UI 导航：回到首页 */
    APP_ACTION_ID_UI_NAV_PUSH,       /**< UI 导航：推入新页面 */
    APP_ACTION_ID_MAX,               /**< 枚举最大值（用于边界检查） */
} app_action_id_t;

/**
 * @brief 任务状态枚举
 */
typedef enum {
    APP_ACTION_JOB_STATE_INVALID = 0,  /**< 无效状态 */
    APP_ACTION_JOB_STATE_RUNNING,      /**< 正在执行 */
    APP_ACTION_JOB_STATE_DONE,         /**< 执行成功 */
    APP_ACTION_JOB_STATE_FAILED,       /**< 执行失败 */
} app_action_job_state_t;

/**
 * @brief 动作请求结构体
 *
 * 使用 tagged union 设计，id 字段标识动作类型，
 * params 联合体根据 id 的不同包含不同的参数。
 */
typedef struct {
    app_action_id_t id;              /**< 动作 ID */
    union {
        struct {
            uint32_t page_id;        /**< 导航目标页面 ID */
        } ui_navigation;             /**< UI 导航参数 */

        //当前只有导航动作
        uint8_t reserved[16];        /**< 预留参数空间 */
    } params;
} app_action_request_t;

/**
 * @brief 任务快照结构体
 *
 * 记录一个动作请求的执行结果和状态信息。
 */
typedef struct {
    app_action_job_id_t job_id;                  /**< 任务 ID */
    app_action_id_t action_id;                    /**< 动作 ID */
    app_action_job_state_t state;                 /**< 任务状态 */
    esp_err_t result;                             /**< 执行结果 */
    char message[APP_ACTION_MESSAGE_MAX_LEN];     /**< 执行消息 */
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
typedef esp_err_t (*app_action_ui_dispatcher_fn)(const app_action_request_t *request,
                                                  void *user_data);

/* ======================== API 函数声明 ======================== */

esp_err_t app_action_init(void);
esp_err_t app_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher,
                                             void *user_data);
esp_err_t app_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id);
esp_err_t app_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot);
esp_err_t app_action_get_latest(app_action_job_snapshot_t *snapshot);
const char *app_action_id_to_name(app_action_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* APP_APP_ACTION_H */
