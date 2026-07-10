#include <assert.h>
#include <string.h>

#include "app_action.h"

static esp_err_t s_dispatch_result = ESP_OK;

static esp_err_t test_dispatcher(const app_action_request_t *request, void *user_data)
{
    int *call_count = user_data;
    assert(request != NULL);
    (*call_count)++;
    return s_dispatch_result;
}

int main(void)
{
    app_action_request_t request;
    app_action_job_snapshot_t snapshot;
    app_action_job_id_t job_id = 0;
    int call_count = 0;

    assert(app_action_init() == ESP_OK);

    memset(&request, 0, sizeof(request));
    request.id = APP_ACTION_ID_UI_NAV_HOME;
    assert(app_action_submit(&request, &job_id) == ESP_ERR_INVALID_STATE);
    assert(app_action_get_job(job_id, &snapshot) == ESP_OK);
    assert(snapshot.state == APP_ACTION_JOB_STATE_FAILED);

    assert(app_action_register_ui_dispatcher(test_dispatcher, &call_count) == ESP_OK);
    request.id = APP_ACTION_ID_UI_NAV_PUSH;
    request.params.ui_navigation.page_id = 2u;
    assert(app_action_submit(&request, &job_id) == ESP_OK);
    assert(call_count == 1);
    assert(app_action_get_latest(&snapshot) == ESP_OK);
    assert(snapshot.state == APP_ACTION_JOB_STATE_DONE);

    request.params.ui_navigation.page_id = 0u;
    assert(app_action_submit(&request, &job_id) == ESP_ERR_INVALID_ARG);
    assert(call_count == 1);

    request.id = APP_ACTION_ID_WIFI_INIT;
    assert(app_action_submit(&request, &job_id) == ESP_ERR_NOT_SUPPORTED);

    s_dispatch_result = ESP_ERR_TIMEOUT;
    request.id = APP_ACTION_ID_UI_NAV_BACK;
    assert(app_action_submit(&request, &job_id) == ESP_ERR_TIMEOUT);
    assert(app_action_get_job(job_id, &snapshot) == ESP_OK);
    assert(snapshot.state == APP_ACTION_JOB_STATE_FAILED);
    assert(snapshot.result == ESP_ERR_TIMEOUT);
    return 0;
}
