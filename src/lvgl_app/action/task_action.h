/**
 * @file task_action.h
 * @brief 任务 Action 底层实现头文件 —— 任务队列管理和 UI 调度器注册。
 *
 * 【架构位置】
 * 此文件是 Action 系统的内部实现层头文件，app_action.c 通过调用
 * 这些函数完成实际的动作执行。外部模块不应直接引用此文件。
 *
 * 【职责】
 *   - 管理任务历史记录（环形缓冲区）
 *   - 维护 UI 调度器回调指针
 *   - 提交动作并记录执行结果
 *   - 按 ID 查询任务快照
 */

#pragma once

#include "app_action.h"

/**
 * @brief 初始化任务 Action 系统
 * @return ESP_OK 成功
 */
esp_err_t task_action_init(void);

/**
 * @brief 注册 UI 调度器回调
 * @param dispatcher 调度器函数指针
 * @param user_data 用户数据
 * @return ESP_OK 成功
 */
esp_err_t task_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher, void *user_data);

/**
 * @brief 提交动作请求并执行
 * @param request 动作请求指针
 * @param job_id [out] 分配的任务 ID
 * @return ESP_OK 成功
 */
esp_err_t task_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id);

/**
 * @brief 根据任务 ID 查询任务快照
 * @param job_id 任务 ID
 * @param snapshot [out] 快照输出缓冲区
 * @return ESP_OK 成功
 */
esp_err_t task_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot);

/**
 * @brief 获取最近提交的任务快照
 * @param snapshot [out] 快照输出缓冲区
 * @return ESP_OK 成功
 */
esp_err_t task_action_get_latest(app_action_job_snapshot_t *snapshot);
