#pragma once

#include "app_action.h"

esp_err_t task_action_init(void);
esp_err_t task_action_register_ui_dispatcher(app_action_ui_dispatcher_fn dispatcher, void *user_data);
esp_err_t task_action_submit(const app_action_request_t *request, app_action_job_id_t *job_id);
esp_err_t task_action_get_job(app_action_job_id_t job_id, app_action_job_snapshot_t *snapshot);
esp_err_t task_action_get_latest(app_action_job_snapshot_t *snapshot);
