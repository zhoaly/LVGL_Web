#include "app_action.h"

#include <stddef.h>

#include "task_action.h"

esp_err_t app_action_init(void)
{
    return task_action_init();
}

esp_err_t app_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher,
                                             void *user_data)
{
    return task_action_register_ui_dispatcher(dispatcher, user_data);
}

esp_err_t app_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id)
{
    if(request == NULL || job_id == NULL ||
       request->id <= APP_ACTION_ID_INVALID || request->id >= APP_ACTION_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if(request->id == APP_ACTION_ID_UI_NAV_PUSH &&
       request->params.ui_navigation.page_id == 0u) {
        return ESP_ERR_INVALID_ARG;
    }

    return task_action_submit(request, job_id);
}

esp_err_t app_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot)
{
    return task_action_get_job(job_id, snapshot);
}

esp_err_t app_action_get_latest(app_action_job_snapshot_t *snapshot)
{
    return task_action_get_latest(snapshot);
}

const char *app_action_id_to_name(app_action_id_t id)
{
    switch(id) {
    case APP_ACTION_ID_UI_NAV_BACK: return "ui_nav_back";
    case APP_ACTION_ID_UI_NAV_HOME: return "ui_nav_home";
    case APP_ACTION_ID_UI_NAV_PUSH: return "ui_nav_push";
    default: return "invalid";
    }
}
