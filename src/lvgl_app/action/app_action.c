/**
 * @file app_action.c
 * @brief Action 系统门面层实现 —— 参数校验并转发到底层 task_action。
 *
 * 【设计模式】门面模式（Facade Pattern）
 * 此文件作为 Action 系统的对外接口层，负责参数合法性校验，
 * 然后将请求委托给底层 task_action 模块执行。
 *
 * 【当前限制】
 * 当前仅支持 UI 导航动作（Back/Home/Push），
 * 其他动作类型暂未实现。
 */

#include "app_action.h"

#include <stddef.h>

#include "task_action.h"

esp_err_t app_action_init(void)
{
    /* 委托给底层 task_action 初始化 */
    return task_action_init();
}

esp_err_t app_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher,
                                             void *user_data)
{
    /* 委托给底层 task_action 注册 UI 调度器 */
    return task_action_register_ui_dispatcher(dispatcher, user_data);
}

esp_err_t app_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id)
{
    /* 参数有效性校验 */
    if(request == NULL || job_id == NULL ||
       request->id <= APP_ACTION_ID_INVALID || request->id >= APP_ACTION_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Push 动作必须指定有效的页面 ID */
    if(request->id == APP_ACTION_ID_UI_NAV_PUSH &&
       request->params.ui_navigation.page_id == 0u) {
        return ESP_ERR_INVALID_ARG;
    }

    /* 委托给底层执行 */
    return task_action_submit(request, job_id);
}

esp_err_t app_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot)
{
    /* 委托给底层查询 */
    return task_action_get_job(job_id, snapshot);
}

esp_err_t app_action_get_latest(app_action_job_snapshot_t *snapshot)
{
    /* 委托给底层查询最新任务 */
    return task_action_get_latest(snapshot);
}

const char *app_action_id_to_name(app_action_id_t id)
{
    /* 将动作 ID 转换为可读的字符串名称 */
    switch(id) {
    case APP_ACTION_ID_UI_NAV_BACK: return "ui_nav_back";
    case APP_ACTION_ID_UI_NAV_HOME: return "ui_nav_home";
    case APP_ACTION_ID_UI_NAV_PUSH: return "ui_nav_push";
    default: return "invalid";
    }
}
