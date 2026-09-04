/**
 * @file App_UiModel.h
 * @brief Model 层头文件 —— 定义 UI 状态快照结构和操作接口。
 *
 * 【设计原则】
 * Model 是纯数据层，不包含任何 LVGL 调用或业务逻辑。
 * 职责单一：存储 UI 状态、维护 dirty 标志、响应事件更新。
 */

#ifndef APP_UI_MODEL_H
#define APP_UI_MODEL_H

#include <stdint.h>

#include "../app/App_Ui.h"

/**
 * @brief UI Model 结构体
 *
 * 保存 UI 当前需要显示的所有状态数据。
 */
typedef struct {
    app_ui_wifi_runtime_t wifi;
    app_ui_wifi_networks_t networks;
    app_ui_wifi_profiles_t profiles;
    app_ui_wifi_operation_t operations[APP_UI_WIFI_OPERATION_CAPACITY];
    app_ui_page_id_t current_page;  /**< 当前显示的页面 ID */
    uint32_t dirty_mask;            /**< 脏标志位：标记哪些域发生了变更 */
    char message[48];               /**< 系统消息/提示文本 */
    app_ui_status_state_t status;   /**< 顶部状态栏使用的平台无关状态 */
} app_ui_model_t;

/**
 * @brief 初始化 Model 为默认值
 * @param model Model 指针
 */
void App_UiModel_Init(app_ui_model_t *model);

/**
 * @brief 将事件应用到 Model 上，更新对应字段并设置 dirty 标志
 * @param model Model 指针
 * @param event 要处理的事件
 */
void App_UiModel_ApplyEvent(app_ui_model_t *model, const app_ui_event_t *event);

/**
 * @brief 设置当前页面 ID，同时设置 APP_UI_DIRTY_NAV 标志
 * @param model Model 指针
 * @param page_id 当前页面 ID
 */
void App_UiModel_SetCurrentPage(app_ui_model_t *model, app_ui_page_id_t page_id);

#endif /* APP_UI_MODEL_H */
