#include "task_action.h"

#include <stdio.h>
#include <string.h>

#define TASK_ACTION_HISTORY_LEN 16u

static app_action_job_snapshot_t s_jobs[TASK_ACTION_HISTORY_LEN];
static app_action_job_id_t s_next_job_id;
static app_action_job_id_t s_latest_job_id;
static app_action_ui_dispatcher_fn s_ui_dispatcher;
static void *s_ui_dispatcher_user;

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
    memset(s_jobs, 0, sizeof(s_jobs));
    s_next_job_id = 1u;
    s_latest_job_id = 0u;
    s_ui_dispatcher = NULL;
    s_ui_dispatcher_user = NULL;
    return ESP_OK;
}

esp_err_t task_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher,
                                              void *user_data)
{
    if(dispatcher == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    s_ui_dispatcher = dispatcher;
    s_ui_dispatcher_user = user_data;
    return ESP_OK;
}

esp_err_t task_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id)
{
    app_action_job_snapshot_t *job;
    esp_err_t result;

    if(request == NULL || job_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *job_id = s_next_job_id++;
    if(s_next_job_id == 0u) {
        s_next_job_id = 1u;
    }
    s_latest_job_id = *job_id;

    job = &s_jobs[*job_id % TASK_ACTION_HISTORY_LEN];
    memset(job, 0, sizeof(*job));
    job->job_id = *job_id;
    job->action_id = request->id;
    job->state = APP_ACTION_JOB_STATE_RUNNING;

    result = s_ui_dispatcher != NULL
        ? s_ui_dispatcher(request, s_ui_dispatcher_user)
        : ESP_ERR_INVALID_STATE;

    job->result = result;
    job->state = result == ESP_OK ? APP_ACTION_JOB_STATE_DONE : APP_ACTION_JOB_STATE_FAILED;
    snprintf(job->message, sizeof(job->message), "%s", result == ESP_OK ? "done" : "failed");
    return result;
}

esp_err_t task_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot)
{
    app_action_job_snapshot_t *job;

    if(job_id == 0u || snapshot == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    job = find_job(job_id);
    if(job == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    *snapshot = *job;
    return ESP_OK;
}

esp_err_t task_action_get_latest(app_action_job_snapshot_t *snapshot)
{
    if(snapshot == NULL || s_latest_job_id == 0u) {
        return ESP_ERR_INVALID_ARG;
    }
    return task_action_get_job(s_latest_job_id, snapshot);
}
