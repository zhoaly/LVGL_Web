/**
 * @file task_action.c
 * @brief 任务 Action 底层实现 —— 任务队列管理和 UI 调度器。
 *
 * 【设计说明】
 * 使用环形缓冲区（circular buffer）保存最近 16 条任务历史记录。
 * 任务 ID 自增，永不重复（溢出后从 1 重新开始）。
 * 提交动作时同步调用 UI 调度器，为同步执行模式。
 *
 * 【数据流】
 *   app_action_submit()
 *     -> task_action_submit()
 *       -> 分配 job_id，填充快照
 *       -> 调用 s_ui_dispatcher() 执行动作
 *       -> 更新执行结果和状态
 *       -> 返回
 */

#include "task_action.h"

#include <stdio.h>
#include <string.h>

/** 任务历史记录环形缓冲区长度 */
#define TASK_ACTION_HISTORY_LEN 16u

/** 任务历史环形缓冲区 */
static app_action_job_snapshot_t s_jobs[TASK_ACTION_HISTORY_LEN];

/** 下一个任务 ID（自增） */
static app_action_job_id_t s_next_job_id = 1u;

/** 最近一次提交的任务 ID */
static app_action_job_id_t s_latest_job_id;

/** 注册的 UI 调度器回调指针 */
static app_action_ui_dispatcher_fn s_ui_dispatcher;

/** UI 调度器用户数据 */
static void *s_ui_dispatcher_user;

/**
 * @brief 在环形缓冲区中根据 job_id 查找任务快照
 * @param job_id 要查找的任务 ID
 * @return 任务快照指针，未找到返回 NULL
 */
static app_action_job_snapshot_t *find_job(app_action_job_id_t job_id)
{
    size_t i;
    for(i = 0; i < TASK_ACTION_HISTORY_LEN; i++) {
        if(s_jobs[i].job_id == job_id) {
            return &s_jobs[i];
        }
    }
    return NULL;
}

esp_err_t task_action_init(void)
{
    /* 当前不需要额外初始化，任务缓冲区为静态全局变量 */
    return ESP_OK;
}

esp_err_t task_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher, void *user_data)
{
    s_ui_dispatcher = dispatcher;
    s_ui_dispatcher_user = user_data;
    return dispatcher != NULL ? ESP_OK : ESP_ERR_INVALID_ARG;
}

esp_err_t task_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id)
{
    app_action_job_snapshot_t *job;
    esp_err_t result;

    /* 参数校验 */
    if(request == NULL || job_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* 分配新任务 ID（自增，溢出后回绕到 1） */
    *job_id = s_next_job_id++;
    if(s_next_job_id == 0u) {
        s_next_job_id = 1u;
    }
    s_latest_job_id = *job_id;

    /* 在环形缓冲区中定位槽位并初始化 */
    job = &s_jobs[*job_id % TASK_ACTION_HISTORY_LEN];
    memset(job, 0, sizeof(*job));
    job->job_id = *job_id;
    job->action_id = request->id;
    job->state = APP_ACTION_JOB_STATE_RUNNING;

    /* 同步调用 UI 调度器执行动作 */
    result = s_ui_dispatcher != NULL
        ? s_ui_dispatcher(request, s_ui_dispatcher_user)
        : ESP_ERR_INVALID_STATE;

    /* 更新执行结果 */
    job->result = result;
    job->state = result == ESP_OK ? APP_ACTION_JOB_STATE_DONE : APP_ACTION_JOB_STATE_FAILED;
    snprintf(job->message, sizeof(job->message), "%s", result == ESP_OK ? "done" : "failed");
    return result;
}

esp_err_t task_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot)
{
    app_action_job_snapshot_t *job;

    /* 参数校验 */
    if(job_id == 0u || snapshot == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* 在环形缓冲区中查找 */
    job = find_job(job_id);
    if(job == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    /* 复制快照到输出缓冲区 */
    *snapshot = *job;
    return ESP_OK;
}

esp_err_t task_action_get_latest(app_action_job_snapshot_t *snapshot)
{
    /* 参数校验 */
    if(snapshot == NULL || s_latest_job_id == 0u) {
        return ESP_ERR_INVALID_ARG;
    }
    /* 按最新 job_id 查询 */
    return task_action_get_job(s_latest_job_id, snapshot);
}
